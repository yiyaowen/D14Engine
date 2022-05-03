#pragma once

#include "Precompile.h"

#include "Label.h"
#include "MaskStyle.h"
#include "ShadowStyle.h"
#include "SolidColorStyle.h"

namespace d14engine::ui
{
    struct Window : Panel, MaskStyle, ShadowStyle, SolidColorStyle
    {
        static ComPtr<ID2D1LinearGradientBrush>
            g_titleBarPanelBrush,
            g_decorativeBarBrush;

        static void LoadCommonResources();

        Window(WstrParam text, const D2D1_RECT_F& rect);

        void OnInitializeFinish() override
        {
            m_title->SetParent(shared_from_this());
            m_closeX->SetParent(shared_from_this());
        }

        // See Panel.h for the reason of an extral OnxxxHelper.

        virtual void OnMinimize();
        virtual void OnMinimizeHelper();

        Function<void(Window*)>
            f_onMinimizeOverride = {},
            f_onMinimizeBefore = {},
            f_onMinimizeAfter = {};

        virtual void OnMaximize();
        virtual void OnMaximizeHelper();

        Function<void(Window*)>
            f_onMaximizeOverride = {},
            f_onMaximizeBefore = {},
            f_onMaximizeAfter = {};

        virtual void OnClose();
        virtual void OnCloseHelper();

        Function<void(Window*)>
            f_onCloseOverride = {},
            f_onCloseBefore = {},
            f_onCloseAfter = {};

    protected:
        SharedPtr<Label> m_title, m_closeX;

        D2D1_RECT_F TitlePanelRect();

        D2D1_RECT_F RelativeTitlePanelRect();

        D2D1_RECT_F DecorativeBarRect();

        bool m_isDragTitleBar = false;

        // The relative coordinate of dragged point on title bar.
        D2D1_POINT_2F m_dragPoint = {};

        bool m_isLeftHover = false, m_isRightHover = false;
        bool m_isTopHover = false, m_isBottomHover = false;

        bool m_isLeftSizing = false, m_isRightSizing = false;
        bool m_isTopSizing = false, m_isBottomSizing = false;

        // The sizing event will be triggered when mouse cursor moves on shadow frame;
        // this field stores the relative offset from the held point to window boundary.
        D2D1_POINT_2F m_sizingOffset = {};

        bool m_isCloseHover = false, m_isCloseDown = false;
        bool m_isMaximizeHover = false, m_isMaximizeDown = false;
        bool m_isMinimizeHover = false, m_isMinimizeDown = false;

        D2D1_RECT_F CloseButtonRect();

        D2D1_RECT_F RelativeCloseButtonRect();

        D2D1_RECT_F MaximizeButtonRect();

        D2D1_RECT_F MaximizeIconRect();

        D2D1_RECT_F MinimizeButtonRect();

        D2D1_RECT_F MinimizeIconRect();

        static void Set3BrothersButtonBrushState(bool isHover, bool isDown);

        static void Set3BrothersIconBrushState();

    public:
        // Override interface methods.

        // IDrawObject2D
        void OnRendererDrawD2D1Layer(Renderer* rndr) override;

        void OnRendererDrawD2D1Object(Renderer* rndr) override;

        // IUIObject
        bool IsHit(Event::Point& p) override;

        float MinimalWidth() override;

        float MinimalHeight() override;

        // See Panel.h for the reason of rewriting OnxxxHelper instead of Onxxx.

        void OnSizeHelper(SizeEvent& e) override;

        bool OnMouseButtonHelper(MouseButtonEvent& e) override;

        bool OnMouseMoveHelper(MouseMoveEvent& e) override;
    };
}