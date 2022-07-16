#include "Common/Precompile.h"

#include "UIKit/ToggleButton.h"

namespace d14engine::uikit
{
    ToggleButton::ToggleButton(
        WstrParam text,
        const D2D1_RECT_F& rect,
        float roundRadius,
        ComPtrParam<ID2D1Bitmap1> icon)
        :
        Panel(rect, Resu::SOLID_COLOR_BRUSH),
        FilledButton(text, rect, roundRadius, icon) { }

    void ToggleButton::OnChangeThemeHelper(WstrViewParam themeName)
    {
        FilledButton::OnChangeThemeHelper(themeName);

        if (themeName == L"Light")
        {
            activatedAppearances[(size_t)Button::State::Idle] =
            {
                activatedAppearances[(size_t)Button::State::Idle].bitmap, // bitmap
                1.0f, // bitmap opacity

                // foreground
                {
                    D2D1::ColorF{ 0xffffff }, // color
                    1.0f // opacity
                },
                // background
                {
                    D2D1::ColorF{ 0xbf2424 }, // color
                    1.0f // opacity
                },
                // stroke
                {
                    0.0f, // width
                    D2D1::ColorF{ 0x000000 }, // color
                    0.0f // opacity
                }
            };
            activatedAppearances[(size_t)Button::State::Hover] =
            {
                activatedAppearances[(size_t)Button::State::Hover].bitmap, // bitmap
                1.0f, // bitmap opacity

                // foreground
                {
                    D2D1::ColorF{ 0xffffff }, // color
                    1.0f // opacity
                },
                // background
                {
                    D2D1::ColorF{ 0xd92929 }, // color
                    1.0f // opacity
                },
                // stroke
                {
                    0.0f, // width
                    D2D1::ColorF{ 0x000000 }, // color
                    0.0f // opacity
                }
            };
            activatedAppearances[(size_t)Button::State::Down] =
            {
                activatedAppearances[(size_t)Button::State::Down].bitmap, // bitmap
                0.65f, // bitmap opacity

                // foreground
                {
                    D2D1::ColorF{ 0xffffff }, // color
                    0.65f // opacity
                },
                // background
                {
                    D2D1::ColorF{ 0xcc2727 }, // color
                    1.0f // opacity
                },
                // stroke
                {
                    0.0f, // width
                    D2D1::ColorF{ 0x000000 }, // color
                    0.0f // opacity
                }
            };
        }
        else if (themeName == L"Dark")
        {
            activatedAppearances[(size_t)Button::State::Idle] =
            {
                activatedAppearances[(size_t)Button::State::Idle].bitmap, // bitmap
                1.0f, // bitmap opacity

                // foreground
                {
                    D2D1::ColorF{ 0x000000 }, // color
                    1.0f // opacity
                },
                // background
                {
                    D2D1::ColorF{ 0x32995f }, // color
                    1.0f // opacity
                },
                // stroke
                {
                    0.0f, // width
                    D2D1::ColorF{ 0x000000 }, // color
                    0.0f // opacity
                }
            };
            activatedAppearances[(size_t)Button::State::Hover] =
            {
                activatedAppearances[(size_t)Button::State::Hover].bitmap, // bitmap
                1.0f, // bitmap opacity

                // foreground
                {
                    D2D1::ColorF{ 0x000000 }, // color
                    1.0f // opacity
                },
                // background
                {
                    D2D1::ColorF{ 0x37a667 }, // color
                    1.0f // opacity
                },
                // stroke
                {
                    0.0f, // width
                    D2D1::ColorF{ 0x000000 }, // color
                    0.0f // opacity
                }
            };
            activatedAppearances[(size_t)Button::State::Down] =
            {
                activatedAppearances[(size_t)Button::State::Down].bitmap, // bitmap
                0.55f, // bitmap opacity

                // foreground
                {
                    D2D1::ColorF{ 0x000000 }, // color
                    0.55f // opacity
                },
                // background
                {
                    D2D1::ColorF{ 0x2e8c57 }, // color
                    1.0f // opacity
                },
                // stroke
                {
                    0.0f, // width
                    D2D1::ColorF{ 0x000000 }, // color
                    0.0f // opacity
                }
            };
        }
        UpdateAppearanceSetting(Button::State::Idle);
    }

    void ToggleButton::OnMouseButtonReleaseHelper(Button::Event& e)
    {
        if (e.Left())
        {
            // Switch current active state.
            if (m_state.activeFlag == StatefulObjectType::State::ActiveFlag::Activated)
            {
                m_state.activeFlag = StatefulObjectType::State::ActiveFlag::Inactivated;
            }
            else m_state.activeFlag = StatefulObjectType::State::ActiveFlag::Activated;

            // Trigger state changing event.
            StatefulObjectType::Event soe = {};
            soe.flag = m_state.activeFlag;

            OnStateChange(soe);
        }
        // Call this after state changed to update the appearance setting incidentally.
        FilledButton::OnMouseButtonReleaseHelper(e);
    }

    void ToggleButton::UpdateAppearanceSetting(Button::State state)
    {
        if (m_state.activeFlag == StatefulObjectType::State::ActiveFlag::Activated)
        {
            auto& setting = activatedAppearances[(size_t)state];

            bitmap = setting.bitmap;
            bitmapOpacity = setting.bitmapOpacity;

            textLabel->foreground = setting.foreground;
            background = setting.background;

            stroke = setting.stroke;
        }
        else FilledButton::UpdateAppearanceSetting(state);
    }
}