#pragma once

#include "Common/Precompile.h"

#include "UIKit/Panel.h"

namespace d14engine::uikit
{
    struct ClickablePanel : virtual Panel
    {
        using Panel::Panel;

    protected:
        // We have to introduce this for the clickable as there's no capture mechanism
        // in the UI event system, which can cause the following unexpected situation:
        // the mouse button was pressed and held outside the panel and then moved into;
        // if the button is released at this time, the button-up will be triggered,
        // which is obviously not the result we want. To check the real click event,
        // we must ensure m_hasXxxPressed is true when button-up event encountered.
        bool m_hasLeftPressed = false, m_hasRightPressed = false, m_hasMiddlePressed = false;

    public:
        struct Event : uikit::Event
        {
            enum class Flag { Unknown, Left, Right, Middle } flag = Flag::Unknown;

            bool Left() { return flag == Flag::Left; }
            bool Right() { return flag == Flag::Right; }
            bool Middle() { return flag == Flag::Middle; }
        };

        void OnMouseButtonPress(Event& e);

        Function<void(ClickablePanel*,Event&)>
            f_onMouseButtonPressOverride = {},
            f_onMouseButtonPressBefore = {},
            f_onMouseButtonPressAfter = {};

        void OnMouseButtonRelease(Event& e);

        Function<void(ClickablePanel*,Event&)>
            f_onMouseButtonReleaseOverride = {},
            f_onMouseButtonReleaseBefore = {},
            f_onMouseButtonReleaseAfter = {};

    protected:
        virtual void OnMouseButtonPressHelper(Event& e);
        virtual void OnMouseButtonReleaseHelper(Event& e);

    protected:
        // Override interface methods.

        // Panel
        bool OnMouseButtonHelper(MouseButtonEvent& e) override;
        void OnMouseButtonWrapper(MouseButtonEvent& e);

        bool OnMouseLeaveHelper(MouseMoveEvent& e) override;
        void OnMouseLeaveWrapper(MouseMoveEvent& e);

        /*
        * Introduce OnXxxWrapper to avoid duplicated call of
        * base class's virtual methods in diamond inheritance.
        * 
        * For example, suppose we define a virtual method called "func()" in base class "A":
        * 
        * A { virtual func() { A's work... } }
        * B1 -> A { func() override { A::func(); B1's work... } }
        * B2 -> A { func() override { A::func(); B2's work... } }
        * C -> B1, B2 { func() override { ???::func(); C's work... } }
        * 
        * If we call B1::func() and B2::func() in C, then A::func() will be called twice,
        * which is obviously not we expect to be. Introduce an intermidiate to solve this problem:
        * 
        * B1 -> A { ... funcWrapper() { B1's work... } }
        * B2 -> A { ... funcWrapper() { B2's work... } }
        * C -> B1, B2 { func() override { A::func(); B1::funcWrapper(); B2::funcWrapper(); C's work... } }
        */

    public:
        void SetEnabled(bool value) override;
        void SetEnabledWrapper(bool value);
    };
}