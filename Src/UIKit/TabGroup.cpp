#include "Common/Precompile.h"

#include "UIKit/TabGroup.h"

#include "Renderer/MathUtils.h"
using namespace d14engine::renderer;

#include "UIKit/Application.h"
#include "UIKit/BitmapUtils.h"
#include "UIKit/ResourceUtils.h"

namespace d14engine::uikit
{
    TabGroup::TabGroup(const D2D1_RECT_F& rect)
        :
        Panel(rect, Resu::SOLID_COLOR_BRUSH),
        ResizablePanel(rect, Resu::SOLID_COLOR_BRUSH)
    {
        m_takeOverChildrenDrawing = true;

        SetResizable(false);
    }

    void TabGroup::OnInitializeFinish()
    {
        ResizablePanel::OnInitializeFinish();

        LoadActiveCardShadowBitmap();
    }

    TabGroup::Page* TabGroup::FindPage(size_t index)
    {
        return (index >= 0 && index < m_pages.size()) ? &m_pages[index] : nullptr;
    }

    TabGroup::Page* TabGroup::FindPage(WstrParam title)
    {
        auto pageItor = std::find_if(m_pages.begin(), m_pages.end(),
            [&](const Page& elem) { return elem.title->Text() == title; });

        return (pageItor != m_pages.end()) ? &(*pageItor) : nullptr;
    }

    void TabGroup::SelectPage(size_t index)
    {
        if (index >= 0 && index < m_pages.size())
        {
            // Try to disable old active page's content and enable new one.
            if (m_currActivePageIndex >= 0 && m_currActivePageIndex < m_pages.size())
            {
                m_pages[m_currActivePageIndex].content->SetEnabled(false);
            }
            m_pages[m_currActivePageIndex = index].content->SetEnabled(true);
        }
        else m_currActivePageIndex = SIZE_T_MAX;
    }

    void TabGroup::SelectPage(WstrParam title)
    {
        auto pageItor = std::find_if(m_pages.begin(), m_pages.end(),
            [&](const Page& elem) { return elem.title->Text() == title; });

        SelectPage(pageItor - m_pages.begin());
    }

    void TabGroup::InsertPage(const Page& page, size_t index)
    {
        // Note "index == m_page.size()" should be taken into account.
        if (index >= 0 && index <= m_pages.size())
        {
            // Maximum card count reached; stop inserting new page.
            if (m_cardWidth * (m_pages.size() + 1) > Width()) return;

            if (page.title == nullptr || page.content == nullptr) return;
            Panel::AddUIObject(page.title);
            Panel::AddUIObject(page.content);

            // Hide and disable new inserted page's content by default.
            // Note there's no need to SetVisible(false) in practice,
            // since only current active page's content will be drawn.
            page.content->SetEnabled(false);
            // Config as center UI object immediately after inserted.
            page.content->Transform(SelfCoordRect());

            if (m_currActivePageIndex >= 0 && m_currActivePageIndex < m_pages.size())
            {
                if (index <= m_currActivePageIndex) ++m_currActivePageIndex;
            }
            m_currHoverPageIndex = SIZE_T_MAX;

            m_pages.insert(m_pages.begin() + index, page);

            m_tabCloseXHoverStates.insert(m_tabCloseXHoverStates.begin() + index, false);
            m_tabCloseXDownStates.insert(m_tabCloseXDownStates.begin() + index, false);
        }
    }

    void TabGroup::AppendPage(const Page& page)
    {
        InsertPage(page, m_pages.size());
    }

    void TabGroup::RemovePage(size_t index)
    {
        if (index >= 0 && index < m_pages.size())
        {
            auto& page = m_pages[index];

            Panel::RemoveUIObject(page.title);
            Panel::RemoveUIObject(page.content);

            if (index == m_currActivePageIndex)
            {
                m_currActivePageIndex = SIZE_T_MAX;
            }
            if (index == m_currHoverPageIndex)
            {
                if (m_currActivePageIndex >= 0 && m_currActivePageIndex < m_pages.size())
                {
                    // Only check "<" here. "==" means current active page will be removed,
                    // so no need to decrement index (already set to SIZE_T_MAX above).
                    if (index < m_currActivePageIndex) --m_currActivePageIndex;
                }
                m_currHoverPageIndex = SIZE_T_MAX;
            }

            m_pages.erase(m_pages.begin() + index);

            m_tabCloseXHoverStates.erase(m_tabCloseXHoverStates.begin() + index);
            m_tabCloseXDownStates.erase(m_tabCloseXDownStates.begin() + index);
        }

        // When click tab's close-x button to remove a page, the card will flicker suddenly,
        // which is caused by delayed updating (doesn't change its appearance immediately).
        // The solution here is broadcasting related event callback manually to force updating.
        MouseMoveEvent immediateMouseMove = {};
        immediateMouseMove.cursorPoint = Application::APP->MainCursor()->AbsolutePosition();

        // Only need to update self appearance, no need to broadcast the event to children.
        m_skipDeliverNextMouseMoveEventToChildren = true;

        OnMouseMoveHelper(immediateMouseMove);
    }

    void TabGroup::RemovePage(WstrParam title)
    {
        auto pageItor = std::find_if(m_pages.begin(), m_pages.end(),
            [&](const Page& elem) { return elem.title->Text() == title; });

        RemovePage(pageItor - m_pages.begin());
    }

    size_t TabGroup::AvailablePageCount()
    {
        return m_pages.size();
    }

