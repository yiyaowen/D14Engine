#include "Precompile.h"

#include "UI/Panel.h"

#include "Renderer/MathUtils.h"
#include "UI/Application.h"

namespace d14engine::ui
{
    bool Panel::IsD2D1ObjectVisible()
    {
        return m_isVisible;
    }

    void Panel::SetD2D1ObjectVisible(bool value)
    {
        m_isVisible = value;
    }

    void Panel::OnRendererUpdateObject2D(Renderer* rndr)
    {
        UpdateChildrenObjects(rndr);
    }

    void Panel::OnRendererDrawD2D1Layer(Renderer* rndr)
    {
        DrawChildrenLayers(rndr);
    }

    void Panel::OnRendererDrawD2D1Object(Renderer* rndr)
    {
        DrawBackground(rndr);
        DrawChildrenObjects(rndr);
    }

    Panel::Panel(
        const D2D1_RECT_F& rect,
        ComPtrParam<ID2D1Brush> brush,
        ComPtrParam<ID2D1Bitmap1> bitmap)
        :
        m_rect(rect),
        brush(brush),
        bitmap(bitmap)
    {
        UpdateAbsoluteRect();
    }

    bool Panel::IsHit(Event::Point& p)
    {
        return Mathu::IsOverlapped(p, m_absoluteRect);
    }

    D2D1_SIZE_F Panel::MinimalSize()
    {
        return { MinimalWidth(), MinimalHeight() };
    }

    float Panel::MinimalWidth()
    {
        return 0.0f;
    }

    float Panel::MinimalHeight()
    {
        return 0.0f;
    }

    void Panel::RegisterDrawObjects()
    {
        Application::RNDR->AddDrawObject2D(shared_from_this());
    }

    void Panel::UnregisterDrawObjects()
    {
        Application::RNDR->RemoveDrawObject2D(shared_from_this());
    }

    void Panel::RegisterApplicationEvents()
    {
        Application::APP->AddUIObject(shared_from_this());
    }

    void Panel::UnregisterApplicationEvents()
    {
        Application::APP->RemoveUIObject(shared_from_this());
    }

    void Panel::OnSize(SizeEvent& e)
    {
        if (f_onSizeOverride)
        {
            f_onSizeOverride(this, e);
        }
        else
        {
            if (f_onSizeBefore) f_onSizeBefore(this, e);

            OnSizeHelper(e);

            if (f_onSizeAfter) f_onSizeAfter(this, e);
        }
    }

    void Panel::OnSizeHelper(SizeEvent& e)
    {
        for (auto& child : m_children)
        {
            child->OnParentSize(e);
        }
    }

    void Panel::OnParentSize(SizeEvent& e)
    {
        if (f_onParentSizeOverride)
        {
            f_onParentSizeOverride(this, e);
        }
        else
        {
            if (f_onParentSizeBefore) f_onParentSizeBefore(this, e);

            OnParentSizeHelper(e);

            if (f_onParentSizeAfter) f_onParentSizeAfter(this, e);
        }
    }

    void Panel::OnParentSizeHelper(SizeEvent& e)
    {
        // TODO: add panel parent sizing logic.
    }

    void Panel::OnMove(MoveEvent& e)
    {
        if (f_onMoveOverride)
        {
            f_onMoveOverride(this, e);
        }
        else
        {
            if (f_onMoveBefore) f_onMoveBefore(this, e);

            OnMoveHelper(e);

            if (f_onMoveAfter) f_onMoveAfter(this, e);
        }
    }

    void Panel::OnMoveHelper(MoveEvent& e)
    {
        MoveEvent me = {};
        // Left top is always (0,0)!
        me.position = { 0.0f, 0.0f };

        for (auto& child : m_children)
        {
            child->OnParentMove(me);
        }
    }

