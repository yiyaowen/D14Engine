#pragma once

#include "Precompile.h"

#include "Label.h"
#include "SolidColorStyle.h"

namespace d14engine::ui
{
    struct Button : Panel, SolidColorStyle
    {
        Button(
            WstrParam text,
            ComPtrParam<ID2D1Bitmap1> icon,
            const D2D1_RECT_F& rect,
            float roundRadius = 0.0f);

        SharedPtr<Label> textLabel;

        D2D1_RECT_F iconRect;

        void OnInitializeFinish() override;

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
        // and make sure both button's HasxxxPressed and event'sxxxUp return true.
        bool HasLeftPressed() { return m_hasLeftPressed; }
        bool HasRightPressed() { return m_hasLeftPressed; }
        bool HasMiddlePressed() { return m_hasLeftPressed; }

    public:
        // Override interface methods.

        // IDrawObject2D
        void OnRendererDrawD2D1Object(Renderer* rndr) override;

        // IUIObject
        void OnSizeHelper(SizeEvent& e) override;

        bool OnMouseButtonHelper(MouseButtonEvent& e) override;

        bool OnMouseLeaveHelper(MouseLeaveEvent& e) override;
    };
}