    float TabGroup::CardWidth()
    {
        return m_cardWidth;
    }

    void TabGroup::SetCardWidth(float value)
    {
        if (m_pages.size() > 0)
        {
            m_cardWidth = std::min(Width() / (float)m_pages.size(), value);
        }
        else m_cardWidth = std::min(Width(), value);

        LoadActiveCardShadowBitmap();
    }

    void TabGroup::LoadActiveCardShadowBitmap()
    {
        auto& activeSetting = tabAppearances[(size_t)TabState::Active];

        m_activeCardShadowStyle.LoadShadowBitmap(
            Mathu::Rounding(m_cardWidth), Mathu::Rounding(activeSetting.card.height));
    }

    TabGroup::TabState TabGroup::GetTabState(size_t index)
    {
        if (index == m_currActivePageIndex)
        {
            return TabState::Active;
        }
        else if (index == m_currHoverPageIndex)
        {
            return TabState::Hover;
        }
        else return TabState::Dormant;
    }

    D2D1_RECT_F TabGroup::TabBarPanelRect()
    {
        auto& dormantSetting = tabAppearances[(size_t)TabState::Dormant];
        return
        {
            m_absoluteRect.left,
            m_absoluteRect.top - dormantSetting.card.height,
            m_absoluteRect.right,
            m_absoluteRect.top + dormantSetting.card.roundRadius
        };
    }

    D2D1_RECT_F TabGroup::CardRect(size_t index)
    {
        auto state = GetTabState(index);

        auto& cardSetting = tabAppearances[(size_t)GetTabState(index)].card;
        D2D1_RECT_F cardRect =
        {
            m_absoluteRect.left + m_cardWidth * (float)index,
            m_absoluteRect.top - cardSetting.height,
            m_absoluteRect.left + m_cardWidth * (float)(index + 1),
            m_absoluteRect.top + cardSetting.roundRadius
        };

        if (state == TabState::Active)
        {
            // Note active-card will be drawn with shadow-bitmap,
            // so it doesn't occupy a complete rectangle area.
            cardRect.bottom = m_absoluteRect.top;
            // Make sure active-card can cover Panel's stroke-boundary.
            return Mathu::Offset(cardRect, { 0.0f, stroke.width });
        }
        else return cardRect;
    }

    D2D1_POINT_2F TabGroup::CardPosition(size_t index)
    {
        auto cardRect = CardRect(index);
        return { cardRect.left, cardRect.top };
    }

    D2D1_RECT_F TabGroup::IconRect(size_t index)
    {
        auto& setting = tabAppearances[(size_t)GetTabState(index)];

        auto cardPosition = CardPosition(index);

        D2D1_RECT_F iconRect = {};
        iconRect.left = cardPosition.x + setting.icon.leftOffset;
        iconRect.right = iconRect.left + setting.icon.size.width;
        iconRect.top = cardPosition.y + (setting.card.height - setting.icon.size.height) * 0.5f;
        iconRect.bottom = iconRect.top + setting.icon.size.height;

        return iconRect;
    }

    D2D1_RECT_F TabGroup::TitleRect(size_t index)
    {
        auto& setting = tabAppearances[(size_t)GetTabState(index)];

        auto cardPosition = CardPosition(index);

        D2D1_RECT_F titleRect = {};
        titleRect.left = cardPosition.x + setting.title.leftOffset;
        titleRect.right = titleRect.left + setting.title.size.width;
        titleRect.top = cardPosition.y + (setting.card.height - setting.title.size.height) * 0.5f;
        titleRect.bottom = titleRect.top + setting.title.size.height;
        
        return titleRect;
    }

    D2D1_RECT_F TabGroup::CloseXRect(size_t index)
    {
        auto& setting = tabAppearances[(size_t)GetTabState(index)];

        auto cardRect = CardRect(index);

        D2D1_RECT_F closeXRect = {};
        closeXRect.right = cardRect.right - setting.closeX.rightOffset;
        closeXRect.left = closeXRect.right - setting.closeX.size.width;
        closeXRect.top = cardRect.top + (setting.card.height - setting.closeX.size.height) * 0.5f;
        closeXRect.bottom = closeXRect.top + setting.closeX.size.height;

        return closeXRect;
    }

    D2D1_RECT_F TabGroup::CloseXBkgnPanelRect(size_t index)
    {
        auto& closeXSetting = tabAppearances[(size_t)GetTabState(index)].closeX;
        return Mathu::Stretch(
            CloseXRect(index),
            {
                closeXSetting.bkgnPanelOffset,
                closeXSetting.bkgnPanelOffset
            });
    }

    void TabGroup::DrawCardIcon(Renderer* rndr, size_t index)
    {
        auto& setting = tabAppearances[(size_t)GetTabState(index)].icon;

        if (setting.bitmap != nullptr)
        {
            rndr->d2d1DeviceContext->DrawBitmap(
                setting.bitmap.Get(), IconRect(index), setting.bitmapOpacity);
        }
    }

