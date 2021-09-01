/*
 *  Sticky Window Snapper Class
 *  Copyright (C) 2021 Pedro López-Cabanillas <plcl@users.sourceforge.net>
 *  Copyright (C) 2014 mmbob (Nicholas Cook)
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <algorithm>
#include <dwmapi.h>
#include "winsnap.h"

Edge::Edge(int position, int start, int end) : Position(position), Start(start), End(end)
{ }

bool Edge::operator ==(const Edge& other) const
{
	return Position == other.Position && Start == other.Start && End == other.End;
}

bool WinSnap::HandleMessage(void *message)
{
    MSG* msg = static_cast<MSG*>(message);
    if (this->m_enabled) {
        this->m_window = msg->hwnd;
        switch (msg->message)
        {
        case WM_SIZING:
            return HandleSizing(*reinterpret_cast<RECT*>(msg->lParam), static_cast<int>(msg->wParam));

        case WM_MOVING:
            return HandleMoving(*reinterpret_cast<RECT*>(msg->lParam));

        case WM_ENTERSIZEMOVE:
            return HandleEnterSizeMove();

        case WM_EXITSIZEMOVE:
            return HandleExitSizeMove();

        default:
            break;
        }
    }
    return false;
}

bool WinSnap::IsEnabled() const
{
    return m_enabled;
}

void WinSnap::SetEnabled(const bool enabled)
{
    m_enabled = enabled;
}

bool WinSnap::HandleEnterSizeMove()
{
    for(auto& edgeList : m_edges)
		edgeList.clear();

	// Pass "this" as the parameter
	EnumDisplayMonitors(nullptr, nullptr, [](HMONITOR monitorHandle, HDC, LPRECT, LPARAM param) -> BOOL
	{
		MONITORINFOEX monitorInfo;
		monitorInfo.cbSize = sizeof(monitorInfo);
		if (GetMonitorInfo(monitorHandle, &monitorInfo) == 0)
            return false;

		// AddRectToEdges adds edges for the outside of a rectangle, which works well
		// for windows.  For monitors, however, we want to snap to the inside, so we 
		// swap the opposite edges.
		std::swap(monitorInfo.rcWork.left, monitorInfo.rcWork.right);
		std::swap(monitorInfo.rcWork.top, monitorInfo.rcWork.bottom);
        reinterpret_cast<WinSnap*>(param)->AddRectToEdges(monitorInfo.rcWork);

        return true;
	}, (LPARAM) this);


	struct Param
	{
        WinSnap* _this;
		std::list<HRGN> windowRegions;
	};

	Param param;
	param._this = this;

	EnumWindows([](HWND windowHandle, LPARAM _param) -> BOOL
	{
		// We don't want non-application windows or application windows the user can't see.
		auto param = reinterpret_cast<Param*>(_param);
        if (windowHandle == param->_this->m_window)
            return true;
		if (!IsWindowVisible(windowHandle))
            return true;
		if (IsIconic(windowHandle))
            return true;

		int styles = (int) GetWindowLongPtr(windowHandle, GWL_STYLE);
		if ((styles & WS_CHILD) != 0 || (styles & WS_CAPTION) == 0)
            return true;
		int extendedStyles = (int) GetWindowLongPtr(windowHandle, GWL_EXSTYLE);
        if (/*(extendedStyles & WS_EX_TOOLWINDOW) != 0 ||*/ (extendedStyles & WS_EX_NOACTIVATE) != 0)
            return true;
		// Ignore the window class 'ApplicationFrameWindow'
		if (GetClassWord(windowHandle, GCW_ATOM) == 0xC194)
            return true;

		RECT thisRect;
		GetWindowRect(windowHandle, &thisRect);
		HRGN thisRegion = CreateRectRgnIndirect(&thisRect);

		// Since EnumWindows enumerates from the highest z-order to the lowest, we
		// can just check to see if this window is covered by any previous ones.
		bool isUserVisible = true;
        for(HRGN region : param->windowRegions)
		{
			if (CombineRgn(thisRegion, thisRegion, region, RGN_DIFF) == NULLREGION)
			{
				isUserVisible = false;
				break;
			}
		}

		if (isUserVisible)
		{
			param->windowRegions.push_back(thisRegion);

			// Maximized windows by definition cover the whole work area, so the
			// only snap edges they would have are on the outside of the monitor.
			if (!IsZoomed(windowHandle)) {
				RECT frame, border;
                if (S_OK == DwmGetWindowAttribute(windowHandle, DWMWA_EXTENDED_FRAME_BOUNDS, &frame, sizeof(RECT))) {
                    border.left = frame.left - thisRect.left;
                    border.top = frame.top - thisRect.top;
                    border.right = thisRect.right - frame.right;
                    border.bottom = thisRect.bottom - frame.bottom;
                    thisRect.left += border.left;
                    thisRect.top += border.top;
                    thisRect.right -= border.right;
                    thisRect.bottom -= border.bottom;
                }
				param->_this->AddRectToEdges(thisRect);
			}
		}
		else
			DeleteObject(thisRegion);

        return true;
	}, (LPARAM) &param);

    for (HRGN region : param.windowRegions)
	{
		DeleteObject(region);
	}

    for (auto& edgeList : m_edges)
	{
		edgeList.sort([](const Edge& _1, const Edge& _2) -> bool { return _1.Position < _2.Position; });
		edgeList.erase(std::unique(edgeList.begin(), edgeList.end()), edgeList.end());
	}

    RECT bounds, frame;
    GetWindowRect(m_window, &bounds);
	GetCursorPos(&m_originalCursorOffset);
    m_originalCursorOffset.x -= bounds.left;
    m_originalCursorOffset.y -= bounds.top;

    if (S_OK == DwmGetWindowAttribute(m_window, DWMWA_EXTENDED_FRAME_BOUNDS, &frame, sizeof(RECT))) {
        m_border.left = frame.left - bounds.left;
        m_border.top = frame.top - bounds.top;
        m_border.right = bounds.right - frame.right;
        m_border.bottom = bounds.bottom - frame.bottom;
    }
	return true;
}

