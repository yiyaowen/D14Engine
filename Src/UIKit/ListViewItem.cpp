#include "Common/Precompile.h"

#include "UIKit/ListViewItem.h"

#include "Renderer/MathUtils.h"
using namespace d14engine::renderer;

#include "UIKit/ResourceUtils.h"

namespace d14engine::uikit
{
    ListViewItem::ListViewItem(
        const D2D1_RECT_F& rect,
        ShrdPtrParam<Panel> content)
        :
        Panel(rect, Resu::SOLID_COLOR_BRUSH),
        ClickablePanel(rect, Resu::SOLID_COLOR_BRUSH),
        m_content(content)
    {
        m_takeOverChildrenDrawing = true;
    }

    void ListViewItem::OnInitializeFinish()
    {
        ClickablePanel::OnInitializeFinish();

        // Don't call this in the ctor since it uses shared_from_this internally.
        AddUIObject(m_content);

        if (m_content != nullptr) m_content->Transform(SelfCoordRect());
    }

    SharedPtr<Panel> ListViewItem::Content()
    {
        return m_content;
    }

    void ListViewItem::SetContent(ShrdPtrParam<Panel> content)
    {
        RemoveUIObject(m_content);

        m_content = content;
        AddUIObject(content);

        // Config content as center UI object immediately.
        if (m_content != nullptr) m_content->Transform(SelfCoordRect());
    }

#define LVIT_STATE ListViewItem::State

    const ListViewItem::StateTransitionMap
    ListViewItem::ENTER_STATE_TRANS_MAP =
    {
        { LVIT_STATE::Idle, LVIT_STATE::Hover },
        { LVIT_STATE::Hover, LVIT_STATE::Hover },
        { LVIT_STATE::ActiveSelected, LVIT_STATE::ActiveSelectedHover },
        { LVIT_STATE::InactiveSelected, LVIT_STATE::InactiveSelected },
        { LVIT_STATE::ActiveSelectedHover, LVIT_STATE::ActiveSelectedHover }
    },
    ListViewItem::LEAVE_STATE_TRANS_MAP =
    {
        { LVIT_STATE::Idle, LVIT_STATE::Idle },
        { LVIT_STATE::Hover, LVIT_STATE::Idle },
        { LVIT_STATE::ActiveSelected, LVIT_STATE::ActiveSelected },
        { LVIT_STATE::InactiveSelected, LVIT_STATE::InactiveSelected },
        { LVIT_STATE::ActiveSelectedHover, LVIT_STATE::ActiveSelected }
    },
    ListViewItem::CHECK_STATE_TRANS_MAP =
    {
        { LVIT_STATE::Idle, LVIT_STATE::ActiveSelectedHover },
        { LVIT_STATE::Hover, LVIT_STATE::ActiveSelectedHover },
        { LVIT_STATE::ActiveSelected, LVIT_STATE::ActiveSelectedHover },
        { LVIT_STATE::InactiveSelected, LVIT_STATE::ActiveSelectedHover },
        { LVIT_STATE::ActiveSelectedHover, LVIT_STATE::ActiveSelectedHover }
    },
    ListViewItem::UNCHECK_STATE_TRANS_MAP =
    {
        { LVIT_STATE::Idle, LVIT_STATE::Idle },
        { LVIT_STATE::Hover, LVIT_STATE::Idle },
        { LVIT_STATE::ActiveSelected, LVIT_STATE::Idle },
        { LVIT_STATE::InactiveSelected, LVIT_STATE::Idle },
        { LVIT_STATE::ActiveSelectedHover, LVIT_STATE::Idle }
    },
    ListViewItem::GET_FOCUS_STATE_TRANS_MAP =
    {
        { LVIT_STATE::Idle, LVIT_STATE::Idle },
        { LVIT_STATE::Hover, LVIT_STATE::Hover },
        { LVIT_STATE::ActiveSelected, LVIT_STATE::ActiveSelected },
        { LVIT_STATE::InactiveSelected, LVIT_STATE::ActiveSelected },
        { LVIT_STATE::ActiveSelectedHover, LVIT_STATE::ActiveSelectedHover }
    },
    ListViewItem::LOSE_FOCUS_STATE_TRANS_MAP =
    {
        { LVIT_STATE::Idle, LVIT_STATE::Idle },
        { LVIT_STATE::Hover, LVIT_STATE::Hover },
        { LVIT_STATE::ActiveSelected, LVIT_STATE::InactiveSelected },
        { LVIT_STATE::InactiveSelected, LVIT_STATE::InactiveSelected },
        { LVIT_STATE::ActiveSelectedHover, LVIT_STATE::InactiveSelected }
    };

#undef LVIT_STATE

