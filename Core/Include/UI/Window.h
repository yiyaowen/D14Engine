#pragma once

#include "Precompile.h"

#include "Label.h"
#include "MaskStyle.h"
#include "ShadowStyle.h"
#include "SolidStyle.h"

namespace d14engine::ui
{
    struct Window : Panel, MaskStyle, ShadowStyle, SolidStyle
    {
        static ComPtr<ID2D1LinearGradientBrush>
            g_titleBarPanelBrush,
            g_decorativeBarBrush;

        static void LoadCommonResources();

        Window(WstrParam text, const D2D1_RECT_F& rect);

        void OnInitializeFinish() override;

        // See Panel.h for the reason of an extral OnxxxHelper.

        void OnMinimize();
        virtual void OnMinimizeHelper();

        Function<void(Window*)>
            f_onMinimizeOverride = {},
            f_onMinimizeBefore = {},
            f_onMinimizeAfter = {};

        void OnMaximize();
        virtual void OnMaximizeHelper();

        Function<void(Window*)>
            f_onMaximizeOverride = {},
            f_onMaximizeBefore = {},
            f_onMaximizeAfter = {};

        void OnClose();
        virtual void OnCloseHelper();

        Function<void(Window*)>
            f_onCloseOverride = {},
            f_onCloseBefore = {},
            f_onCloseAfter = {};

    protected:
        SharedPtr<Label> m_title, m_closeX;

        // Define an extra visibility flag to control whether to show the title;
        // it will only be drawn when both extra and self visibility flags are true,
        // which is used to prevent the title displaying unexpectedly in such case:
        // the window is scaled too small to show the title text, and if we use the
        // self visibility flag to hide it then there's a chance to show it again
        // incorrectly by simply calling SetVisible(true), so we must introduce the
        // protected visibility flag to make sure the title won't show in such case.
        bool m_isTitleVisible = true;

        // Forbid the 3 brothers to respond events when move or resize the window.
        bool m_is3BrothersEnabled = true;

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

        static void Set3BrothersIconBrushState(bool isHover, bool isDown);

    public:
        // Override interface methods.

        // IDrawObject2D
        void OnRendererDrawD2D1Layer(Renderer* rndr) override;

        void OnRendererDrawD2D1Object(Renderer* rndr) override;

        // Panel
        bool IsHit(Event::Point& p) override;

        float MinimalWidth() override;

        float MinimalHeight() override;

        // See Panel.h for the reason of rewriting OnxxxHelper instead of Onxxx.

        void OnSizeHelper(SizeEvent& e) override;

        bool OnMouseButtonHelper(MouseButtonEvent& e) override;

        bool OnMouseMoveHelper(MouseMoveEvent& e) override;
    };
}