    void TabGroup::DrawTitleLabel(Renderer* rndr, size_t index)
    {
        if (index >= 0 && index < m_pages.size())
        {
            auto& page = m_pages[index];
            auto& setting = tabAppearances[(size_t)GetTabState(index)].title;

            // Position & Size
            auto titleRect = AbsoluteToSelfCoord(TitleRect(index));

            page.title->Transform(
                titleRect.left,
                titleRect.top,
                titleRect.right - titleRect.left,
                titleRect.bottom - titleRect.top);

            // Color Scheme & Text Format
            page.title->format = setting.format;

            page.title->foreground = setting.foreground;
            page.title->background = setting.background;

            page.title->alignment.horizontal = DWRITE_TEXT_ALIGNMENT_LEADING;
            page.title->alignment.vertical = DWRITE_PARAGRAPH_ALIGNMENT_NEAR;

            page.title->OnRendererDrawD2D1Object(rndr);
        }
    }

    void TabGroup::DrawCloseXButton(Renderer* rndr, size_t index)
    {
        auto& setting = tabAppearances[(size_t)GetTabState(index)].closeX;

        auto state = TabCloseXState::Idle; // Idle
        if (index >= 0 && index < m_pages.size())
        {
            if (m_tabCloseXHoverStates[index]) state = TabCloseXState::Hover;
            if (m_tabCloseXDownStates[index]) state = TabCloseXState::Down;
        }
        auto& colorScheme = setting.colorSchemes[(size_t)state];

        // Background
        Resu::SOLID_COLOR_BRUSH->SetColor(colorScheme.background.color);
        Resu::SOLID_COLOR_BRUSH->SetOpacity(colorScheme.background.opacity);

        rndr->d2d1DeviceContext->FillRoundedRectangle(
            {
                CloseXBkgnPanelRect(index),
                setting.bkgnPanelRoundRadius,
                setting.bkgnPanelRoundRadius
            },
            Resu::SOLID_COLOR_BRUSH.Get());

        // "X" Icon
        Resu::SOLID_COLOR_BRUSH->SetColor(colorScheme.foreground.color);
        Resu::SOLID_COLOR_BRUSH->SetOpacity(colorScheme.foreground.opacity);

        auto closeXRect = CloseXRect(index);

        // Main Diagonal
        rndr->d2d1DeviceContext->DrawLine(
            { closeXRect.left, closeXRect.top },
            { closeXRect.right, closeXRect.bottom },
            Resu::SOLID_COLOR_BRUSH.Get(),
            setting.strokeWidth);

        // Back Diagonal
        rndr->d2d1DeviceContext->DrawLine(
            { closeXRect.right, closeXRect.top },
            { closeXRect.left, closeXRect.bottom },
            Resu::SOLID_COLOR_BRUSH.Get(),
            setting.strokeWidth);
    }

    void TabGroup::TriggerPagePromoteEvent(MouseMoveEvent& e)
    {
        if (m_dragCardIndex >= 0 && m_dragCardIndex < m_pages.size())
        {
            auto w = PromotePageToWindow(m_dragCardIndex);

            w->MoveTopmost();

            // We decide the window should be able to demote back again.
            w->RegisterTabGroup(std::dynamic_pointer_cast<TabGroup>(shared_from_this()));

            // TODO: support promote to child window (always root for the time being).
            w->RegisterDrawObjects();
            w->RegisterApplicationEvents();

            auto dragPoint = Mathu::IncreaseY(m_dragPoint, tabBarExtendedHeight);
            // Make sure the cursor is within the window's title panel,
            // and thus the immediate mouse-button event can trigger dragging.
            w->Move(e.cursorPoint.x - dragPoint.x, e.cursorPoint.y - dragPoint.y);

            // Continue dragging window if mouse button keep pressed.
            MouseButtonEvent immediateMouseButton = {};
            immediateMouseButton.cursorPoint = e.cursorPoint;
            immediateMouseButton.status = { MouseButtonEvent::Status::Flag::LeftDown };

            w->OnMouseButton(immediateMouseButton);
        }
    }

    SharedPtr<Window> TabGroup::PromotePageToWindow(size_t index)
    {
        if (index >= 0 && index < m_pages.size())
        {
            Page page = m_pages[index];
            RemovePage(index);

            // Wrap page's content within new window's client area.
            D2D1_RECT_F initialRect = page.content->AbsoluteRect();

            initialRect.right = initialRect.left + std::max(
                initialRect.right - initialRect.left, Window::NonClientAreaMinimalWidth());

            initialRect = Mathu::IncreaseTop(initialRect, -Window::NonClientAreaHeight());

            // Create new unregistered window.
            auto w = MakeUIObject<Window>(page.title->Text(), initialRect);

            // Inherit original size constraints of pgae's content.
            w->minimalWidth = std::max(MinimalWidth(), Window::NonClientAreaMinimalWidth());
            w->maximalWidth = MaximalWidth();

            w->minimalHeight = MinimalHeight() + Window::NonClientAreaHeight();
            w->maximalHeight = MaximalHeight() + Window::NonClientAreaHeight();

            // Set page's content as new window's center UI object.
            w->SetCenterUIObject(page.content);

            return w;
        }
        else return nullptr;
    }

    bool TabGroup::IsTemporaryWindowDragAbove()
    {
        if (!temporaryAssociatedWindow.expired())
        {
            auto targetWindow = temporaryAssociatedWindow.lock();
            auto targetTabGroup = targetWindow->temporaryAssociatedTabGroup.lock();

            return targetTabGroup.get() == this;
        }
        else return false;
    }