    void Panel::OnParentMove(MoveEvent& e)
    {
        if (f_onParentMoveOverride)
        {
            f_onParentMoveOverride(this, e);
        }
        else
        {
            if (f_onParentMoveBefore) f_onParentMoveBefore(this, e);

            OnParentMoveHelper(e);

            if (f_onParentMoveAfter) f_onParentMoveAfter(this, e);
        }
    }

    void Panel::OnParentMoveHelper(MoveEvent& e)
    {
        UpdateAbsoluteRect();

        MoveEvent me = {};
        me.position = { m_rect.left, m_rect.top };

        OnMove(me);
    }

    bool Panel::OnMouseButton(MouseButtonEvent& e)
    {
        if (f_onMouseButtonOverride)
        {
            return f_onMouseButtonOverride(this, e);
        }
        else
        {
            // Return boolean is discarded for before event.
            if (f_onMouseButtonBefore) f_onMouseButtonBefore(this, e);

            bool value = OnMouseButtonHelper(e);

            return f_onMouseButtonAfter ? f_onMouseButtonAfter(this, e) : value;
        }
    }

    bool Panel::OnMouseButtonHelper(MouseButtonEvent& e)
    {
        // Only broadcast the event to those hit widgets.
        for (auto& child : m_hitChildren)
        {
            if (child->IsMouseButtonSensitive())
            {
                // Note the return boolean means whether to continue delivering the event.
                if (!child->OnMouseButton(e)) break;
            }
        }
        for (auto& child : m_pinnedChildren)
        {
            if (child->IsMouseButtonSensitive())
            {
                if (!child->OnMouseButton(e)) break;
            }
        }
        return false;
    }

    bool Panel::OnMouseEnter(MouseEnterEvent& e)
    {
        if (f_onMouseEnterOverride)
        {
            return f_onMouseEnterOverride(this, e);
        }
        else
        {
            if (f_onMouseEnterBefore) f_onMouseEnterBefore(this, e);

            bool value = OnMouseEnterHelper(e);

            return f_onMouseEnterAfter ? f_onMouseEnterAfter(this, e) : value;
        }
    }

    bool Panel::OnMouseEnterHelper(MouseEnterEvent& e)
    {
        return false;
    }

    bool Panel::OnMouseMove(MouseMoveEvent& e)
    {
        if (f_onMouseMoveOverride)
        {
            return f_onMouseMoveOverride(this, e);
        }
        else
        {
            if (f_onMouseMoveBefore) f_onMouseMoveBefore(this, e);

            bool value = OnMouseMoveHelper(e);

            return f_onMouseMoveAfter ? f_onMouseMoveAfter(this, e) : value;
        }
    }

    bool Panel::OnMouseMoveHelper(MouseMoveEvent& e)
    {
        ChildObjectPrioritySet currHitChildren;
        // m_hitChildren now stores last hit children;
        // compare them to broadcast mouse enter & leave events.
        for (auto& child : m_children)
        {
            if (child->IsHit(e.cursorPoint))
            {
                currHitChildren.insert(child);
            }
        }
        // OnMouseEnter
        MouseEnterEvent mee = {};
        mee.cursorPoint = e.cursorPoint;

        for (auto& child : currHitChildren)
        {
            // Moved in just now, trigger OnMouseEnter event.
            if (m_hitChildren.find(child) == m_hitChildren.end())
            {
                if (child->IsMouseEnterSensitive())
                {
                    if (!child->OnMouseEnter(mee)) break;
                }
            }
        }
        // OnMouseLeave
        MouseLeaveEvent mle = {};
        mle.cursorPoint = e.lastCursorPoint;

        for (auto& child : m_hitChildren)
        {
            // Moved out just now, trigger OnMouseLeave event.
            if (currHitChildren.find(child) == currHitChildren.end())
            {
                if (child->IsMouseLeaveSensitive())
                {
                    if (!child->OnMouseLeave(mle)) break;
                }
            }
        }
        // Update panel's hit children set.
        m_hitChildren = std::move(currHitChildren);

        // Only broadcast the event to those hit widgets.
        for (auto& hit : m_hitChildren)
        {
            if (hit->IsMouseMoveSensitive())
            {
                // Note the return boolean means whether to continue delivering the event.
                if (!hit->OnMouseMove(e)) break;
            }
        }
        for (auto& pinned : m_pinnedChildren)
        {
            if (pinned->IsMouseMoveSensitive())
            {
                if (!pinned->OnMouseMove(e)) break;
            }
        }
        return false;
    }

