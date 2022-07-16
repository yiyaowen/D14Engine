#include "Common/Precompile.h"

#include "UIKit/ResizablePanel.h"

#include "Renderer/MathUtils.h"
using namespace d14engine::renderer;

#include "UIKit/Application.h"

namespace d14engine::uikit
{
    ResizablePanel::ResizablePanel(
        const D2D1_RECT_F& rect,
        ComPtrParam<ID2D1Brush> brush,
        ComPtrParam<ID2D1Bitmap1> bitmap,
        D2D1_RECT_F resizeFrameOffset)
        :
        Panel(rect, brush, bitmap),
        resizeFrameOffset(resizeFrameOffset) { }

    D2D1_RECT_F ResizablePanel::ResizeFrameExtendedRect(const D2D1_RECT_F& flatRect)
    {
        return
        {
            flatRect.left - (isLeftResizable ? resizeFrameOffset.left : 0.0f),
            flatRect.top - (isTopResizable ? resizeFrameOffset.top : 0.0f),
            flatRect.right + (isRightResizable ? resizeFrameOffset.right : 0.0f),
            flatRect.bottom + (isBottomResizable ? resizeFrameOffset.bottom : 0.0f)
        };
    }

    void ResizablePanel::SetResizable(bool value)
    {
        isLeftResizable = isTopResizable = isRightResizable = isBottomResizable = value;
    }

    bool ResizablePanel::IsHit(Event::Point& p)
    {
        return Mathu::IsOverlapped(p, ResizeFrameExtendedRect(m_absoluteRect));
    }

    bool ResizablePanel::OnMouseButtonHelper(MouseButtonEvent& e)
    {
        OnMouseButtonWrapper(e);
        return Panel::OnMouseButtonHelper(e);
    }

    void ResizablePanel::OnMouseButtonWrapper(MouseButtonEvent& e)
    {
        auto p = e.cursorPoint;

        if (e.status.LeftDown())
        {
            m_isLeftSizing = m_isLeftHover && isLeftResizable;
            m_isTopSizing = m_isTopHover && isTopResizable;
            m_isRightSizing = m_isRightHover && isRightResizable;
            m_isBottomSizing = m_isBottomHover && isBottomResizable;

            // Check whether is holding window frame.
            if (m_isLeftSizing || m_isTopSizing || m_isRightSizing || m_isBottomSizing)
            {
                m_sizingOffset = AbsoluteToSelfCoord(p);

                if (m_sizingOffset.x > 0) m_sizingOffset.x -= Width();
                if (m_sizingOffset.y > 0) m_sizingOffset.y -= Height();

                // In case the mouse dashes out of the boundary.
                PinApplicationEvents();
            }
        }
        else if (e.status.LeftUp())
        {
            // Always cancel sizing if left button up.
            if (m_isLeftSizing || m_isTopSizing || m_isRightSizing || m_isBottomSizing)
            {
                m_isLeftSizing = m_isTopSizing = m_isRightSizing = m_isBottomSizing = false;
                UnpinApplicationEvents();
            }
        }
    }

    bool ResizablePanel::OnMouseMoveHelper(MouseMoveEvent& e)
    {
        if (OnMouseMoveWrapper(e))
        {
            return Panel::OnMouseMoveHelper(e);
        }
        else return false;
    }