bool WinSnap::HandleExitSizeMove()
{
	m_inProgress = false;

	return true;
}

bool WinSnap::HandleMoving(RECT& bounds)
{
	// The difference between the cursor position and the top-left corner of the dragged window.
	// This is normally constant while dragging a window, but when near an edge that we snap to,
	// this changes.
    POINT cursorOffset;
	GetCursorPos(&cursorOffset);
	cursorOffset.x -= bounds.left;
	cursorOffset.y -= bounds.top;

	// While we are snapping a window, the window displayed to the user is not the "real" location
	// of the window, i.e. where it would be if we hadn't snapped it.
    RECT realBounds;
	if (m_inProgress)
	{
        POINT offsetDiff{ cursorOffset.x - m_originalCursorOffset.x, cursorOffset.y - m_originalCursorOffset.y };
		SetRect(&realBounds, bounds.left + offsetDiff.x, bounds.top + offsetDiff.y,
				bounds.right + offsetDiff.x, bounds.bottom + offsetDiff.y);
	}
	else
		realBounds = bounds;

    int boundsEdges[Side::Count]{ realBounds.left, realBounds.top, realBounds.right, realBounds.bottom };
    int snapEdges[Side::Count]{ SHRT_MIN, SHRT_MIN, SHRT_MAX, SHRT_MAX };
    bool snapDirections[Side::Count]{ false, false, false, false };

	for (int i = 0; i < Side::Count; ++i)
	{
		int snapPosition;
        if (CanSnapEdge(boundsEdges, (Side) i, &snapPosition))
		{
			snapDirections[i] = true;
			snapEdges[i] = snapPosition;
		}
	}

	if ((GetKeyState(VK_SHIFT) & 0x8000) == 0 && (snapDirections[0] || snapDirections[1] || snapDirections[2] || snapDirections[3]))
	{
		if (!m_inProgress)
			m_inProgress = true;

		RECT snapRect = { snapEdges[0] - m_border.left, snapEdges[1] - m_border.top, snapEdges[2] + m_border.right, snapEdges[3] + m_border.bottom };
		SnapToRect(&bounds, snapRect, true, snapDirections[0], snapDirections[1], snapDirections[2], snapDirections[3]);

		return true;
	}
	else if (m_inProgress)
	{
		m_inProgress = false;
		bounds = realBounds;
		return true;
	}

	return false;
}