    bool Panel::OnMouseLeave(MouseLeaveEvent& e)
    {
        if (f_onMouseLeaveOverride)
        {
            return f_onMouseLeaveOverride(this, e);
        }
        else
        {
            if (f_onMouseLeaveBefore) f_onMouseLeaveBefore(this, e);

            bool value = OnMouseLeaveHelper(e);

            return f_onMouseLeaveAfter ? f_onMouseLeaveAfter(this, e) : value;
        }
    }

    bool Panel::OnMouseLeaveHelper(MouseLeaveEvent& e)
    {
        return false;
    }

    bool Panel::OnMouseWheel(MouseWheelEvent& e)
    {
        if (f_onMouseWheelOverride)
        {
            return f_onMouseWheelOverride(this, e);
        }
        else
        {
            if (f_onMouseWheelBefore) f_onMouseWheelBefore(this, e);

            bool value = OnMouseWheelHelper(e);

            return f_onMouseWheelAfter ? f_onMouseWheelAfter(this, e) : value;
        }
    }

    bool Panel::OnMouseWheelHelper(MouseWheelEvent& e)
    {
        // Only broadcast the event to those hit widgets.
        for (auto& child : m_hitChildren)
        {
            if (child->IsMouseWheelSensitive())
            {
                // Note the return boolean means whether to continue delivering the event.
                if (!child->OnMouseWheel(e)) break;
            }
        }
        for (auto& child : m_pinnedChildren)
        {
            if (child->IsMouseWheelSensitive())
            {
                if (!child->OnMouseWheel(e)) break;
            }
        }
        return false;
    }

    bool Panel::OnKeyboard(KeyboardEvent& e)
    {
        if (f_onKeyboardOverride)
        {
            return f_onKeyboardOverride(this, e);
        }
        else
        {
            if (f_onKeyboardBefore) f_onKeyboardBefore(this, e);

            bool value = OnKeyboardHelper(e);

            return f_onKeyboardAfter ? f_onKeyboardAfter(this, e) : value;
        }
    }

    bool Panel::OnKeyboardHelper(KeyboardEvent& e)
    {
        // Only broadcast the event to those hit widgets.
        for (auto& child : m_hitChildren)
        {
            if (child->IsKeyboardSensitive())
            {
                // Note the return boolean means whether to continue delivering the event.
                if (!child->OnKeyboard(e)) break;
            }
        }
        for (auto& child : m_pinnedChildren)
        {
            if (child->IsKeyboardSensitive())
            {
                if (!child->OnKeyboard(e)) break;
            }
        }
        return false;
    }

    bool Panel::IsMouseButtonSensitive()
    {
        return m_isMouseButtonSensitive;
    }

    void Panel::SetMouseButtonSensitive(bool value)
    {
        m_isMouseButtonSensitive = value;
    }

    bool Panel::IsMouseEnterSensitive()
    {
        return m_isMouseEnterSensitive;
    }

    void Panel::SetMouseEnterSensitive(bool value)
    {
        m_isMouseEnterSensitive = value;
    }

    bool Panel::IsMouseMoveSensitive()
    {
        return m_isMouseMoveSensitive;
    }

    void Panel::SetMouseMoveSensitive(bool value)
    {
        m_isMouseMoveSensitive = value;
    }

    bool Panel::IsMouseLeaveSensitive()
    {
        return m_isMouseLeaveSensitive;
    }

    void Panel::SetMouseLeaveSensitive(bool value)
    {
        m_isMouseLeaveSensitive = value;
    }

