#pragma once

#include "Precompile.h"

#include "Panel.h"

namespace d14engine::ui
{
    struct ResizablePanel : Panel
    {
        ResizablePanel(
            const D2D1_RECT_F& rect,
            ComPtrParam<ID2D1Brush> brush = nullptr,
            ComPtrParam<ID2D1Bitmap1> bitmap = nullptr,
            D2D1_RECT_F resizeFrameOffset = { 6.0f, 6.0f, 6.0f, 6.0f });

        // This field decides the extension of IsHit area.
        D2D1_RECT_F resizeFrameOffset = {};

        D2D1_RECT_F ResizeFrameExtendedRect(const D2D1_RECT_F& flatRect);

        bool isLeftResizable = true, isRightResizable = true;
        bool isTopResizable = true, isBottomResizable = true;

        void SetResizable(bool value);

    protected:
        bool m_isLeftHover = false, m_isRightHover = false;
        bool m_isTopHover = false, m_isBottomHover = false;

        bool m_isLeftSizing = false, m_isRightSizing = false;
        bool m_isTopSizing = false, m_isBottomSizing = false;

        // The sizing event will be triggered when mouse cursor drags the resize-frames;
        // this field stores the relative offset from the held point to window boundary.
        D2D1_POINT_2F m_sizingOffset = {};

        // The mouse cursor might dashes out of the boundary during special operations,
        // which can trigger some children events unexpectedly. Set this field to true
        // when moving or resizing the panel to prevent children from responding events.
        bool m_skipDeliverNextMouseMoveEventToChildren = false;

    public:
        // Override interface methods.

        // Panel
        bool IsHit(Event::Point& p) override;

        bool OnMouseButtonHelper(MouseButtonEvent& e) override;

        bool OnMouseMoveHelper(MouseMoveEvent& e) override;
    };
}