    void ListViewItem::OnRendererDrawD2D1LayerHelper(Renderer* rndr)
    {
        ClickablePanel::OnRendererDrawD2D1LayerHelper(rndr);

        if (m_content != nullptr && m_content->IsD2D1ObjectVisible())
        {
            m_content->OnRendererDrawD2D1Layer(rndr);
        }
        // Content on Mask. Note other children are hidden by default.
        contentMask.BeginMaskDraw(rndr->d2d1DeviceContext.Get(), D2D1::Matrix3x2F::Translation(-m_absoluteRect.left, -m_absoluteRect.top));
        {
            if (m_content != nullptr && m_content->IsD2D1ObjectVisible())
            {
                m_content->OnRendererDrawD2D1Object(rndr);
            }
        }
        contentMask.EndMaskDraw(rndr->d2d1DeviceContext.Get());
    }

    void ListViewItem::OnRendererDrawD2D1ObjectHelper(Renderer* rndr)
    {
        auto& setting = appearances[(size_t)state];

        // Background
        Resu::SOLID_COLOR_BRUSH->SetColor(setting.background.color);
        Resu::SOLID_COLOR_BRUSH->SetOpacity(setting.background.opacity);

        ClickablePanel::DrawBackground(rndr);

        // Content
        rndr->d2d1DeviceContext->DrawBitmap(contentMask.bitmap.Get(), m_absoluteRect);

        // Outline
        Resu::SOLID_COLOR_BRUSH->SetColor(setting.stroke.color);
        Resu::SOLID_COLOR_BRUSH->SetOpacity(setting.stroke.opacity);

        auto innerRect = Mathu::Stretch(
            m_absoluteRect, { -setting.stroke.width * 0.5f, -setting.stroke.width * 0.5f });

        rndr->d2d1DeviceContext->DrawRoundedRectangle(
            { innerRect, roundRadiusX, roundRadiusY },
            Resu::SOLID_COLOR_BRUSH.Get(), setting.stroke.width);
    }

    void ListViewItem::OnSizeHelper(SizeEvent& e)
    {
        ClickablePanel::OnSizeHelper(e);

        // Keep content panel.
        if (m_content != nullptr) m_content->Transform(SelfCoordRect());

        // Adjust content mask.
        contentMask.LoadMaskBitmap(Mathu::Rounding(e.size.width), Mathu::Rounding(e.size.height));
    }