    bool Panel::IsMouseWheelSensitive()
    {
        return m_isMouseWheelSensitive;
    }

    void Panel::SetMouseWheelSensitive(bool value)
    {
        m_isMouseWheelSensitive = value;
    }

    bool Panel::IsKeyboardSensitive()
    {
        return m_isKeyboardSensitive;
    }

    void Panel::SetKeyboardSensitive(bool value)
    {
        m_isKeyboardSensitive = value;
    }

    bool Panel::IsVisible()
    {
        return IsD2D1ObjectVisible();
    }

    void Panel::SetVisible(bool value)
    {
        SetD2D1ObjectVisible(value);
    }

    bool Panel::IsEnabled()
    {
        return
            IsMouseButtonSensitive() ||
            IsMouseEnterSensitive() ||
            IsMouseMoveSensitive() ||
            IsMouseLeaveSensitive() ||
            IsMouseWheelSensitive() ||
            IsKeyboardSensitive();
    }

    void Panel::SetEnabled(bool value)
    {
        SetMouseButtonSensitive(value);
        SetMouseEnterSensitive(value);
        SetMouseMoveSensitive(value);
        SetMouseLeaveSensitive(value);
        SetMouseWheelSensitive(value);
        SetKeyboardSensitive(value);
    }

    D2D1_SIZE_F Panel::Size()
    {
        return { m_rect.right - m_rect.left, m_rect.bottom - m_rect.top };
    }

    float Panel::Width()
    {
        return m_rect.right - m_rect.left;
    }

    float Panel::Height()
    {
        return m_rect.bottom - m_rect.top;
    }

    D2D1_POINT_2F Panel::Position()
    {
        return { m_rect.left, m_rect.top };
    }

    D2D_POINT_2F Panel::AbsolutePosition()
    {
        return { m_absoluteRect.left, m_absoluteRect.top };
    }

    SharedPtr<Panel> Panel::Parent()
    {
        return m_parent.lock();
    }

    void Panel::SetParent(ShrdPtrParam<Panel> uiobj)
    {
        if (!m_parent.expired())
        {
            auto tmpParent = m_parent.lock();
            // We must check this to avoid infinite iteration!
            if (tmpParent == uiobj) return;
            tmpParent->RemoveUIObject(shared_from_this());
        }
        if (uiobj != nullptr)
        {
            // An iterative invocation, also see the comment above.
            (m_parent = uiobj).lock()->AddUIObject(shared_from_this());
        }
        else m_parent.reset();

        UpdateAbsoluteRect();
    }

    void Panel::AddUIObject(ShrdPtrParam<Panel> uiobj)
    {
        if (uiobj == nullptr) return;
        // We must check this to avoid infinite iteration!
        if (m_children.find(uiobj) == m_children.end())
        {
            m_children.insert(uiobj);
            m_drawObjects2D.insert(uiobj);
            // An iterative invocation, also see the comment above.
            uiobj->SetParent(shared_from_this());
        }
    }

    void Panel::RemoveUIObject(ShrdPtrParam<Panel> uiobj)
    {
        m_children.erase(uiobj);
        m_drawObjects2D.insert(uiobj);
    }

    void Panel::PinUIObject(ShrdPtrParam<Panel> uiobj)
    {
        if (uiobj == nullptr) return;
        m_pinnedChildren.insert(uiobj);
    }

    void Panel::UnpinUIObject(ShrdPtrParam<Panel> uiobj)
    {
        m_pinnedChildren.erase(uiobj);
    }

    void Panel::ForeachChild(const Function<void(ShrdPtrParam<Panel>)>& func)
    {
        for (auto& uiobj : m_children) func(uiobj);
    }

    D2D1_POINT_2F Panel::AbsoluteToRelative(const D2D1_POINT_2F& p)
    {
        return { p.x + m_rect.left - m_absoluteRect.left, p.y + m_rect.top - m_absoluteRect.top };
    }

