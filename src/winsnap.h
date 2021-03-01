#ifndef WINSNAP_H
#define WINSNAP_H

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

#include <list>
#include <Windows.h>

enum Side
{
    Left = 0,
    Top = 1,
    Right = 2,
    Bottom = 3,

    Count = 4,
};

struct Edge
{
	int Position;
	int Start;
	int End;

	Edge(int position, int start, int end);

	bool operator ==(const Edge& other) const;
};

class WinSnap
{
	// The distance at which edges will snap.
    static const int SNAP_DISTANCE = 30;
    // Snap effect is enabled?
    bool m_enabled{ true };
	// An array of sorted lists of edges which can be snapped to for each side of the window.
    std::list<Edge> m_edges[Side::Count];
    // Is the window currently being snapped?
    bool m_inProgress{ false };
    // The difference between the cursor position and the top-left of the window being dragged.
    POINT m_originalCursorOffset;
    // The window handle
    HWND m_window;
    // The window border sizes, for Windows 10 Aero theme are: 7,0,7,7
    RECT m_border{ 0,0,0,0 };

	void AddRectToEdges(const RECT& rect);
	void SnapToRect(RECT* bounds, const RECT& rect, bool retainSize, bool left, bool top, bool right, bool bottom) const;
    bool CanSnapEdge(int boundsEdges[Side::Count], Side which, int* snapPosition) const;

	bool HandleEnterSizeMove();
	bool HandleExitSizeMove();
	bool HandleMoving(RECT& bounds);
	bool HandleSizing(RECT& bounds, int which);
public:
    bool HandleMessage(void *message);
    bool IsEnabled() const;
    void SetEnabled(const bool enabled);
};

#endif // WINSNAP_H