    void ListViewItem::OnChangeThemeHelper(WstrViewParam themeName)
    {
        ClickablePanel::OnChangeThemeHelper(themeName);

        if (themeName == L"Light")
        {
            appearances[(size_t)State::Idle] =
            {
                appearances[(size_t)State::Idle].bitmap, // bitmap
                1.0f, // bitmap opacity

                { // background
                    D2D1::ColorF{ 0xf2f2f2 }, // color
                    1.0f // opacity
                },
                { // stroke
                    0.0f, // width
                    D2D1::ColorF{ 0x000000 }, // color
                    0.0f // opacity
                }
            };
            appearances[(size_t)State::Hover] =
            {
                appearances[(size_t)State::Hover].bitmap, // bitmap
                1.0f, // bitmap opacity

                { // background
                    D2D1::ColorF{ 0xe1eefa }, // color
                    1.0f // opacity
                },
                { // stroke
                    0.0f, // width
                    D2D1::ColorF{ 0x000000 }, // color
                    0.0f // opacity
                }
            };
            appearances[(size_t)State::ActiveSelected] =
            {
                appearances[(size_t)State::ActiveSelected].bitmap, // bitmap
                1.0f, // bitmap opacity

                { // background
                    D2D1::ColorF{ 0xcce8ff }, // color
                    1.0f // opacity
                },
                { // stroke
                    0.0f, // width
                    D2D1::ColorF{ 0x000000 }, // color
                    0.0f // opacity
                }
            };
            appearances[(size_t)State::InactiveSelected] =
            {
                appearances[(size_t)State::InactiveSelected].bitmap, // bitmap
                1.0f, // bitmap opacity

                { // background
                    D2D1::ColorF{ 0xd9d9d9 }, // color
                    1.0f // opacity
                },
                { // stroke
                    0.0f, // width
                    D2D1::ColorF{ 0x000000 }, // color
                    0.0f // opacity
                }
            };
            appearances[(size_t)State::ActiveSelectedHover] =
            {
                appearances[(size_t)State::ActiveSelectedHover].bitmap, // bitmap
                1.0f, // bitmap opacity

                { // background
                    D2D1::ColorF{ 0xcce8ff }, // color
                    1.0f // opacity
                },
                { // stroke
                    1.0f, // width
                    D2D1::ColorF{ 0x99d1ff }, // color
                    1.0f // opacity
                }
            };
        }
        else if (themeName == L"Dark")
        {
            appearances[(size_t)State::Idle] =
            {
                appearances[(size_t)State::Idle].bitmap, // bitmap
                1.0f, // bitmap opacity

                { // background
                    D2D1::ColorF{ 0x1f1f1f }, // color
                    1.0f // opacity
                },
                { // stroke
                    0.0f, // width
                    D2D1::ColorF{ 0x000000 }, // color
                    0.0f // opacity
                }
            };
            appearances[(size_t)State::Hover] =
            {
                appearances[(size_t)State::Hover].bitmap, // bitmap
                1.0f, // bitmap opacity

                { // background
                    D2D1::ColorF{ 0x2a2d2e }, // color
                    1.0f // opacity
                },
                { // stroke
                    0.0f, // width
                    D2D1::ColorF{ 0x000000 }, // color
                    0.0f // opacity
                }
            };
            appearances[(size_t)State::ActiveSelected] =
            {
                appearances[(size_t)State::ActiveSelected].bitmap, // bitmap
                1.0f, // bitmap opacity

                { // background
                    D2D1::ColorF{ 0x04395e }, // color
                    1.0f // opacity
                },
                { // stroke
                    0.0f, // width
                    D2D1::ColorF{ 0x000000 }, // color
                    0.0f // opacity
                }
            };
            appearances[(size_t)State::InactiveSelected] =
            {
                appearances[(size_t)State::InactiveSelected].bitmap, // bitmap
                1.0f, // bitmap opacity

                { // background
                    D2D1::ColorF{ 0x37373d }, // color
                    1.0f // opacity
                },
                { // stroke
                    0.0f, // width
                    D2D1::ColorF{ 0x000000 }, // color
                    0.0f // opacity
                }
            };
            appearances[(size_t)State::ActiveSelectedHover] =
            {
                appearances[(size_t)State::ActiveSelectedHover].bitmap, // bitmap
                1.0f, // bitmap opacity

                { // background
                    D2D1::ColorF{ 0x04395e }, // color
                    1.0f // opacity
                },
                { // stroke
                    1.0f, // width
                    D2D1::ColorF{ 0x007fd4 }, // color
                    1.0f // opacity
                }
            };
        }
    }
}