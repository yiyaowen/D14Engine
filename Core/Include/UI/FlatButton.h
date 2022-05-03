#pragma once

#include "Precompile.h"

#include "Button.h"
#include "Label.h"

namespace d14engine::ui
{
    struct FlatButton : Button
    {
        using Button::Button;

    public:
        // Override interface methods.

        // IUIObject
        bool OnMouseButtonHelper(MouseButtonEvent& e) override;

        bool OnMouseEnterHelper(MouseEnterEvent& e) override;

        bool OnMouseLeaveHelper(MouseLeaveEvent& e) override;
    };
}