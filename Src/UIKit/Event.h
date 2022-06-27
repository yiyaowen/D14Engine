#pragma once

#include "Common/Precompile.h"

namespace d14engine::uikit
{
    struct Panel;

    struct Event
    {
        using Size = D2D1_SIZE_F;
        using Point = D2D1_POINT_2F;
    };

    // Space transform events are divided into self event and parent event.
    // For example: OnSize & OnParentSize, OnMove & OnParentMove ......

    struct SizeEvent : Event
    {
        Size size = {};
    };

    struct MoveEvent : Event
    {
        // This field must be (0,0) in parent event like OnParentMove,
        // since the parent would always be static relative to itself.
        Point position = {};
    };

    // Broadcast to current focused objects when trigger mouse operations.
    struct MouseEvent : Event
    {
        using CursorPoint = Point;

        CursorPoint cursorPoint = {};
    };

    struct MouseButtonEvent : MouseEvent
    {
        struct Status
        {
            enum class Flag
            {
                Unknown,
                LeftDown,
                LeftUp,
                RightDown,
                RightUp,
                MiddleDown,
                MiddleUp
            }
            flag = Flag::Unknown;

            bool LeftDown() { return flag == Flag::LeftDown; }
            bool LeftUp() { return flag == Flag::LeftUp; }
            bool RightDown() { return flag == Flag::RightDown; }
            bool RightUp() { return flag == Flag::RightUp; }
            bool MiddleDown() { return flag == Flag::MiddleDown; }
            bool MiddleUp() { return flag == Flag::MiddleUp; }
        }
        status = {};

        // This field always points to the current focused UI object. Only focusable UI object could be focused.
        // e.g. suppose the event is delivered as A --> B --> C, where A, C is not focusable and B is focusable:
        // For A, focused keeps nullptr; for B, focused changes to B; for C, focused remains B as C is not focusable.
        WeakPtr<Panel> focused = {};
    };

    struct MouseMoveEvent : MouseEvent
    {
        struct ButtonState
        {
            bool leftPressed = false;
            bool rightPressed = false;
            bool middlePressed = false;
        }
        buttonState = {};

        struct KeyState
        {
            bool ALT = false;
            bool CTRL = false;
            bool SHIFT = false;
        }
        keyState = {};

        CursorPoint lastCursorPoint = {};
    };

    struct MouseWheelEvent : MouseEvent
    {
        struct ButtonState
        {
            bool leftPressed = false;
            bool rightPressed = false;
            bool middlePressed = false;
        }
        buttonState = {};

        struct KeyState
        {
            bool CTRL = false;
            bool SHIFT = false;
        }
        keyState = {};

        int deltaCount = 0;
    };

    // Broadcast to current focused objects when trigger keyboard operations.
    struct KeyboardEvent : Event
    {
        struct Status
        {
            enum class Flag
            {
                Unknown,
                Pressed,
                Released,
            }
            flag = Flag::Unknown;

            bool Pressed() { return flag == Flag::Pressed; }
            bool Released() { return flag == Flag::Released; }
        }
        status = {};

        WPARAM vkey = VK_NONAME;

        bool LALT() { return GetAsyncKeyState(VK_LMENU) & 0x8000; }
        bool RALT() { return GetAsyncKeyState(VK_RMENU) & 0x8000; }

        bool LCTRL() { return GetAsyncKeyState(VK_LCONTROL) & 0x8000; }
        bool RCTRL() { return GetAsyncKeyState(VK_RCONTROL) & 0x8000; }

        bool LSHIFT() { return GetAsyncKeyState(VK_LSHIFT) & 0x8000; }
        bool RSHIFT() { return GetAsyncKeyState(VK_RSHIFT) & 0x8000; }
    };
}