    void TabGroup::OnRendererDrawD2D1LayerHelper(Renderer* rndr)
    {
        ResizablePanel::OnRendererDrawD2D1LayerHelper(rndr);

        if (m_currActivePageIndex >= 0 && m_currActivePageIndex < m_pages.size())
        {
            // Only need to prepare layer for content.
            if (m_pages[m_currActivePageIndex].content->IsD2D1ObjectVisible())
            {
                m_pages[m_currActivePageIndex].content->OnRendererDrawD2D1Layer(rndr);
            }
            // Active-card on Mask && Shape of Shadow
            m_activeCardShadowStyle.BeginDrawOnShadow(rndr->d2d1DeviceContext.Get());
            {
                auto& cardSetting = tabAppearances[(size_t)TabState::Active].card;

                ComPtr<ID2D1PathGeometry> pathGeo;
                THROW_IF_FAILED(rndr->d2d1Factory->CreatePathGeometry(&pathGeo));

                ComPtr<ID2D1GeometrySink> geoSink;
                THROW_IF_FAILED(pathGeo->Open(&geoSink));
                {
                    geoSink->BeginFigure({ 0.0f, cardSetting.height }, D2D1_FIGURE_BEGIN_FILLED);

                    // Left Bottom Corner
                    geoSink->AddArc(
                        {
                            { cardSetting.roundRadius, cardSetting.height - cardSetting.roundRadius }, // point
                            { cardSetting.roundRadius, cardSetting.roundRadius }, // size
                            90.0f, // rotation degrees
                            D2D1_SWEEP_DIRECTION_COUNTER_CLOCKWISE, // sweep direction
                            D2D1_ARC_SIZE_SMALL // arc size
                        });

                    geoSink->AddLine({ cardSetting.roundRadius, cardSetting.roundRadius });

                    // Left Top Corner
                    geoSink->AddArc(
                        {
                            { 2.0f * cardSetting.roundRadius, 0.0f }, // point
                            { cardSetting.roundRadius, cardSetting.roundRadius }, // size
                            90.0f, // rotation degrees
                            D2D1_SWEEP_DIRECTION_CLOCKWISE, // sweep direction
                            D2D1_ARC_SIZE_SMALL // arc size
                        });

                    geoSink->AddLine({ m_cardWidth - 2.0f * cardSetting.roundRadius, 0.0f });

                    // Right Top Corner
                    geoSink->AddArc(
                        {
                            { m_cardWidth - cardSetting.roundRadius, cardSetting.roundRadius }, // point
                            { cardSetting.roundRadius, cardSetting.roundRadius }, // size
                            90.0f, // rotation degrees
                            D2D1_SWEEP_DIRECTION_CLOCKWISE, // sweep direction
                            D2D1_ARC_SIZE_SMALL // arc size
                        });

                    geoSink->AddLine({ m_cardWidth - cardSetting.roundRadius, cardSetting.height - cardSetting.roundRadius });

                    // Right Bottom Corner
                    geoSink->AddArc(
                        {
                            { m_cardWidth, cardSetting.height }, // point
                            { cardSetting.roundRadius, cardSetting.roundRadius }, // size
                            90.0f, // rotation degrees
                            D2D1_SWEEP_DIRECTION_COUNTER_CLOCKWISE, // sweep direction
                            D2D1_ARC_SIZE_SMALL // arc size
                        });

                    geoSink->AddLine({ 0.0f, cardSetting.height });

                    geoSink->EndFigure(D2D1_FIGURE_END_CLOSED);
                }
                THROW_IF_FAILED(geoSink->Close());

                Resu::SOLID_COLOR_BRUSH->SetColor(cardSetting.background.color);
                Resu::SOLID_COLOR_BRUSH->SetOpacity(cardSetting.background.opacity);

                rndr->d2d1DeviceContext->FillGeometry(pathGeo.Get(), Resu::SOLID_COLOR_BRUSH.Get());
            }
            m_activeCardShadowStyle.EndDrawOnShadow(rndr->d2d1DeviceContext.Get());
        }
    }