bool WinSnap::HandleSizing(RECT& bounds, int which)
{
    bool allowSnap[Side::Count]{ true, true, true, true };
	allowSnap[Side::Left] = (which == WMSZ_LEFT || which == WMSZ_TOPLEFT || which == WMSZ_BOTTOMLEFT);
	allowSnap[Side::Top] = (which == WMSZ_TOP || which == WMSZ_TOPLEFT || which == WMSZ_TOPRIGHT);
	allowSnap[Side::Right] = (which == WMSZ_RIGHT || which == WMSZ_TOPRIGHT || which == WMSZ_BOTTOMRIGHT);
	allowSnap[Side::Bottom] = (which == WMSZ_BOTTOM || which == WMSZ_BOTTOMLEFT || which == WMSZ_BOTTOMRIGHT);

	// The difference between the cursor position and the top-left corner of the dragged window.
	// This is normally constant while dragging a window, but when near an edge that we snap to,
	// this changes.
    POINT cursorOffset;
	GetCursorPos(&cursorOffset);
	cursorOffset.x -= bounds.left;
	cursorOffset.y -= bounds.top;

    int boundsEdges[Side::Count]{ bounds.left, bounds.top, bounds.right, bounds.bottom };
    int snapEdges[Side::Count]{ SHRT_MIN, SHRT_MIN, SHRT_MAX, SHRT_MAX };
    bool snapDirections[Side::Count]{ false, false, false, false };

	for (int i = 0; i < Side::Count; ++i)
	{
		if (!allowSnap[i])
			continue;
		int snapPosition;
        if (CanSnapEdge(boundsEdges, (Side) i, &snapPosition))
		{
			snapDirections[i] = true;
			snapEdges[i] = snapPosition;
		}
	}

	if ((GetKeyState(VK_SHIFT) & 0x8000) == 0 && (snapDirections[0] || snapDirections[1] || snapDirections[2] || snapDirections[3]))
	{
		if (!m_inProgress)
			m_inProgress = true;

		RECT snapRect = { snapEdges[0] - m_border.left, snapEdges[1] - m_border.top, snapEdges[2] + m_border.right, snapEdges[3] + m_border.bottom };
		SnapToRect(&bounds, snapRect, false, snapDirections[0], snapDirections[1], snapDirections[2], snapDirections[3]);
		return true;
	}
	else if (m_inProgress)
	{
		m_inProgress = false;

		return true;
	}

	return false;
}

// Breaks down a rect into 4 edges which are added to the global list of edges to snap to.
void WinSnap::AddRectToEdges(const RECT& rect)
{
	int startX = std::min<>(rect.left, rect.right);
	int endX = std::max<>(rect.left, rect.right);
	int startY = std::min<>(rect.top, rect.bottom);
	int endY = std::max<>(rect.top, rect.bottom);

	m_edges[Side::Left].push_front(Edge(rect.right, startY, endY));
	m_edges[Side::Right].push_front(Edge(rect.left, startY, endY));
	m_edges[Side::Top].push_front(Edge(rect.bottom, startX, endX));
	m_edges[Side::Bottom].push_front(Edge(rect.top, startX, endX));
}

void WinSnap::SnapToRect(RECT* bounds, const RECT& rect, bool retainSize, bool left, bool top, bool right, bool bottom) const
{
	if (left && right)
	{
		bounds->left = rect.left;
		bounds->right = rect.right;
	}
	else if (left)
	{
		if (retainSize)
			bounds->right += rect.left - bounds->left;
		bounds->left = rect.left;
	}
	else if (right)
	{
		if (retainSize)
			bounds->left += rect.right - bounds->right;
		bounds->right = rect.right;
	}

	if (top && bottom)
	{
		bounds->top = rect.top;
		bounds->bottom = rect.bottom;
	}
	else if (top)
	{
		if (retainSize)
			bounds->bottom += rect.top - bounds->top;
		bounds->top = rect.top;
	}
	else if (bottom)
	{
		if (retainSize)
			bounds->top += rect.bottom - bounds->bottom;
		bounds->bottom = rect.bottom;
	}
}

bool WinSnap::CanSnapEdge(int boundsEdges[Side::Count], Side which, int* snapPosition) const
{
    for (const Edge& edge : m_edges[which])
	{
		int edgeDistance = edge.Position - boundsEdges[which];
		// Since each edge list is sorted, if the snap edge is past our bound's edge, we know that none of the other edges will work.
		if (edgeDistance >= SNAP_DISTANCE)
			break;
		else if (edgeDistance > -SNAP_DISTANCE)
		{
			// The modulos get the position of the edges perpendicular to the bound edge we are working on.
			int min = std::min<>(boundsEdges[(which + 1) % Side::Count], boundsEdges[(which + 3) % Side::Count]);
			int max = std::max<>(boundsEdges[(which + 1) % Side::Count], boundsEdges[(which + 3) % Side::Count]);
			if (max > edge.Start && min < edge.End)
			{
				*snapPosition = edge.Position;
				return true;
			}
		}
	}
	return false;
}
