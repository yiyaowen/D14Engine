#pragma once

#include "Precompile.h"

#include "ResizablePanel.h"

#include "Label.h"
#include "MaskStyle.h"
#include "ShadowStyle.h"
#include "SolidStyle.h"

namespace d14engine::ui
{
    struct Window : ResizablePanel, protected MaskStyle, protected ShadowStyle, SolidStyle
    {
        static ComPtr<ID2D1LinearGradientBrush>
            g_titleBarPanelBrush,
            g_decorativeBarBrush;

        static void LoadCommonResources();

        Window(WstrParam title, const D2D1_RECT_F& rect);

        void OnInitializeFinish() override;

    public:
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

    public:
        // The states of the close brother is listed separately since it uses different settings.
        enum class ThreeBrosState { Idle, Hover, Down, CloseIdle, CloseHover, CloseDown, Count };

        struct ThreeBrosAppearance
        {
            D2D1_COLOR_F buttonColor = (D2D1::ColorF)D2D1::ColorF::White;
            float buttonOpacity = 0.0f;

            D2D1_COLOR_F iconColor = (D2D1::ColorF)D2D1::ColorF::Black;
            float iconOpacity = 1.0f;
        }
        threeBrosAppearances[(size_t)(ThreeBrosState::Count)];

        const Wstring& Title();
        void SetTitle(WstrViewParam title);

        float ClientAreaHeight();

        constexpr float NonClientAreaHeight() { return 40.0f; }

        D2D1_RECT_F ClientAreaRect();

    protected:
        SharedPtr<Label> m_title, m_closeX;

        // Forbid the 3 brothers to respond events when move or resize the window.
        bool m_is3BrothersEnabled = true;

        D2D1_RECT_F TitlePanelRect();
        D2D1_RECT_F CenterTitleRect();
        D2D1_RECT_F DecorativeBarRect();

        bool m_isDragTitleBar = false;

        // The relative position of dragged point on title bar.
        D2D1_POINT_2F m_dragPoint = {};

        bool m_isCloseHover = false, m_isCloseDown = false;
        bool m_isMaximizeHover = false, m_isMaximizeDown = false;
        bool m_isMinimizeHover = false, m_isMinimizeDown = false;

        D2D1_RECT_F CloseButtonRect();
        D2D1_RECT_F RelativeCloseButtonRect();
        D2D1_RECT_F MaximizeButtonRect();
        D2D1_RECT_F MaximizeIconRect();
        D2D1_RECT_F MinimizeButtonRect();
        D2D1_RECT_F MinimizeIconRect();

        void Set3BrothersButtonBrushState(bool isHover, bool isDown);
        void Set3BrothersIconBrushState(bool isHover, bool isDown);

    public:
        // Override interface methods.

        // IDrawObject2D
        void OnRendererDrawD2D1LayerHelper(Renderer* rndr) override;

        void OnRendererDrawD2D1ObjectHelper(Renderer* rndr) override;

        // Panel
        float MinimalWidth() override;

        float MinimalHeight() override;

        // See Panel.h for the reason of rewriting OnxxxHelper instead of Onxxx.

        void OnSizeHelper(SizeEvent& e) override;

        void OnChangeThemeHelper(WstrViewParam themeName) override;

        bool OnMouseButtonHelper(MouseButtonEvent& e) override;

        bool OnMouseMoveHelper(MouseMoveEvent& e) override;
    };
}