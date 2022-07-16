#include "Common/Precompile.h"

#include "UIKit/ClickablePanel.h"

namespace d14engine::uikit
{
    void ClickablePanel::OnMouseButtonPress(Event& e)
    {
        if (f_onMouseButtonPressOverride)
        {
            f_onMouseButtonPressOverride(this, e);
        }
        else
        {
            if (f_onMouseButtonPressBefore) f_onMouseButtonPressBefore(this, e);

            OnMouseButtonPressHelper(e);

            if (f_onMouseButtonPressAfter) f_onMouseButtonPressAfter(this, e);
        }
    }

    void ClickablePanel::OnMouseButtonRelease(Event& e)
    {
        if (f_onMouseButtonReleaseOverride)
        {
            f_onMouseButtonReleaseOverride(this, e);
        }
        else
        {
            if (f_onMouseButtonReleaseBefore) f_onMouseButtonReleaseBefore(this, e);

            OnMouseButtonReleaseHelper(e);

            if (f_onMouseButtonReleaseAfter) f_onMouseButtonReleaseAfter(this, e);
        }
    }

    void ClickablePanel::OnMouseButtonPressHelper(Event& e)
    {
        // TODO: add clickable-panel mouse button pressed logic.
    }

    void ClickablePanel::OnMouseButtonReleaseHelper(Event& e)
    {
        // TODO: add clickable-panel mouse button released logic.
    }

    bool ClickablePanel::OnMouseButtonHelper(MouseButtonEvent& e)
    {
        OnMouseButtonWrapper(e);
        return Panel::OnMouseButtonHelper(e);
    }

    void ClickablePanel::OnMouseButtonWrapper(MouseButtonEvent& e)
    {
        Event be = {};

        if (e.status.LeftDown() || e.status.RightDown() || e.status.MiddleDown())
        {
            if (e.status.LeftDown())
            {
                m_hasLeftPressed = true;
                be.flag = Event::Flag::Left;
            }
            else if (e.status.RightDown())
            {
                m_hasRightPressed = true;
                be.flag = Event::Flag::Right;
            }
            else if (e.status.MiddleDown())
            {
                m_hasMiddlePressed = true;
                be.flag = Event::Flag::Middle;
            }
            else be.flag = Event::Flag::Unknown;

            OnMouseButtonPress(be);
        }
        else if (m_hasLeftPressed || m_hasRightPressed || m_hasMiddlePressed)
        {
            if (e.status.LeftUp() || e.status.RightUp() || e.status.MiddleUp())
            {
                if (e.status.LeftUp())
                {
                    m_hasLeftPressed = false;
                    be.flag = Event::Flag::Left;
                }
                else if (e.status.RightUp())
                {
                    m_hasRightPressed = false;
                    be.flag = Event::Flag::Right;
                }
                else if (e.status.MiddleUp())
                {
                    m_hasMiddlePressed = false;
                    be.flag = Event::Flag::Middle;
                }
                else be.flag = Event::Flag::Unknown;

                OnMouseButtonRelease(be);
            }
        }
    }

    bool ClickablePanel::OnMouseLeaveHelper(MouseMoveEvent& e)
    {
        OnMouseLeaveWrapper(e);
        return Panel::OnMouseLeaveHelper(e);
    }
    void ClickablePanel::OnMouseLeaveWrapper(MouseMoveEvent& e)
    {
        m_hasLeftPressed = m_hasRightPressed = m_hasMiddlePressed = false;
    }

    void ClickablePanel::SetEnabled(bool value)
    {
        Panel::SetEnabled(value);
        SetEnabledWrapper(value);
    }

    void ClickablePanel::SetEnabledWrapper(bool value)
    {
        m_hasLeftPressed = m_hasRightPressed = m_hasMiddlePressed = false;
    }
}