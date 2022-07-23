#pragma once

#include "Common/Precompile.h"

#include "UIKit/Label.h"
#include "UIKit/ResizablePanel.h"
#include "UIKit/ShadowStyle.h"
#include "UIKit/SolidStyle.h"
#include "UIKit/StrokeStyle.h"
#include "UIKit/Window.h"

namespace d14engine::uikit
{
    struct TabGroup : ResizablePanel
    {
        explicit TabGroup(const D2D1_RECT_F& rect);

        void OnInitializeFinish() override;

        SolidStyle background = {};
        StrokeStyle stroke = {};

    public:
        // This field decides top-extended height of Panel's IsHit area.
        float tabBarExtendedHeight = 30.0f;

        struct TabSeparatorStyle
        {
            D2D1_SIZE_F size = {};

            SolidStyle background = {};
        }
        tabSeparatorStyle = {};

        enum class TabState { Active, Hover, Dormant, Count };

        // State candidates of tab card's close-x button.
        enum class TabCloseXState { Idle, Hover, Down, Count };

        struct TabAppearance
        {
            struct Card
            {
                float height = {};
                float roundRadius = {};

                SolidStyle background = {};
            }
            card = {};

            struct Icon
            {
                D2D1_SIZE_F size = {};
                float leftOffset = {};

                ComPtr<ID2D1Bitmap1> bitmap = {};
                float bitmapOpacity = {};
            }
            icon = {};

            struct Title
            {
                D2D1_SIZE_F size = {};
                float leftOffset = {};

                ComPtr<IDWriteTextFormat> format = {};

                SolidStyle foreground = {};
                SolidStyle background = {};
            }
            title = {};

            struct CloseX
            {
                D2D1_SIZE_F size = {};
                float rightOffset = {};

                // Left/top-most distance between background panel and "X" icon.
                float bkgnPanelOffset = {};
                float bkgnPanelRoundRadius = {};

                struct ColorScheme
                {
                    SolidStyle foreground = {};
                    SolidStyle background = {};
                }
                colorSchemes[(size_t)TabCloseXState::Count] = {};

                float strokeWidth = {};
            }
            closeX = {};
        }
        tabAppearances[(size_t)TabState::Count] = {};

    public:
        struct Page { SharedPtr<Label> title; SharedPtr<Panel> content; };

        Page* FindPage(size_t index);
        Page* FindPage(WstrParam title);

        void SelectPage(size_t index);
        void SelectPage(WstrParam title);

        void InsertPage(const Page& page, size_t index = 0);
        void AppendPage(const Page& page);

        void RemovePage(size_t index);
        void RemovePage(WstrParam title);

        size_t AvailablePageCount();

    protected:
        float m_cardWidth = 120.0f;

        ShadowStyle m_activeCardShadowStyle = { 0, 0 };

        using PageArray = std::vector<Page>;

        PageArray m_pages = {};

        size_t m_currActivePageIndex = SIZE_T_MAX;
        size_t m_currHoverPageIndex = SIZE_T_MAX;

        using TabCloseXStateArray = std::vector<bool>;

        // Maintain state of each card's close-x button respectively.
        // Their element sizes should always equal to page-array's.
        TabCloseXStateArray m_tabCloseXHoverStates = {}, m_tabCloseXDownStates = {};

        size_t m_dragCardIndex = SIZE_T_MAX;

        // The position of dragged point on card in self-coordinate.
        D2D1_POINT_2F m_dragPoint = {};

    public:
        float CardWidth();
        void SetCardWidth(float value);

    protected:
        void LoadActiveCardShadowBitmap();

        TabState GetTabState(size_t index);

        D2D1_RECT_F TabBarPanelRect();

        D2D1_RECT_F CardRect(size_t index);
        D2D1_POINT_2F CardPosition(size_t index);

        D2D1_RECT_F IconRect(size_t index);
        D2D1_RECT_F TitleRect(size_t index);
        D2D1_RECT_F CloseXRect(size_t index);
        D2D1_RECT_F CloseXBkgnPanelRect(size_t index);

        void DrawCardIcon(Renderer* rndr, size_t index);
        void DrawTitleLabel(Renderer* rndr, size_t index);
        void DrawCloseXButton(Renderer* rndr, size_t index);

        void TriggerPagePromoteEvent(MouseMoveEvent& e);

    public:
        SharedPtr<Window> PromotePageToWindow(size_t index);

        SolidStyle maskStyleWhenBelowDragWindow = {};

        // When a window is being dragged, all registered tab-groups will be
        // associated with the window. After the window is released above
        // any tab-group, its center UI object (if has) will be demoted as
        // a new page and then its content will be added into the tab-group.
        WeakPtr<Window> temporaryAssociatedWindow = {};

    protected:
        bool IsTemporaryWindowDragAbove();

    protected:
        // Override interface methods.

        // IDrawObject2D
        void OnRendererDrawD2D1LayerHelper(Renderer* rndr) override;

        void OnRendererDrawD2D1ObjectHelper(Renderer* rndr) override;

    public:
        // Panel
        bool IsHitHelper(Event::Point& p) override;

        float MinimalWidth() override;

    protected:
        void OnSizeHelper(SizeEvent& e) override;

        void OnChangeThemeHelper(WstrViewParam themeName) override;

        bool OnMouseButtonHelper(MouseButtonEvent& e) override;

        bool OnMouseMoveHelper(MouseMoveEvent& e) override;

        bool OnMouseLeaveHelper(MouseMoveEvent& e) override;
    };
}