    void TabGroup::OnRendererDrawD2D1ObjectHelper(Renderer* rndr)
    {
        // Tab Bar Panel
        {
            auto& cardSetting = tabAppearances[(size_t)TabState::Dormant].card;

            Resu::SOLID_COLOR_BRUSH->SetColor(cardSetting.background.color);
            Resu::SOLID_COLOR_BRUSH->SetOpacity(cardSetting.background.opacity);

            rndr->d2d1DeviceContext->FillRoundedRectangle(
                {
                    TabBarPanelRect(),
                    cardSetting.roundRadius,
                    cardSetting.roundRadius
                },
                Resu::SOLID_COLOR_BRUSH.Get());
        }
        // Shadow of Active-card
        if (m_currActivePageIndex >= 0 && m_currActivePageIndex < m_pages.size())
        {
            m_activeCardShadowStyle.ConfigShadowEffectInput(Resu::SHADOW_EFFECT.Get());

            rndr->d2d1DeviceContext->DrawImage(Resu::SHADOW_EFFECT.Get(), CardPosition(m_currActivePageIndex));
        }
        // Foreground of Hover-card
        if (m_currHoverPageIndex >= 0 && m_currHoverPageIndex < m_pages.size())
        {
            // No need to draw again when hover an active-card.
            if (m_currHoverPageIndex != m_currActivePageIndex)
            {
                auto& cardSetting = tabAppearances[(size_t)TabState::Hover].card;

                Resu::SOLID_COLOR_BRUSH->SetColor(cardSetting.background.color);
                Resu::SOLID_COLOR_BRUSH->SetOpacity(cardSetting.background.opacity);

                rndr->d2d1DeviceContext->FillRoundedRectangle(
                    {
                        CardRect(m_currHoverPageIndex),
                        cardSetting.roundRadius,
                        cardSetting.roundRadius
                    },
                    Resu::SOLID_COLOR_BRUSH.Get());
            }
        }
        // Background
        {
            Resu::SOLID_COLOR_BRUSH->SetColor(background.color);
            Resu::SOLID_COLOR_BRUSH->SetOpacity(background.opacity);

            ResizablePanel::DrawBackground(rndr);
        }
        // Content
        if (m_currActivePageIndex >= 0 && m_currActivePageIndex < m_pages.size())
        {
            if (m_pages[m_currActivePageIndex].content->IsD2D1ObjectVisible())
            {
                m_pages[m_currActivePageIndex].content->OnRendererDrawD2D1Object(rndr);
            }
        }
        // Outline
        {
            Resu::SOLID_COLOR_BRUSH->SetColor(stroke.color);
            Resu::SOLID_COLOR_BRUSH->SetOpacity(stroke.opacity);

            auto innerRect = Mathu::Stretch(m_absoluteRect, { -stroke.width * 0.5f, -stroke.width * 0.5f });

            rndr->d2d1DeviceContext->DrawRoundedRectangle(
                { innerRect, roundRadiusX, roundRadiusY }, Resu::SOLID_COLOR_BRUSH.Get(), stroke.width);
        }
        // Foreground of Active-card
        if (m_currActivePageIndex >= 0 && m_currActivePageIndex < m_pages.size())
        {
            auto& cardSetting = tabAppearances[(size_t)TabState::Active].card;

            rndr->d2d1DeviceContext->DrawBitmap(
                m_activeCardShadowStyle.bitmap.Get(),
                CardRect(m_currActivePageIndex),
                cardSetting.background.opacity);
        }
        // Separator of Dormant-card
        {
            auto& cardSetting = tabAppearances[(size_t)TabState::Dormant].card;

            for (size_t cardIndex = 0; cardIndex < m_pages.size(); ++cardIndex)
            {
                auto& page = m_pages[cardIndex];

                if (GetTabState(cardIndex) == TabState::Dormant)
                {
                    if (cardIndex != m_pages.size() - 1 && GetTabState(cardIndex + 1) == TabState::Dormant)
                    {
                        // Draw separators between dormant tabs.
                        Resu::SOLID_COLOR_BRUSH->SetColor(tabSeparatorStyle.background.color);
                        Resu::SOLID_COLOR_BRUSH->SetOpacity(tabSeparatorStyle.background.opacity);

                        auto cardRect = CardRect(cardIndex);

                        float lineTop = cardRect.top + (cardSetting.height - tabSeparatorStyle.size.height) * 0.5f;
                        float lineBottom = lineTop + tabSeparatorStyle.size.width;

                        rndr->d2d1DeviceContext->DrawLine(
                            { cardRect.right, lineTop },
                            { cardRect.right, lineBottom },
                            Resu::SOLID_COLOR_BRUSH.Get(),
                            tabSeparatorStyle.size.width);
                    }
                }
                // Draw icon, title && close-x for all tabs.
                DrawCardIcon(rndr, cardIndex);
                DrawTitleLabel(rndr, cardIndex);
                DrawCloseXButton(rndr, cardIndex);
            }
        }
        // Mask when Below Drag Window
        if (IsTemporaryWindowDragAbove())
        {
            Resu::SOLID_COLOR_BRUSH->SetColor(maskStyleWhenBelowDragWindow.color);
            Resu::SOLID_COLOR_BRUSH->SetOpacity(maskStyleWhenBelowDragWindow.opacity);

            rndr->d2d1DeviceContext->FillRoundedRectangle(
                { m_absoluteRect, roundRadiusX, roundRadiusY }, Resu::SOLID_COLOR_BRUSH.Get());
        }
    }

    bool TabGroup::IsHitHelper(Event::Point& p)
    {
        return Mathu::IsOverlapped(p, Mathu::IncreaseTop(
            ResizeFrameExtendedRect(m_absoluteRect), -tabBarExtendedHeight));
    }

    float TabGroup::MinimalWidth()
    {
        return m_cardWidth * (float)m_pages.size();
    }

    void TabGroup::OnSizeHelper(SizeEvent& e)
    {
        ResizablePanel::OnSizeHelper(e);

        for (size_t cardIndex = 0; cardIndex < m_pages.size(); ++cardIndex)
        {
            auto& page = m_pages[cardIndex];

            // Keep content panel.
            page.content->Transform(SelfCoordRect());

            // Update title-label.
            auto titleRect = AbsoluteToSelfCoord(TitleRect(cardIndex));

            page.title->Transform(
                titleRect.left,
                titleRect.top,
                titleRect.right - titleRect.left,
                titleRect.bottom - titleRect.top);
        }
    }

