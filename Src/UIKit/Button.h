#pragma once

#include "Common/Precompile.h"

#include "UIKit/Label.h"
#include "UIKit/SolidStyle.h"
#include "UIKit/StrokeStyle.h"

namespace d14engine::uikit
{
    struct Button : Panel, SolidStyle, StrokeStyle
    {
        Button(
            WstrParam text,
            const D2D1_RECT_F& rect,
            float roundRadius = 0.0f,
            ComPtrParam<ID2D1Bitmap1> icon = nullptr);

        SharedPtr<Label> textLabel = {};

        // This field decides the rectangle of the icon bitmap in self-coordinate.
        D2D1_RECT_F iconRect = {};

        void OnInitializeFinish() override;

    public:
        // The appearance settings will be updated dynamically in each render pass.

        enum class State { Idle, Hover, Down, Count };

        struct Appearance
        {
            D2D1_COLOR_F backgroundColor = {};
            float backgroundOpacity = {};

            ComPtr<ID2D1Bitmap1> bitmap = {};
            float bitmapOpacity = {};

            D2D1_COLOR_F foregroundColor = {};
            float foregroundOpacity = {};

            D2D1_COLOR_F strokeColor = {};
            float strokeOpacity = {};
            float strokeWidth = {};
        }
        appearances[(size_t)State::Count] = {};

    protected:
        void UpdateAppearanceSetting(State state);

    protected:
        // We have to introduce this for the button since there's no capture mechanism
        // in the UI event system, which can cause the following unexpected situation:
        // the mouse button was pressed and held outside the button and then moved into;
        // if the button is released at this time, the button-up will be triggered,
        // which is obviously not the result we want. To check the real click event,
        // we must ensure HasxxxPressed returns true when button-up event encountered.
        bool m_hasLeftPressed = false, m_hasRightPressed = false, m_hasMiddlePressed = false;

    public:
        // To check the button-up event, write the logic in f_onMouseButtonBefore
        // and make sure both button's HasxxxPressed and event's xxxUp return true.
        // We recommend to use the ready-made OnPress and OnRelease callbacks below.
        bool HasLeftPressed() { return m_hasLeftPressed; }
        bool HasRightPressed() { return m_hasLeftPressed; }
        bool HasMiddlePressed() { return m_hasLeftPressed; }

        struct Event : uikit::Event
        {
            enum class Flag
            {
                Unknown,
                Left,
                Right,
                Middle
            }
            flag = Flag::Unknown;

            bool Left() { return flag == Flag::Left; }
            bool Right() { return flag == Flag::Right; }
            bool Middle() { return flag == Flag::Middle; }
        };

        void OnPress(Event& e);

        Function<void(Button*,Event&)>
            f_onPressOverride = {},
            f_onPressBefore = {},
            f_onPressAfter = {};

        void OnRelease(Event& e);

        Function<void(Button*,Event&)>
            f_onReleaseOverride = {},
            f_onReleaseBefore = {},
            f_onReleaseAfter = {};

    protected:
        virtual void OnPressHelper(Event& e);
        virtual void OnReleaseHelper(Event& e);

    public:
        // Override interface methods.

        // IDrawObject2D
        void OnRendererDrawD2D1ObjectHelper(Renderer* rndr) override;

        // Panel
        void OnSizeHelper(SizeEvent& e) override;

        void OnChangeThemeHelper(WstrViewParam themeName) override;

        bool OnMouseButtonHelper(MouseButtonEvent& e) override;

        bool OnMouseEnterHelper(MouseMoveEvent& e) override;

        bool OnMouseLeaveHelper(MouseMoveEvent& e) override;
    };
}