    bool ResizablePanel::OnMouseMoveWrapper(MouseMoveEvent& e)
    {
        auto p = e.cursorPoint;

        auto relative = AbsoluteToRelative(p);

        // The sizing point is not aligned to window boundary strictly since the sizing event
        // will be triggered when mouse cursor moves on shadow frame. This causes the point
        // actually offsets the boundary by a few pixels, which is stored in m_sizingOffset.
        relative.x -= m_sizingOffset.x;
        relative.y -= m_sizingOffset.y;

        float minWidth = MinimalWidth();
        float minHeight = MinimalHeight();

        float maxWidth = MaximalWidth();
        float maxHeight = MaximalHeight();

        // Left

        if (m_isLeftSizing && !m_isTopSizing && !m_isRightSizing && !m_isBottomSizing &&
            isLeftResizable)
        {
            float afterWidth = m_rect.right - relative.x;

            if (afterWidth < minWidth)
            {
                afterWidth = minWidth;
                relative.x = m_rect.right - minWidth;
            }
            else if (afterWidth > maxWidth)
            {
                afterWidth = maxWidth;
                relative.x = m_rect.right - maxWidth;
            }
            Transform(relative.x, m_rect.top, afterWidth, Height());

            Application::APP->MainCursor()->SetIcon(Cursor::IconIndex::HorzSize);
        }

        // Right

        else if (m_isRightSizing && !m_isLeftSizing && !m_isTopSizing && !m_isBottomSizing &&
                 isRightResizable)
        {
            float afterWidth = relative.x - m_rect.left;

            if (afterWidth < minWidth)
            {
                afterWidth = minWidth;
            }
            else if (afterWidth > maxWidth)
            {
                afterWidth = maxWidth;
            }
            Transform(m_rect.left, m_rect.top, afterWidth, Height());

            Application::APP->MainCursor()->SetIcon(Cursor::IconIndex::HorzSize);
        }

        // Top

        else if (m_isTopSizing && !m_isLeftSizing && !m_isRightSizing && !m_isBottomSizing &&
                 isTopResizable)
        {
            float afterHeight = m_rect.bottom - relative.y;

            if (afterHeight < minHeight)
            {
                afterHeight = minHeight;
                relative.y = m_rect.bottom - minHeight;
            }
            else if (afterHeight > maxHeight)
            {
                afterHeight = maxHeight;
                relative.y = m_rect.bottom - maxHeight;
            }
            Transform(m_rect.left, relative.y, Width(), afterHeight);

            Application::APP->MainCursor()->SetIcon(Cursor::IconIndex::VertSize);
        }

        // Bottom

        else if (m_isBottomSizing && !m_isLeftSizing && !m_isTopSizing && !m_isRightSizing &&
                 isBottomResizable)
        {
            float afterHeight = relative.y - m_rect.top;

            if (afterHeight < minHeight)
            {
                afterHeight = minHeight;
            }
            else if (afterHeight > maxHeight)
            {
                afterHeight = maxHeight;
            }
            Transform(m_rect.left, m_rect.top, Width(), afterHeight);

            Application::APP->MainCursor()->SetIcon(Cursor::IconIndex::VertSize);
        }

        // Left Top

        else if (m_isLeftSizing && m_isTopSizing && !m_isRightSizing && !m_isBottomSizing &&
                 isLeftResizable && isTopResizable)
        {
            float afterWidth = m_rect.right - relative.x;
            float afterHeight = m_rect.bottom - relative.y;

            if (afterWidth < minWidth)
            {
                afterWidth = minWidth;
                relative.x = m_rect.right - minWidth;
            }
            else if (afterWidth > maxWidth)
            {
                afterWidth = maxWidth;
                relative.x = m_rect.right - maxWidth;
            }
            if (afterHeight < minHeight)
            {
                afterHeight = minHeight;
                relative.y = m_rect.bottom - minHeight;
            }
            else if (afterHeight > maxHeight)
            {
                afterHeight = maxHeight;
                relative.y = m_rect.bottom - maxHeight;
            }
            Transform(relative.x, relative.y, afterWidth, afterHeight);

            Application::APP->MainCursor()->SetIcon(Cursor::IconIndex::MainDiagSize);
        }

        // Left Bottom

        else if (m_isLeftSizing && m_isBottomSizing && !m_isRightSizing && !m_isTopSizing &&
                 isLeftResizable && isBottomResizable)
        {
            float afterWidth = m_rect.right - relative.x;
            float afterHeight = relative.y - m_rect.top;

            if (afterWidth < minWidth)
            {
                afterWidth = minWidth;
                relative.x = m_rect.right - minWidth;
            }
            else if (afterWidth > maxWidth)
            {
                afterWidth = maxWidth;
                relative.x = m_rect.right - maxWidth;
            }
            if (afterHeight < minHeight)
            {
                afterHeight = minHeight;
            }
            else if (afterHeight > maxHeight)
            {
                afterHeight = maxHeight;
            }
            Transform(relative.x, m_rect.top, afterWidth, afterHeight);

            Application::APP->MainCursor()->SetIcon(Cursor::IconIndex::BackDiagSize);
        }

        // Right Top

        else if (m_isRightSizing && m_isTopSizing && !m_isLeftSizing && !m_isBottomSizing &&
                 isRightResizable && isTopResizable)
        {
            float afterWidth = relative.x - m_rect.left;
            float afterHeight = m_rect.bottom - relative.y;

            if (afterWidth < minWidth)
            {
                afterWidth = minWidth;
            }
            else if (afterWidth > maxWidth)
            {
                afterWidth = maxWidth;
            }
            if (afterHeight < minHeight)
            {
                afterHeight = minHeight;
                relative.y = m_rect.bottom - minHeight;
            }
            else if (afterHeight > maxHeight)
            {
                afterHeight = maxHeight;
                relative.y = m_rect.bottom - maxHeight;
            }
            Transform(m_rect.left, relative.y, afterWidth, afterHeight);

            Application::APP->MainCursor()->SetIcon(Cursor::IconIndex::BackDiagSize);
        }

        // Right Bottom

        else if (m_isRightSizing && m_isBottomSizing && !m_isLeftSizing && !m_isTopSizing &&
                 isRightResizable && isBottomResizable)
        {
            float afterWidth = relative.x - m_rect.left;
            float afterHeight = relative.y - m_rect.top;

            if (afterWidth < minWidth)
            {
                afterWidth = minWidth;
            }
            else if (afterWidth > maxWidth)
            {
                afterWidth = maxWidth;
            }
            if (afterHeight < minHeight)
            {
                afterHeight = minHeight;
            }
            else if (afterHeight > maxHeight)
            {
                afterHeight = maxHeight;
            }
            Transform(m_rect.left, m_rect.top, afterWidth, afterHeight);

            Application::APP->MainCursor()->SetIcon(Cursor::IconIndex::MainDiagSize);
        }

        // No Sizing

        else // Update sizing boundary states & icons.
        {
            m_isLeftHover = m_isTopHover = m_isRightHover = m_isBottomHover = false;

            if ((p.x < m_absoluteRect.left && p.y < m_absoluteRect.top) ||
                (p.x > m_absoluteRect.right && p.y > m_absoluteRect.bottom))
            {
                if (p.x < m_absoluteRect.left)
                {
                    m_isLeftHover = m_isTopHover = true;
                }
                else m_isRightHover = m_isBottomHover = true;

                if ((m_isLeftHover && m_isTopHover && isLeftResizable && isTopResizable) ||
                    (m_isRightHover && m_isBottomHover && isRightResizable && isBottomResizable))
                {
                    Application::APP->MainCursor()->SetIcon(Cursor::IconIndex::MainDiagSize);
                }
            }
            else if ((p.x < m_absoluteRect.left && p.y > m_absoluteRect.bottom) ||
                     (p.x > m_absoluteRect.right && p.y < m_absoluteRect.top))
            {
                if (p.x < m_absoluteRect.left)
                {
                    m_isLeftHover = m_isBottomHover = true;
                }
                else m_isRightHover = m_isTopHover = true;

                if ((m_isLeftHover && m_isBottomHover && isLeftResizable && isBottomResizable) ||
                    (m_isRightHover && m_isTopHover && isRightResizable && isTopResizable))
                {
                    Application::APP->MainCursor()->SetIcon(Cursor::IconIndex::BackDiagSize);
                }
            }
            else if (p.x < m_absoluteRect.left || p.x > m_absoluteRect.right)
            {
                if (p.x < m_absoluteRect.left)
                {
                    m_isLeftHover = true;
                }
                else m_isRightHover = true;

                if ((m_isLeftHover && isLeftResizable) || (m_isRightHover && isRightResizable))
                {
                    Application::APP->MainCursor()->SetIcon(Cursor::IconIndex::HorzSize);
                }
            }
            else if (p.y < m_absoluteRect.top || p.y > m_absoluteRect.bottom)
            {
                if (p.y < m_absoluteRect.top)
                {
                    m_isTopHover = true;
                }
                else m_isBottomHover = true;

                if ((m_isTopHover && isTopResizable) || (m_isBottomHover && isBottomResizable))
                {
                    Application::APP->MainCursor()->SetIcon(Cursor::IconIndex::VertSize);
                }
            }
        }

        // Decide whether to handle children events in Panel.

        if (m_isLeftSizing || m_isTopSizing || m_isRightSizing || m_isBottomSizing ||
            m_skipDeliverNextMouseMoveEventToChildren) // Take sizing into account.
        {
            m_skipDeliverNextMouseMoveEventToChildren = false;
            return false;
        }
        else return true;
    }
}