/*
    MIDI Virtual Piano Keyboard
    Copyright (C) 2008-2024, Pedro Lopez-Cabanillas <plcl@users.sf.net>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; If not, see <http://www.gnu.org/licenses/>.
*/

#include "nativefilter.h"
#include <QApplication>

#if defined(Q_OS_WIN)
#include <windows.h>
/* http://msdn.microsoft.com/en-us/library/ms646280(VS.85).aspx */
#elif defined(Q_OS_MAC)
#include "maceventhelper.h"
#elif defined(Q_OS_UNIX)
#include <xcb/xcb.h>
#if QT_VERSION < QT_VERSION_CHECK(6,0,0)
#include <QX11Info>
// private header, avoid if possible...
//#include <qpa/qplatformnativeinterface.h>
#else // needs Qt6 >= 6.2
#include <QGuiApplication>
#endif
#endif

#if QT_VERSION < QT_VERSION_CHECK(6,0,0)
bool NativeFilter::nativeEventFilter(const QByteArray &eventType, void *message, long *)
#else
bool NativeFilter::nativeEventFilter(const QByteArray &eventType, void *message, qintptr *)
#endif
{

    if (!m_enabled || (m_handler == nullptr)) {
        return false;
    }

    if (eventType == "xcb_generic_event_t") {
#if defined(Q_OS_LINUX)
        static xcb_timestamp_t last_rel_time = 0;
        static xcb_keycode_t last_rel_code = 0;
        static xcb_connection_t *connection = nullptr;
        bool isRepeat = false;
        if (connection == nullptr) {
#if QT_VERSION < QT_VERSION_CHECK(6,0,0)
            // private API, avoid if possible...
            //QPlatformNativeInterface *native = qApp->platformNativeInterface();
            //void *conn = native->nativeResourceForWindow(QByteArray("connection"), 0);
            //connection = reinterpret_cast<xcb_connection_t *>(conn);
            connection = QX11Info::connection();
#else // needs Qt6 >= 6.2
            if (auto x11nativeitf = qApp->nativeInterface<QNativeInterface::QX11Application>()) {
                connection = x11nativeitf->connection();
            }
#endif
        }
        xcb_generic_event_t* ev = reinterpret_cast<xcb_generic_event_t *>(message);
        switch (ev->response_type & ~0x80) {
            case XCB_KEY_PRESS:
            {
                xcb_key_press_event_t *kp = reinterpret_cast<xcb_key_press_event_t *>(ev);
                if ((last_rel_code != 0) && (last_rel_time != 0)) {
                    isRepeat = ((kp->detail == last_rel_code) && (kp->time == last_rel_time));
                    last_rel_code = 0;
                    last_rel_time = 0;
                }
                if (!isRepeat) {
                    //qDebug() << "key pressed:" << kp->detail;
                    return m_handler->handleKeyPressed(kp->detail);
                }
            }
            break;

            case XCB_KEY_RELEASE:
            {
                xcb_key_release_event_t *kr = reinterpret_cast<xcb_key_release_event_t *>(ev);
                if (connection) {
                    xcb_query_keymap_cookie_t cookie = xcb_query_keymap(connection);
                    xcb_query_keymap_reply_t *keymap = xcb_query_keymap_reply(connection, cookie, nullptr);
                    isRepeat = (keymap->keys[kr->detail / 8] & (1 << (kr->detail % 8)));
                    free(keymap);
                    last_rel_code = kr->detail;
                    last_rel_time = kr->time;
                }
                if (!isRepeat) {
                    //qDebug() << "key released:" << kr->detail;
                    return m_handler->handleKeyReleased(kr->detail);
                }
            }
            break;
        }
#endif
    } else if (eventType == "windows_dispatcher_MSG" || eventType == "windows_generic_MSG") {
#if defined(Q_OS_WIN)
        bool isRepeat = false;
        MSG* msg = static_cast<MSG *>(message);
        if (msg->message == WM_KEYDOWN || msg->message == WM_KEYUP ||
            msg->message == WM_SYSKEYDOWN || msg->message == WM_SYSKEYUP) {
            int keycode = HIWORD(msg->lParam) & 0xff;
            isRepeat = (msg->message == WM_KEYDOWN || msg->message == WM_SYSKEYDOWN) &&
                       ((HIWORD(msg->lParam) & 0x4000) != 0);
            if (!isRepeat) {
                if ( msg->message == WM_KEYDOWN || msg->message == WM_SYSKEYDOWN )
                    return m_handler->handleKeyPressed(keycode);
                    //qDebug() << "key pressed:" << keycode;
                else
                    return m_handler->handleKeyReleased(keycode);
                    //qDebug() << "key released:" << keycode;
            }
        }
#endif
    } else if (eventType == "mac_generic_NSEvent") {
#if defined(Q_OS_MAC)
        static MacEventHelper *helper = new MacEventHelper();
        helper->setNativeEvent(static_cast<void *>(message));
        if (helper->isKeyEvent()) {
            int keycode = helper->rawKeyCode();
            if (helper->isNotRepeated()) {
                if (helper->isKeyPress())
                    return m_handler->handleKeyPressed(keycode);
                    //qDebug() << "key pressed:" << keycode;
                else
                    return m_handler->handleKeyReleased(keycode);
                    //qDebug() << "key pressed:" << keycode;
            }
        }
        helper->clearNativeEvent();
#endif
    }
    return false;
}
