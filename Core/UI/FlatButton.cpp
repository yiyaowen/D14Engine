#include "Precompile.h"

#include "UI/FlatButton.h"

namespace d14engine::ui
{
    bool FlatButton::OnMouseButtonHelper(MouseButtonEvent& e)
    {
        Button::OnMouseButtonHelper(e);

        if (e.status.LeftDown())
        {
            solidColorOpaque = 0.2f;
        }
        else if (e.status.LeftUp())
        {
            solidColorOpaque = 0.1f;
        }
        return false;
    }

    bool FlatButton::OnMouseEnterHelper(MouseEnterEvent& e)
    {
        Button::OnMouseEnterHelper(e);

        solidColorOpaque = 0.1f;

        return false;
    }

    bool FlatButton::OnMouseLeaveHelper(MouseLeaveEvent& e)
    {
        Button::OnMouseLeaveHelper(e);

        solidColorOpaque = 0.0f;

        return false;
    }
}