    D2D1_RECT_F Panel::AbsoluteToRelative(const D2D1_RECT_F& rect)
    {
        auto leftTop = AbsoluteToRelative(D2D1_POINT_2F{ rect.left, rect.top });
        auto rightBottom = AbsoluteToRelative(D2D1_POINT_2F{ rect.right, rect.bottom });
        
        return { leftTop.x, leftTop.y, rightBottom.x, rightBottom.y };
    }

    D2D1_POINT_2F Panel::AbsoluteToSelfCoord(const D2D1_POINT_2F& p)
    {
        return { p.x - m_absoluteRect.left, p.y - m_absoluteRect.top };
    }

    D2D1_RECT_F Panel::AbsoluteToSelfCoord(const D2D1_RECT_F& rect)
    {
        auto leftTop = AbsoluteToSelfCoord(D2D1_POINT_2F{ rect.left, rect.top });
        auto rightBottom = AbsoluteToSelfCoord(D2D1_POINT_2F{ rect.right, rect.bottom });

        return { leftTop.x, leftTop.y, rightBottom.x, rightBottom.y };
    }

    D2D1_POINT_2F Panel::RelativeToAbsolute(const D2D1_POINT_2F& p)
    {
        return { p.x - m_rect.left + m_absoluteRect.left, p.y - m_rect.top + m_absoluteRect.top };
    }

    D2D1_RECT_F Panel::RelativeToAbsolute(const D2D1_RECT_F& rect)
    {
        auto leftTop = RelativeToAbsolute(D2D1_POINT_2F{ rect.left, rect.top });
        auto rightBottom = RelativeToAbsolute(D2D1_POINT_2F{ rect.right, rect.bottom });

        return { leftTop.x, leftTop.y, rightBottom.x, rightBottom.y };
    }

    D2D1_POINT_2F Panel::RelativeToSelfCoord(const D2D1_POINT_2F& p)
    {
        return { p.x - m_rect.left, p.y - m_rect.top };
    }

    D2D1_RECT_F Panel::RelativeToSelfCoord(const D2D1_RECT_F& rect)
    {
        auto leftTop = RelativeToSelfCoord(D2D1_POINT_2F{ rect.left, rect.top });
        auto rightBottom = RelativeToSelfCoord(D2D1_POINT_2F{ rect.right, rect.bottom });

        return { leftTop.x, leftTop.y, rightBottom.x, rightBottom.y };
    }

    D2D1_POINT_2F Panel::SelfCoordToAbsolute(const D2D1_POINT_2F& p)
    {
        return { p.x + m_absoluteRect.left, p.y + m_absoluteRect.top };
    }

    D2D1_RECT_F Panel::SelfCoordToAbsolute(const D2D1_RECT_F& rect)
    {
        auto leftTop = SelfCoordToAbsolute(D2D1_POINT_2F{ rect.left, rect.top });
        auto rightBottom = SelfCoordToAbsolute(D2D1_POINT_2F{ rect.right, rect.bottom });

        return { leftTop.x, leftTop.y, rightBottom.x, rightBottom.y };
    }

    D2D1_POINT_2F Panel::SelfCoordToRelative(const D2D1_POINT_2F& p)
    {
        return { p.x + m_rect.left, p.y + m_rect.top };
    }

    D2D1_RECT_F Panel::SelfCoordToRelative(const D2D1_RECT_F& rect)
    {
        auto leftTop = SelfCoordToRelative(D2D1_POINT_2F{ rect.left, rect.top });
        auto rightBottom = SelfCoordToRelative(D2D1_POINT_2F{ rect.right, rect.bottom });

        return { leftTop.x, leftTop.y, rightBottom.x, rightBottom.y };
    }

    void Panel::Transform(float left, float top, float width, float height)
    {
        float minWidth = MinimalWidth();
        width = max(width, minWidth);

        float minHeight = MinimalHeight();
        height = max(height, minHeight);

        m_rect = { left, top, left + width, top + height };

        UpdateAbsoluteRect();
    }