    void TabGroup::OnChangeThemeHelper(WstrViewParam themeName)
    {
        ResizablePanel::OnChangeThemeHelper(themeName);

        if (themeName == L"Light")
        {
            background.color = D2D1::ColorF{ 0xf2f2f2 };
            background.opacity = 1.0f;

            stroke.width = 1.0f;

            stroke.color = D2D1::ColorF{ 0xe5e5e5 };
            stroke.opacity = 1.0f;

            tabSeparatorStyle =
            {
                // size
                {
                    1.0f, // width
                    16.0f // height
                },
                // background
                {
                    D2D1::ColorF{ 0x808080 }, // color
                    1.0f // opacity
                }
            };
            tabAppearances[(size_t)TabState::Active] =
            {
                // card appearance
                {
                    32.0f, // height
                    8.0f, // round radius

                    // background
                    {
                        D2D1::ColorF{ 0xf2f2f2 }, // color
                        1.0f // opacity
                    }
                },
                // icon appearance
                tabAppearances[(size_t)TabState::Active].icon,

                // title appearance
                {
                    { 80.0f, 20.0f }, // size
                    20.0f, // left offset

                    Resu::TEXT_FORMATS.at(L"微软雅黑/Light/12"),// format

                    // foreground
                    {
                        D2D1::ColorF{ 0x000000 }, //color
                        1.0 // opacity
                    },
                    // background
                    {
                        D2D1::ColorF{ 0x000000 }, // color
                        0.0f // opacity
                    }
                },
                // close-x appearance
                {
                    { 8.0f, 8.0f }, // size
                    20.0f, // right offset

                    3.0f, // bkgn panel offset
                    4.0f, // bkgn panel round radius

                    // color scheme
                    {
                        // idle
                        {
                            // foreground
                            {
                                 D2D1::ColorF{ 0x000000 }, // color
                                1.0f // opacity
                            },
                            // background
                            {
                                D2D1::ColorF{ 0x000000 }, // color
                                0.0f // opacity
                            }
                        },
                        // hover
                        {
                            // foreground
                            {
                                 D2D1::ColorF{ 0x000000 }, // color
                                1.0f // opacity
                            },
                            // background
                            {
                                D2D1::ColorF{ 0x000000 }, // color
                                0.1f // opacity
                            }
                        },
                        // down
                        {
                            // foreground
                            {
                                 D2D1::ColorF{ 0x000000 }, // color
                                1.0f // opacity
                            },
                            // background
                            {
                                D2D1::ColorF{ 0x000000 }, // color
                                0.2f // opacity
                            }
                        }
                    },
                    1.0f, // stroke width
                }
            };
            tabAppearances[(size_t)TabState::Hover] =
            {
                // card appearance
                {
                    24.0f, // height
                    8.0f, // round radius

                    // background
                    {
                        D2D1::ColorF{ 0xd9d9d9 }, // color
                        1.0f // opacity
                    }
                },
                // icon appearance
                tabAppearances[(size_t)TabState::Hover].icon,

                // title appearance
                {
                    { 80.0f, 20.0f }, // size
                    12.0f, // left offset

                    Resu::TEXT_FORMATS.at(L"微软雅黑/Light/12"),// format

                    // foreground
                    {
                        D2D1::ColorF{ 0x000000 }, //color
                        1.0 // opacity
                    },
                    // background
                    {
                        D2D1::ColorF{ 0x000000 }, // color
                        0.0f // opacity
                    }
                },
                // close-x appearance
                {
                    { 8.0f, 8.0f }, // size
                    12.0f, // right offset

                    3.0f, // bkgn panel offset
                    4.0f, // bkgn panel round radius

                    // color scheme
                    {
                        // idle
                        {
                            // foreground
                            {
                                 D2D1::ColorF{ 0x000000 }, // color
                                1.0f // opacity
                            },
                            // background
                            {
                                D2D1::ColorF{ 0x000000 }, // color
                                0.0f // opacity
                            }
                        },
                        // hover
                        {
                            // foreground
                            {
                                 D2D1::ColorF{ 0x000000 }, // color
                                1.0f // opacity
                            },
                            // background
                            {
                                D2D1::ColorF{ 0x000000 }, // color
                                0.1f // opacity
                            }
                        },
                        // down
                        {
                            // foreground
                            {
                                 D2D1::ColorF{ 0x000000 }, // color
                                1.0f // opacity
                            },
                            // background
                            {
                                D2D1::ColorF{ 0x000000 }, // color
                                0.2f // opacity
                            }
                        }
                    },
                    1.0f, // stroke width
                }
            };
            tabAppearances[(size_t)TabState::Dormant] =
            {
                // card appearance
                {
                    24.0f, // height
                    8.0f, // round radius

                    // background
                    {
                        D2D1::ColorF{ 0xe5e5e5 }, // color
                        1.0f // opacity
                    }
                },
                // icon appearance
                tabAppearances[(size_t)TabState::Dormant].icon,

                // title appearance
                {
                    { 80.0f, 20.0f }, // size
                    12.0f, // left offset

                    Resu::TEXT_FORMATS.at(L"微软雅黑/Light/12"),// format

                    // foreground
                    {
                        D2D1::ColorF{ 0x000000 }, //color
                        1.0 // opacity
                    },
                    // background
                    {
                        D2D1::ColorF{ 0x000000 }, // color
                        0.0f // opacity
                    }
                },
                // close-x appearance
                {
                    { 8.0f, 8.0f }, // size
                    12.0f, // right offset

                    3.0f, // bkgn panel offset
                    4.0f, // bkgn panel round radius

                    // color scheme
                    {
                        // idle
                        {
                            // foreground
                            {
                                 D2D1::ColorF{ 0x000000 }, // color
                                1.0f // opacity
                            },
                            // background
                            {
                                D2D1::ColorF{ 0x000000 }, // color
                                0.0f // opacity
                            }
                        },
                        // hover
                        {
                            // foreground
                            {
                                 D2D1::ColorF{ 0x000000 }, // color
                                1.0f // opacity
                            },
                            // background
                            {
                                D2D1::ColorF{ 0x000000 }, // color
                                0.1f // opacity
                            }
                        },
                        // down
                        {
                            // foreground
                            {
                                 D2D1::ColorF{ 0x000000 }, // color
                                1.0f // opacity
                            },
                            // background
                            {
                                D2D1::ColorF{ 0x000000 }, // color
                                0.2f // opacity
                            }
                        }
                    },
                    1.0f, // stroke width
                }
            };
            maskStyleWhenBelowDragWindow =
            {
                D2D1::ColorF{ 0xadd6ff }, // color
                0.5f // opacity
            };
        }
        else if (themeName == L"Dark")
        {
            background.color = D2D1::ColorF{ 0x1f1f1f };
            background.opacity = 1.0f;

            stroke.width = 1.0f;

            stroke.color = D2D1::ColorF{ 0x1a1a1a };
            stroke.opacity = 1.0f;

            tabSeparatorStyle =
            {
                // size
                {
                    1.0f, // width
                    16.0f // height
                },
                // background
                {
                    D2D1::ColorF{ 0x808080 }, // color
                    1.0f // opacity
                }
            };
            tabAppearances[(size_t)TabState::Active] =
            {
                // card appearance
                {
                    32.0f, // height
                    8.0f, // round radius

                    // background
                    {
                        D2D1::ColorF{ 0x1f1f1f }, // color
                        1.0f // opacity
                    }
                },
                // icon appearance
                tabAppearances[(size_t)TabState::Active].icon,

                // title appearance
                {
                    { 80.0f, 20.0f }, // size
                    20.0f, // left offset

                    Resu::TEXT_FORMATS.at(L"微软雅黑/Light/12"),// format

                    // foreground
                    {
                        D2D1::ColorF{ 0xe5e5e5 }, //color
                        1.0 // opacity
                    },
                    // background
                    {
                        D2D1::ColorF{ 0x000000 }, // color
                        0.0f // opacity
                    }
                },
                // close-x appearance
                {
                    { 8.0f, 8.0f }, // size
                    20.0f, // right offset

                    3.0f, // bkgn panel offset
                    4.0f, // bkgn panel round radius

                    // color scheme
                    {
                        // idle
                        {
                            // foreground
                            {
                                 D2D1::ColorF{ 0xffffff }, // color
                                1.0f // opacity
                            },
                            // background
                            {
                                D2D1::ColorF{ 0x000000 }, // color
                                0.0f // opacity
                            }
                        },
                        // hover
                        {
                            // foreground
                            {
                                 D2D1::ColorF{ 0xffffff }, // color
                                1.0f // opacity
                            },
                            // background
                            {
                                D2D1::ColorF{ 0xffffff }, // color
                                0.2f // opacity
                            }
                        },
                        // down
                        {
                            // foreground
                            {
                                 D2D1::ColorF{ 0xffffff }, // color
                                1.0f // opacity
                            },
                            // background
                            {
                                D2D1::ColorF{ 0xffffff }, // color
                                0.3f // opacity
                            }
                        }
                    },
                    1.0f, // stroke width
                }
            };
            tabAppearances[(size_t)TabState::Hover] =
            {
                // card appearance
                {
                    24.0f, // height
                    8.0f, // round radius

                    // background
                    {
                        D2D1::ColorF{ 0x2e2e2e }, // color
                        1.0f // opacity
                    }
                },
                // icon appearance
                tabAppearances[(size_t)TabState::Hover].icon,

                // title appearance
                {
                    { 80.0f, 20.0f }, // size
                    12.0f, // left offset

                    Resu::TEXT_FORMATS.at(L"微软雅黑/Light/12"),// format

                    // foreground
                    {
                        D2D1::ColorF{ 0xe5e5e5 }, //color
                        1.0 // opacity
                    },
                    // background
                    {
                        D2D1::ColorF{ 0x000000 }, // color
                        0.0f // opacity
                    }
                },
                // close-x appearance
                {
                    { 8.0f, 8.0f }, // size
                    12.0f, // right offset

                    3.0f, // bkgn panel offset
                    4.0f, // bkgn panel round radius

                    // color scheme
                    {
                        // idle
                        {
                            // foreground
                            {
                                 D2D1::ColorF{ 0xffffff }, // color
                                1.0f // opacity
                            },
                            // background
                            {
                                D2D1::ColorF{ 0x000000 }, // color
                                0.0f // opacity
                            }
                        },
                        // hover
                        {
                            // foreground
                            {
                                 D2D1::ColorF{ 0xffffff }, // color
                                1.0f // opacity
                            },
                            // background
                            {
                                D2D1::ColorF{ 0xffffff }, // color
                                0.2f // opacity
                            }
                        },
                        // down
                        {
                            // foreground
                            {
                                 D2D1::ColorF{ 0xffffff }, // color
                                1.0f // opacity
                            },
                            // background
                            {
                                D2D1::ColorF{ 0xffffff }, // color
                                0.3f // opacity
                            }
                        }
                    },
                    1.0f, // stroke width
                }
            };
            tabAppearances[(size_t)TabState::Dormant] =
            {
                // card appearance
                {
                    24.0f, // height
                    8.0f, // round radius

                    // background
                    {
                        D2D1::ColorF{ 0x1a1a1a }, // color
                        1.0f // opacity
                    }
                },
                // icon appearance
                tabAppearances[(size_t)TabState::Dormant].icon,

                // title appearance
                {
                    { 80.0f, 20.0f }, // size
                    12.0f, // left offset

                    Resu::TEXT_FORMATS.at(L"微软雅黑/Light/12"),// format

                    // foreground
                    {
                        D2D1::ColorF{ 0xe5e5e5 }, //color
                        1.0 // opacity
                    },
                    // background
                    {
                        D2D1::ColorF{ 0x000000 }, // color
                        0.0f // opacity
                    }
                },
                // close-x appearance
                {
                    { 8.0f, 8.0f }, // size
                    12.0f, // right offset

                    3.0f, // bkgn panel offset
                    4.0f, // bkgn panel round radius

                    // color scheme
                    {
                        // idle
                        {
                            // foreground
                            {
                                 D2D1::ColorF{ 0xffffff }, // color
                                1.0f // opacity
                            },
                            // background
                            {
                                D2D1::ColorF{ 0x000000 }, // color
                                0.0f // opacity
                            }
                        },
                        // hover
                        {
                            // foreground
                            {
                                 D2D1::ColorF{ 0xffffff }, // color
                                1.0f // opacity
                            },
                            // background
                            {
                                D2D1::ColorF{ 0xffffff }, // color
                                0.2f // opacity
                            }
                        },
                        // down
                        {
                            // foreground
                            {
                                 D2D1::ColorF{ 0xffffff }, // color
                                1.0f // opacity
                            },
                            // background
                            {
                                D2D1::ColorF{ 0xffffff }, // color
                                0.3f // opacity
                            }
                        }
                    },
                    1.0f, // stroke width
                }
            };
            maskStyleWhenBelowDragWindow =
            {
                D2D1::ColorF{ 0x264f78 }, // color
                0.5f // opacity
            };
        }
    }

