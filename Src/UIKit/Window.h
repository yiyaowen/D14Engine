#pragma once

#include "Common/Precompile.h"

#include "UIKit/Label.h"
#include "UIKit/MaskStyle.h"
#include "UIKit/ResizablePanel.h"
#include "UIKit/ShadowStyle.h"
#include "UIKit/SolidStyle.h"

namespace d14engine::uikit
{
    struct TabGroup;

    struct Window : ResizablePanel
    {
        Window(WstrParam title, const D2D1_RECT_F& rect);

        void OnInitializeFinish() override;

        MaskStyle mask = { 0, 0 };
        ShadowStyle shadow = { 0, 0 };
        SolidStyle background = {};

    public:
        // See Panel.h for the reason of an extral OnXxxHelper.

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
            D2D1_COLOR_F buttonColor = {};
            float buttonOpacity = {};

            D2D1_COLOR_F iconColor = {};
            float iconOpacity = {};
        }
        threeBrosAppearances[(size_t)(ThreeBrosState::Count)] = {};

        const Wstring& Title();
        void SetTitle(WstrViewParam title);

        float ClientAreaHeight();

        constexpr static float NonClientAreaMinimalWidth() { return 144.0f; }
        constexpr static float NonClientAreaHeight() { return 40.0f; }

        D2D1_RECT_F ClientAreaRect();

        ComPtr<ID2D1LinearGradientBrush> titleBarPanelBrush = {}, decorativeBarBrush = {};

    protected:
        SharedPtr<Label> m_title = {};

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
        D2D1_RECT_F CloseIconRect();
        constexpr static float CloseXStrokeWidth() { return 2.0f; }

        D2D1_RECT_F MaximizeButtonRect();
        D2D1_RECT_F MaximizeIconRect();
        D2D1_RECT_F MinimizeButtonRect();
        D2D1_RECT_F MinimizeIconRect();

        ThreeBrosState GetMinMaxBroState(bool isHover, bool isDown);
        ThreeBrosState GetCloseXBroState(bool isHover, bool isDown);

        void Set3BrothersButtonBrushState(ThreeBrosState state);
        void Set3BrothersIconBrushState(ThreeBrosState state);

    protected:
        WeakPtr<Panel> m_centerUIObject = {};

        // Only registered tab-group can receive and demote this window.
        using RegisteredTabGroupSet = // See TabGroup.h for more details.
            std::set<WeakPtr<TabGroup>, std::owner_less<WeakPtr<TabGroup>>>;

        RegisteredTabGroupSet m_registeredTabGroups = {};

    public:
        SharedPtr<Panel> CenterUIObject();
        void SetCenterUIObject(ShrdPtrParam<Panel> uiobj);

        void RegisterTabGroup(WeakPtrParam<TabGroup> tg);
        void UnregisterTabGroup(WeakPtrParam<TabGroup> tg);

        float maskOpacityWhenDragAboveTabGroup = 0.5f;

        // This field will be set to current hovered tab-group when dragging.
        WeakPtr<TabGroup> temporaryAssociatedTabGroup = {};

    protected:
        void HandleMouseButtonForRegisteredTabGroups(MouseButtonEvent& e);
        void HandleMouseMoveForRegisteredTabGroups(MouseMoveEvent& e);

    protected:
        // Override interface methods.

        // IDrawObject2D
        void OnRendererDrawD2D1LayerHelper(Renderer* rndr) override;

        void OnRendererDrawD2D1ObjectHelper(Renderer* rndr) override;

    public:
        // Panel
        float MinimalWidth() override;

        float MinimalHeight() override;

    protected:
        // See Panel.h for the reason of rewriting OnXxxHelper instead of OnXxx.

        void OnSizeHelper(SizeEvent& e) override;

        void OnChangeThemeHelper(WstrViewParam themeName) override;

        bool OnMouseButtonHelper(MouseButtonEvent& e) override;

        bool OnMouseMoveHelper(MouseMoveEvent& e) override;
    };
}