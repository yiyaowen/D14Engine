#pragma once

#include "Precompile.h"

#include "Event.h"
#include "Label.h"
#include "SolidStyle.h"
#include "StrokeStyle.h"

namespace d14engine::ui
{
    struct Button : Panel, SolidStyle, StrokeStyle
    {
        Button(
            WstrParam text,
            const D2D1_RECT_F& rect,
            float roundRadius = 0.0f,
            ComPtrParam<ID2D1Bitmap1> icon = nullptr);

        SharedPtr<Label> textLabel;

        D2D1_RECT_F iconRect;

        void OnInitializeFinish() override;

    public:
        // The appearance settings will be updated dynamically in each render pass.

        enum class State { Idle, Hover, Down, Count };

        struct Appearance
        {
            D2D1_COLOR_F solidColor = (D2D1::ColorF)D2D1::ColorF::White;
            float solidColorOpaque = 1.0f;

            ComPtr<ID2D1Bitmap1> bitmap;
            float bitmapOpaque = 1.0f;

            D2D1_COLOR_F textColor = (D2D1::ColorF)D2D1::ColorF::Black;
            float textColorOpaque = 1.0f;

            D2D1_COLOR_F strokeColor = (D2D1::ColorF)D2D1::ColorF::Black;
            float strokeColorOpaque = 1.0f;
            float strokeWidth = 0.0f;
        }
        appearances[(size_t)State::Count];

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

        struct Event : ui::Event
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
        virtual void OnPressHelper(Event& e);

        Function<void(Button*,Event&)>
            f_onPressOverride,
            f_onPressBefore,
            f_onPressAfter;

        void OnRelease(Event& e);
        virtual void OnReleaseHelper(Event& e);

        Function<void(Button*,Event&)>
            f_onReleaseOverride,
            f_onReleaseBefore,
            f_onReleaseAfter;

    public:
        // Override interface methods.

        // IDrawObject2D
        void OnRendererDrawD2D1Object(Renderer* rndr) override;

        // Panel
        void OnSizeHelper(SizeEvent& e) override;

        bool OnMouseButtonHelper(MouseButtonEvent& e) override;

        bool OnMouseEnterHelper(MouseEnterEvent& e) override;

        bool OnMouseLeaveHelper(MouseLeaveEvent& e) override;
    };
}