    bool TabGroup::OnMouseButtonHelper(MouseButtonEvent& e)
    {
        auto& p = e.cursorPoint;

        if (e.status.LeftDown())
        {
            if (m_currHoverPageIndex >= 0 && m_currHoverPageIndex < m_pages.size())
            {
                m_tabCloseXDownStates[m_currHoverPageIndex] = m_tabCloseXHoverStates[m_currHoverPageIndex];

                // Close-x button's event takes precedence over tab's.
                if (!m_tabCloseXHoverStates[m_currHoverPageIndex])
                {
                    // Try to change active page to current selected one.
                    if (m_currHoverPageIndex != m_currActivePageIndex)
                    {
                        SelectPage(m_currHoverPageIndex);
                    }
                    m_dragCardIndex = m_currHoverPageIndex;
                    m_dragPoint = AbsoluteToSelfCoord(p);
                }
            }
        }
        else if (e.status.LeftUp())
        {
            if (m_currHoverPageIndex >= 0 && m_currHoverPageIndex < m_pages.size())
            {
                if (m_tabCloseXDownStates[m_currHoverPageIndex])
                {
                    RemovePage(m_currHoverPageIndex);
                }
            }
            m_dragCardIndex = SIZE_T_MAX;
            m_dragPoint = { 0.0f, 0.0f };
        }
        return ResizablePanel::OnMouseButtonHelper(e);
    }