    void Panel::Move(float left, float top)
    {
        m_rect = { left, top, left + Width(), top + Height() };

        UpdateAbsoluteRect();
    }

    void Panel::Resize(float width, float height)
    {
        float minWidth = MinimalWidth();
        width = max(width, minWidth);

        float minHeight = MinimalHeight();
        height = max(height, minHeight);

        m_rect.right = m_rect.left + width;
        m_rect.bottom = m_rect.top + height;

        UpdateAbsoluteRect();
    }

    void Panel::SetUIObjectPriority(int value)
    {
        ISortable<Panel>::m_priority = value;

        if (m_parent.expired())
        {
            UnregisterDrawObjects();
            RegisterDrawObjects();
        }
        else // Managed by parent object.
        {
            auto tmpParent = m_parent.lock();
            tmpParent->RemoveUIObject(shared_from_this());
            tmpParent->AddUIObject(shared_from_this());
        }
    }

    void Panel::SetD2D1ObjectPriority(int value)
    {
        ISortable<IDrawObject2D>::m_priority = value;

        if (m_parent.expired())
        {
            UnregisterApplicationEvents();
            RegisterApplicationEvents();
        }
        else // Managed by parent object.
        {
            auto tmpParent = m_parent.lock();
            tmpParent->RemoveUIObject(shared_from_this());
            tmpParent->AddUIObject(shared_from_this());
        }
    }

    void Panel::UpdateAbsoluteRect()
    {
        // Don't use Size() since it depends on the relative rect,
        // which might has been updated before the absolute rect.
        D2D1_SIZE_F originSize =
        {
            m_absoluteRect.right - m_absoluteRect.left,
            m_absoluteRect.bottom - m_absoluteRect.top
        };
        D2D1_POINT_2F originPosition = AbsolutePosition();

        if (!m_parent.expired())
        {
            m_absoluteRect = Mathu::Offset(m_rect, m_parent.lock()->AbsolutePosition());
        }
        else m_absoluteRect = m_rect;

        float width = Width();
        float height = Height();
        // Check whether to trigger sizing event.
        if (originSize.width != width || originSize.height != height)
        {
            SizeEvent e = {};
            e.size = { width, height };

            OnSize(e);
        }
        float x = m_absoluteRect.left;
        float y = m_absoluteRect.top;
        // Check whether to trigger movement event.
        if (originPosition.x != x || originPosition.y != y)
        {
            MoveEvent me = {};
            me.position = { m_rect.left, m_rect.top };

            OnMove(me);
        }
    }

    D2D1_RECT_F Panel::SelfCoordRect()
    {
        return { 0.0f, 0.0f, Width(), Height() };
    }

    void Panel::UpdateChildrenObjects(Renderer* rndr)
    {
        for (auto& child : m_children)
        {
            if (child->IsD2D1ObjectVisible())
            {
                child->OnRendererUpdateObject2D(rndr);
            }
        }
    }

    void Panel::DrawChildrenLayers(Renderer* rndr)
    {
        for (auto& child : m_children)
        {
            if (child->IsD2D1ObjectVisible())
            {
                child->OnRendererDrawD2D1Layer(rndr);
            }
        }
    }

    void Panel::DrawBackground(Renderer* rndr)
    {
        if (brush != nullptr)
        {
            D2D1_ROUNDED_RECT roundedRect = { m_absoluteRect, m_radiusX, m_radiusY };
            rndr->d2d1DeviceContext->FillRoundedRectangle(roundedRect, brush.Get());
        }
        if (bitmap != nullptr)
        {
            rndr->d2d1DeviceContext->DrawBitmap(bitmap.Get(), m_absoluteRect);
        }
    }
    void Panel::DrawChildrenObjects(Renderer* rndr)
    {
        for (auto& child : m_children)
        {
            if (child->IsD2D1ObjectVisible())
            {
                child->OnRendererDrawD2D1Object(rndr);
            }
        }
    }
}