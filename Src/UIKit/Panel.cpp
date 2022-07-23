#include "Common/Precompile.h"

#include "UIKit/Panel.h"

#include "Renderer/MathUtils.h"
using namespace d14engine::renderer;

#include "UIKit/Application.h"

namespace d14engine::uikit
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
        if (f_onRendererUpdateObject2DOverride)
        {
            f_onRendererUpdateObject2DOverride(rndr);
        }
        else
        {
            if (f_onRendererUpdateObject2DBefore) f_onRendererUpdateObject2DBefore(rndr);

            // Make sure all children complete updating before parent.
            UpdateChildrenObjects(rndr);

            OnRendererUpdateObject2DHelper(rndr);

            if (f_onRendererUpdateObject2DAfter) f_onRendererUpdateObject2DAfter(rndr);
        }
    }

    void Panel::OnRendererDrawD2D1Layer(Renderer* rndr)
    {
        if (f_onRendererDrawD2D1LayerOverride)
        {
            f_onRendererDrawD2D1LayerOverride(rndr);
        }
        else
        {
            if (f_onRendererDrawD2D1LayerBefore) f_onRendererDrawD2D1LayerBefore(rndr);

            // Make sure all child-layers are prepared in advance.
            if (!m_takeOverChildrenDrawing) DrawChildrenLayers(rndr);

            OnRendererDrawD2D1LayerHelper(rndr);

            if (f_onRendererDrawD2D1LayerAfter) f_onRendererDrawD2D1LayerAfter(rndr);
        }
    }

    void Panel::OnRendererDrawD2D1Object(Renderer* rndr)
    {
        if (f_onRendererDrawD2D1ObjectOverride)
        {
            f_onRendererDrawD2D1ObjectOverride(rndr);
        }
        else
        {
            if (f_onRendererDrawD2D1ObjectBefore) f_onRendererDrawD2D1ObjectBefore(rndr);

            OnRendererDrawD2D1ObjectHelper(rndr);

            // Make sure all children are drawn above parent.
            if (!m_takeOverChildrenDrawing) DrawChildrenObjects(rndr);

            if (f_onRendererDrawD2D1ObjectAfter) f_onRendererDrawD2D1ObjectAfter(rndr);
        }
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

    void Panel::OnInitializeFinish()
    {
        OnChangeThemeHelper(Application::APP->CurrentThemeName());
    }

    bool Panel::IsHit(Event::Point& p)
    {
        if (f_isHit) return f_isHit(this, p);
        else return IsHitHelper(p);
    }

    D2D1_SIZE_F Panel::MinimalSize()
    {
        return { MinimalWidth(), MinimalHeight() };
    }

    D2D1_SIZE_F Panel::MaximalSize()
    {
        return { MaximalWidth(), MaximalHeight() };
    }

    float Panel::MinimalWidth()
    {
        return minimalWidth.has_value() ? minimalWidth.value() : 0.0f;
    }

    float Panel::MinimalHeight()
    {
        return minimalHeight.has_value() ? minimalHeight.value() : 0.0f;
    }

    float Panel::MaximalWidth()
    {
        return maximalWidth.has_value() ? maximalWidth.value() : FLT_MAX;
    }

    float Panel::MaximalHeight()
    {
        return maximalHeight.has_value() ? maximalHeight.value() : FLT_MAX;
    }

    void Panel::RegisterDrawObjects()
    {
        Application::APP->MainRenderer()->AddDrawObject2D(shared_from_this());
    }

    void Panel::UnregisterDrawObjects()
    {
        Application::APP->MainRenderer()->RemoveDrawObject2D(shared_from_this());
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

    void Panel::OnChangeTheme(WstrViewParam themeName)
    {
        if (f_onChangeThemeBefore)
        {
            f_onChangeThemeOverride(this, themeName);
        }
        else
        {
            if (f_onChangeThemeBefore) f_onChangeThemeBefore(this, themeName);

            OnChangeThemeHelper(themeName);

            if (f_onChangeThemeAfter) f_onChangeThemeAfter(this, themeName);
        }
    }

    bool Panel::OnGetFocus()
    {
        if (f_onGetFocusOverride)
        {
            return f_onGetFocusOverride(this);
        }
        else
        {
            if (f_onGetFocusBefore) f_onGetFocusBefore(this);

            bool value = OnGetFocusHelper();

            return f_onGetFocusAfter ? f_onGetFocusAfter(this) : value;
        }
    }

    bool Panel::OnLoseFocus()
    {
        if (f_onLoseFocusOverride)
        {
            return f_onLoseFocusOverride();
        }
        else
        {
            if (f_onLoseFocusBefore) f_onLoseFocusBefore();

            bool value = OnLoseFocusHelper();

            return f_onLoseFocusAfter ? f_onLoseFocusAfter() : value;
        }
    }

    bool Panel::OnMouseButton(MouseButtonEvent& e)
    {
        if (f_onMouseButtonOverride)
        {
            return f_onMouseButtonOverride(this, e);
        }
        else
        {
            if (f_onMouseButtonBefore) f_onMouseButtonBefore(this, e);

            bool value = OnMouseButtonHelper(e);

            return f_onMouseButtonAfter ? f_onMouseButtonAfter(this, e) : value;
        }
    }

    bool Panel::OnMouseEnter(MouseMoveEvent& e)
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

    bool Panel::OnMouseLeave(MouseMoveEvent& e)
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

    bool Panel::IsHitHelper(Event::Point& p)
    {
        return Mathu::IsOverlapped(p, m_absoluteRect);
    }

    void Panel::OnSizeHelper(SizeEvent& e)
    {
        for (auto& child : m_children)
        {
            child->OnParentSize(e);
        }
    }

    void Panel::OnParentSizeHelper(SizeEvent& e)
    {
        // TODO: add panel parent sizing logic.
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

    void Panel::OnParentMoveHelper(MoveEvent& e)
    {
        UpdateAbsoluteRect();

        MoveEvent me = {};
        me.position = { m_rect.left, m_rect.top };

        OnMove(me);
    }

    void Panel::OnChangeThemeHelper(WstrViewParam themeName)
    {
        if (m_skipChangeChildrenTheme) return;

        for (auto& child : m_children)
        {
            child->OnChangeTheme(themeName);
        }
    }

    bool Panel::OnGetFocusHelper()
    {
        PinApplicationEvents();
        return true;
    }

    bool Panel::OnLoseFocusHelper()
    {
        UnpinApplicationEvents();
        return true;
    }

    bool Panel::OnMouseButtonHelper(MouseButtonEvent& e)
    {
        // Only broadcast the event to those hit children.
        ISortable<Panel>::Foreach(m_hitChildren, [&](ShrdPtrParam<Panel> child)
        {
            if (child->appEventFlags.mouse.button)
            {
                if (e.status.LeftDown() && child->isFocusable) e.focused = child;
                // Note the return boolean means whether to continue delivering the event.
                return child->OnMouseButton(e);
            }
            return false;
        });
        ISortable<Panel>::Foreach(m_diffPinnedChildren, [&](ShrdPtrParam<Panel> child)
        {
            if (child->appEventFlags.mouse.button)
            {
                // The focused should be selected from those actual hit children,
                // so there's no need to update mouse-button event's focused field.
                return child->OnMouseButton(e);
            }
            return false;
        });
        return false;
    }

    bool Panel::OnMouseEnterHelper(MouseMoveEvent& e)
    {
        return false;
    }

    bool Panel::OnMouseMoveHelper(MouseMoveEvent& e)
    {
        ChildObjectPrioritySet currHitChildren;
        // m_hitChildren now stores last hit children;
        // compare them to broadcast mouse enter & leave events.
        for (auto& child : m_children)
        {
            if (child->appEventFlags.hitTest && child->IsHit(e.cursorPoint))
            {
                currHitChildren.insert(child);
            }
        }
        ISortable<Panel>::Foreach(currHitChildren, [&](ShrdPtrParam<Panel> child)
        {
            // Moved in just now, trigger OnMouseEnter event.
            if (m_hitChildren.find(child) == m_hitChildren.end())
            {
                if (child->appEventFlags.mouse.enter)
                {
                    return child->OnMouseEnter(e);
                }
            }
            return false;
        });
        ISortable<Panel>::Foreach(m_hitChildren, [&](ShrdPtrParam<Panel> child)
        {
            // Moved out just now, trigger OnMouseLeave event.
            if (currHitChildren.find(child) == currHitChildren.end())
            {
                if (child->appEventFlags.mouse.leave)
                {
                    return child->OnMouseLeave(e);
                }
            }
            return false;
        });
        m_hitChildren = std::move(currHitChildren);

        // Only broadcast the event to those hit children.
        ISortable<Panel>::Foreach(m_hitChildren, [&](ShrdPtrParam<Panel> child)
        {
            if (child->appEventFlags.mouse.move)
            {
                // Note the return boolean means whether to continue delivering the event.
                return child->OnMouseMove(e);
            }
            return false;
        });
        UpdateDiffPinnedUIObjects();

        ISortable<Panel>::Foreach(m_diffPinnedChildren, [&](ShrdPtrParam<Panel> child)
        {
            if (child->appEventFlags.mouse.move)
            {
                return child->OnMouseMove(e);
            }
            return false;
        });
        return false;
    }

    bool Panel::OnMouseLeaveHelper(MouseMoveEvent& e)
    {
        // Also see OnMouseMoveHelper's implementation: when mouse entered panel,
        // mouse-enter-event was called followed by mouse-move-event; however,
        // when mouse left panel, only mouse-leave-event would be called since
        // panel's hit-test failed at this point. To fix this bug, we decide to
        // trigger mouse-move-event manually when mouse-leave-event was fired.
        return OnMouseMoveHelper(e);
    }

    bool Panel::OnMouseWheelHelper(MouseWheelEvent& e)
    {
        // Only broadcast the event to those hit children.
        ISortable<Panel>::Foreach(m_hitChildren, [&](ShrdPtrParam<Panel> child)
        {
            if (child->appEventFlags.mouse.wheel)
            {
                // Note the return boolean means whether to continue delivering the event.
                return child->OnMouseWheel(e);
            }
            return false;
        });
        ISortable<Panel>::Foreach(m_diffPinnedChildren, [&](ShrdPtrParam<Panel> child)
        {
            if (child->appEventFlags.mouse.wheel)
            {
                return child->OnMouseWheel(e);
            }
            return false;
        });
        return false;
    }

    bool Panel::OnKeyboardHelper(KeyboardEvent& e)
    {
        // Only broadcast the event to those hit children.
        ISortable<Panel>::Foreach(m_hitChildren, [&](ShrdPtrParam<Panel> child)
        {
            if (child->appEventFlags.keyboard)
            {
                // Note the return boolean means whether to continue delivering the event.
                return child->OnKeyboard(e);
            }
            return false;
        });
        ISortable<Panel>::Foreach(m_diffPinnedChildren, [&](ShrdPtrParam<Panel> child)
        {
            if (child->appEventFlags.keyboard)
            {
                return child->OnKeyboard(e);
            }
            return false;
        });
        return false;
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
        return m_isEnabled;
    }

    void Panel::SetEnabled(bool value)
    {
        m_isEnabled = value;

        appEventFlags.hitTest = value;
        appEventFlags.mouse.button = value;
        appEventFlags.mouse.enter = value;
        appEventFlags.mouse.move = value;
        appEventFlags.mouse.leave = value;
        appEventFlags.mouse.wheel = value;
        appEventFlags.keyboard = value;
    }

    D2D1_SIZE_F Panel::Size()
    {
        return { Width(), Height() };
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

    const D2D1_RECT_F& Panel::AbsoluteRect()
    {
        return m_absoluteRect;
    }

    const D2D1_RECT_F& Panel::RelativeRect()
    {
        return m_rect;
    }

    D2D1_RECT_F Panel::SelfCoordRect()
    {
        return { 0.0f, 0.0f, Width(), Height() };
    }

    WeakPtr<Panel> Panel::Parent()
    {
        return m_parent;
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
        m_drawObjects2D.erase(uiobj);
    }

    void Panel::PinUIObject(ShrdPtrParam<Panel> uiobj)
    {
        if (uiobj == nullptr) return;
        m_pinnedChildren.insert(uiobj);

        UpdateDiffPinnedUIObjectsLater();
    }

    void Panel::UnpinUIObject(ShrdPtrParam<Panel> uiobj)
    {
        m_pinnedChildren.erase(uiobj);

        UpdateDiffPinnedUIObjectsLater();
    }

    void Panel::UpdateDiffPinnedUIObjects()
    {
        m_diffPinnedChildren.clear();
        // The pinned children are not included in hit children to avoid double-callback.
        // For example, a button will respond twice if it is pinned and hit at the same time.
        std::set_difference(
            m_pinnedChildren.begin(), m_pinnedChildren.end(),
            m_hitChildren.begin(), m_hitChildren.end(),
            std::inserter(m_diffPinnedChildren, m_diffPinnedChildren.begin()),
            ISortable<Panel>::WeakAscending()); // Can't deduce automatically.
    }

    void Panel::UpdateDiffPinnedUIObjectsLater()
    {
        Application::APP->MarkDiffPinnedUpdatingCandidate(weak_from_this());
        Application::APP->PostCustomWinMessage(Application::CustomWinMessage::UpdateMiscDiffPinnedUIObjects);
    }

    void Panel::PinApplicationEvents()
    {
        if (m_parent.expired())
        {
            Application::APP->PinUIObject(shared_from_this());
        }
        else m_parent.lock()->PinUIObject(shared_from_this());
    }

    void Panel::UnpinApplicationEvents()
    {
        if (m_parent.expired())
        {
            Application::APP->UnpinUIObject(shared_from_this());
        }
        else m_parent.lock()->UnpinUIObject(shared_from_this());
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

    void Panel::Resize(const D2D1_SIZE_F& size)
    {
        m_rect.right = m_rect.left + size.width;
        m_rect.bottom = m_rect.top + size.height;
        UpdateAbsoluteRect();
    }

    void Panel::Resize(float width, float height)
    {
        Resize({ width, height });
    }

    void Panel::Move(const D2D1_POINT_2F& point)
    {
        m_rect = { point.x, point.y, point.x + Width(), point.y + Height() };
        UpdateAbsoluteRect();
    }

    void Panel::Move(float left, float top)
    {
        Move({ left, top });
    }

    void Panel::Transform(const D2D1_RECT_F& rect)
    {
        m_rect = rect;
        UpdateAbsoluteRect();
    }

    void Panel::Transform(float left, float top, float width, float height)
    {
        Transform({ left, top, left + width, top + height });
    }

    void Panel::SetUIObjectPriority(int value)
    {
        // std::set is implemented with red-black tree and its equality check
        // relies on strong order relation, which means that to find whether
        // two elements are equal we need to compare them twice:
        // we say "a == b" if and only if "a > b" is false and "a < b" is false.
        // Since we introduce "priority" as one of the sorting criteria,
        // so we must not change it before equality-check-dependent operations.
        if (m_parent.expired())
        {
            if (Application::APP->FindUIObject(shared_from_this()))
            {
                UnregisterApplicationEvents();

                // Update priority after find and erase operations.
                ISortable<Panel>::m_priority = value;
                RegisterApplicationEvents();
            }
        }
        else // Managed by parent object.
        {
            auto parent = m_parent.lock();
            parent->RemoveUIObject(shared_from_this());

            // Update priority after erase operation.
            ISortable<Panel>::m_priority = value;
            parent->AddUIObject(shared_from_this());
        }
        // Always update priority whether it is registered or not.
        ISortable<Panel>::m_priority = value;
    }

    void Panel::SetD2D1ObjectPriority(int value)
    {
        // See SetUIObjectPriority above for explanation of updating priority 3 times.
        if (m_parent.expired())
        {
            if (Application::APP->MainRenderer()->FindDrawObject2D(shared_from_this()))
            {
                UnregisterDrawObjects();

                ISortable<IDrawObject2D>::m_priority = value;
                RegisterDrawObjects();
            }
        }
        else // Managed by parent object.
        {
            auto parent = m_parent.lock();
            parent->RemoveUIObject(shared_from_this());

            ISortable<IDrawObject2D>::m_priority = value;
            parent->AddUIObject(shared_from_this());
        }
        ISortable<IDrawObject2D>::m_priority = value;
    }

    void Panel::UpdateAbsoluteRect()
    {
        // Don't use Size() since it depends on the relative rect,
        // which might has been updated before the absolute rect.
        D2D1_SIZE_F originalSize =
        {
            m_absoluteRect.right - m_absoluteRect.left,
            m_absoluteRect.bottom - m_absoluteRect.top
        };
        D2D1_POINT_2F originalPosition = AbsolutePosition();

        if (!m_parent.expired())
        {
            m_absoluteRect = Mathu::Offset(m_rect, m_parent.lock()->AbsolutePosition());
        }
        else m_absoluteRect = m_rect;

        float width = Width();
        float height = Height();
        // Check whether to trigger sizing event.
        if (Mathu::Rounding(originalSize.width) != Mathu::Rounding(width) ||
            Mathu::Rounding(originalSize.height) != Mathu::Rounding(height))
        {
            SizeEvent e = {};
            e.size = { width, height };

            OnSize(e);
        }
        float x = m_absoluteRect.left;
        float y = m_absoluteRect.top;
        // Check whether to trigger movement event.
        if (Mathu::Rounding(originalPosition.x) != Mathu::Rounding(x) ||
            Mathu::Rounding(originalPosition.y) != Mathu::Rounding(y))
        {
            MoveEvent me = {};
            me.position = { m_rect.left, m_rect.top };

            OnMove(me);
        }
    }

    void Panel::MoveChildWindowTopmost(Panel* w)
    {
        w->SetD2D1ObjectPriority(++m_topmostWindowPriority.d2d1Object);
        w->SetUIObjectPriority(--m_topmostWindowPriority.uiObject);
    }

    void Panel::MoveTopmost()
    {
        if (m_parent.expired())
        {
            Application::APP->MoveRootWindowTopmost(this);
        }
        else // Managed by parent object.
        {
            m_parent.lock()->MoveChildWindowTopmost(this);
        }
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
            D2D1_ROUNDED_RECT roundedRect = { m_absoluteRect, roundRadiusX, roundRadiusY };
            rndr->d2d1DeviceContext->FillRoundedRectangle(roundedRect, brush.Get());
        }
        if (bitmap != nullptr)
        {
            rndr->d2d1DeviceContext->DrawBitmap(bitmap.Get(), m_absoluteRect, bitmapOpacity);
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

    void Panel::OnRendererUpdateObject2DHelper(Renderer* rndr)
    {
        // TODO: add panel obj2d updating logic.
    }

    void Panel::OnRendererDrawD2D1LayerHelper(Renderer* rndr)
    {
        // TODO: add panel d2d1 layer drawing logic.
    }

    void Panel::OnRendererDrawD2D1ObjectHelper(Renderer* rndr)
    {
        DrawBackground(rndr);
    }
}