    bool TabGroup::OnMouseMoveHelper(MouseMoveEvent& e)
    {
        auto& p = e.cursorPoint;

        m_currHoverPageIndex = SIZE_T_MAX;

        // Note IsHit area has already been extended with tabBarExtendedHeight,
        // so check again to decide whether current cursor is above tab bar.
        if (!Mathu::IsOverlapped(p, AbsoluteRect()))
        {
            for (size_t cardIndex = 0; cardIndex < m_pages.size(); ++cardIndex)
            {
                if (Mathu::IsInside(p, CardRect(cardIndex)))
                {
                    m_currHoverPageIndex = cardIndex;
                }
                if (!Mathu::IsInside(p, CloseXBkgnPanelRect(cardIndex)))
                {
                    m_tabCloseXHoverStates[cardIndex] = false;
                    m_tabCloseXDownStates[cardIndex] = false;
                }
                else m_tabCloseXHoverStates[cardIndex] = true;
            }
            if (e.buttonState.leftPressed)
            {
                // Adjust index of current active page.
                if (m_dragCardIndex >= 0 && m_dragCardIndex < m_pages.size())
                {
                    auto nextCardIndex = (size_t)(AbsoluteToSelfCoord(p).x / m_cardWidth);

                    if (nextCardIndex >= 0 && nextCardIndex < m_pages.size() &&
                        nextCardIndex != m_dragCardIndex) // In case trigger frequently.
                    {
                        Page page = m_pages[m_dragCardIndex];

                        RemovePage(m_dragCardIndex);
                        InsertPage(page, nextCardIndex);
                        SelectPage(nextCardIndex);

                        m_dragCardIndex = nextCardIndex;
                    }
                }
            }
        }
        else // Out of tab bar panel area.
        {
            if (e.buttonState.leftPressed)
            {
                TriggerPagePromoteEvent(e);
            }
            m_dragCardIndex = SIZE_T_MAX;
        }
        return ResizablePanel::OnMouseMoveHelper(e);
    }

    bool TabGroup::OnMouseLeaveHelper(MouseMoveEvent& e)
    {
        if (e.buttonState.leftPressed)
        {
            TriggerPagePromoteEvent(e);
        }
        return ResizablePanel::OnMouseLeaveHelper(e);
    }
}