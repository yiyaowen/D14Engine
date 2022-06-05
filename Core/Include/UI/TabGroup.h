#pragma once

#include "Precompile.h"

#include "ResizablePanel.h"

#include "Label.h"
#include "MaskStyle.h"
#include "ShadowStyle.h"
#include "SolidStyle.h"
#include "StrokeStyle.h"

namespace d14engine::ui
{
    struct TabGroup : ResizablePanel, SolidStyle, StrokeStyle
    {
        explicit TabGroup(const D2D1_RECT_F& rect);

        void OnInitializeFinish() override;

    public:
        // This field decides top-extended height of Panel's IsHit area.
        float tabBarExtendedHeight = 30.0f;

        struct TabSeparatorStyle
        {
            float height = 0.0f;

            D2D1_COLOR_F color = (D2D1::ColorF)D2D1::ColorF::Black;
            float opacity = 1.0f;

            float strokeWidth = 0.0f;
        }
        tabSeparatorStyle = {};

        enum class TabState { Active, Hover, Dormant, Count };

        // State candidates of tab card's close-x button.
        enum class TabCloseXState { Idle, Hover, Down, Count };

        struct TabAppearance
        {
            struct Card
            {
                float height = 0.0f;

                float roundRadius = 0.0f;

                D2D1_COLOR_F color = (D2D1::ColorF)D2D1::ColorF::Black;
                float opacity = 1.0f;
            }
            card = {};

            struct Title
            {
                D2D1_SIZE_F size = { 0.0f, 0.0f };
                float leftOffset = 0.0f;

                ComPtr<IDWriteTextFormat> format;

                D2D1_COLOR_F foregroundColor = (D2D1::ColorF)D2D1::ColorF::Black;
                float foregroundOpacity = 1.0f;

                D2D1_COLOR_F backgroundColor = (D2D1::ColorF)D2D1::ColorF::Black;
                float backgroundOpacity = 1.0f;
            }
            title = {};

            struct CloseX
            {
                D2D1_SIZE_F size = { 0.0f, 0.0f };
                float rightOffset = 0.0f;

                // Left/top-most distance between background panel and "X" icon.
                float bkgnPanelOffset = 0.0f;
                float bkgnPanelRoundRadius = 0.0f;

                struct ColorScheme
                {
                    D2D1_COLOR_F foregroundColor = (D2D1::ColorF)D2D1::ColorF::Black;
                    float foregroundOpacity = 1.0f;

                    D2D1_COLOR_F backgroundColor = (D2D1::ColorF)D2D1::ColorF::Black;
                    float backgroundOpacity = 1.0f;
                }
                colorSchemes[(size_t)TabCloseXState::Count];

                float strokeWidth = 0.0f;
            }
            closeX = {};
        }
        tabAppearances[(size_t)TabState::Count];

    public:
        void SelectPage(size_t index);
        void SelectPage(WstrParam title);

        struct Page { SharedPtr<Label> title; SharedPtr<Panel> content; };

        void InsertPage(const Page& page, size_t index = 0);
        void AppendPage(const Page& page);

        void RemovePage(size_t index);
        void RemovePage(WstrParam title);

    protected:
        float m_cardWidth = 120.0f;

        ShadowStyle m_activeCardShadowStyle = { 0, 0 };

        using PageArray = std::vector<Page>;

        PageArray m_pages;

        size_t m_currActivePageIndex = SIZE_T_MAX;
        size_t m_currHoverPageIndex = SIZE_T_MAX;

        using TabCloseXStateArray = std::vector<bool>;

        // Maintain state of each card's close-x button respectively.
        // Their element sizes should always equal to page-array's.
        TabCloseXStateArray m_tabCloseXHoverStates, m_tabCloseXDownStates;

        size_t m_dragCardIndex = SIZE_T_MAX;

    public:
        float CardWidth();
        void SetCardWidth(float value);

    protected:
        void LoadActiveCardShadowBitmap();

        TabState GetTabState(size_t index);

        D2D1_RECT_F TabBarPanelRect();

        D2D1_RECT_F CardRect(size_t index);
        D2D1_POINT_2F CardPosition(size_t index);

        D2D1_RECT_F TitleRect(size_t index);
        D2D1_RECT_F CloseXRect(size_t index);
        D2D1_RECT_F CloseXBkgnPanelRect(size_t index);

        void UpdateTitleAppearance(size_t index);
        void UpdateTitleAppearanceForAllPages();

        void DrawTitleLabel(Renderer* rndr, size_t index);
        void DrawCloseXButton(Renderer* rndr, size_t index);

    public:
        // Override interface methods.

        // IDrawObject2D
        void OnRendererDrawD2D1LayerHelper(Renderer* rndr) override;

        void OnRendererDrawD2D1ObjectHelper(Renderer* rndr) override;

        // Panel
        bool IsHit(Event::Point& p) override;

        float MinimalWidth() override;

        void OnSizeHelper(SizeEvent& e) override;

        void OnChangeThemeHelper(WstrViewParam themeName) override;

        bool OnMouseButtonHelper(MouseButtonEvent& e) override;

        bool OnMouseMoveHelper(MouseMoveEvent& e) override;
    };
}