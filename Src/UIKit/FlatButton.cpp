#include "Common/Precompile.h"

#include "UIKit/FlatButton.h"

namespace d14engine::uikit
{
    FlatButton::FlatButton(
        WstrParam text,
        const D2D1_RECT_F& rect,
        float roundRadius,
        ComPtrParam<ID2D1Bitmap1> icon)
        :
        Panel(rect, Resu::SOLID_COLOR_BRUSH),
        Button(text, rect, roundRadius, icon) { }

    void FlatButton::OnInitializeFinish()
    {
        Button::OnInitializeFinish();

        // Note OnChangeThemeHelper has been called in OnInitializeFinish,
        // so we need to update the appearance settings immediately here.
        UpdateAppearanceSetting(State::Idle);
    }

    void FlatButton::OnChangeThemeHelper(WstrViewParam themeName)
    {
        Button::OnChangeThemeHelper(themeName);

        if (themeName == L"Light")
        {
            appearances[(size_t)State::Idle] =
            {
                appearances[(size_t)State::Idle].bitmap, // bitmap
                1.0f, // bitmap opacity

                // foreground
                {
                    D2D1::ColorF{ 0x000000 }, // color
                    1.0f // opacity
                },
                // background
                {
                    D2D1::ColorF{ 0x000000 }, // color
                    0.0f // opacity
                },
                // stroke
                {
                    0.0f, // width
                    D2D1::ColorF{ 0x000000 }, // color
                    0.0f // opacity
                }
            };
            appearances[(size_t)State::Hover] =
            {
                appearances[(size_t)State::Hover].bitmap, // bitmap
                1.0f, // bitmap opacity

                // foreground
                {
                    D2D1::ColorF{ 0x000000 }, // color
                    1.0f // opacity
                },
                // background
                {
                    D2D1::ColorF{ 0x000000 }, // color
                    0.06f // opacity
                },
                // stroke
                {
                    0.0f, // width
                    D2D1::ColorF{ 0x000000 }, // color
                    0.0f // opacity
                }
            };
            appearances[(size_t)State::Down] =
            {
                appearances[(size_t)State::Down].bitmap, // bitmap
                0.65f, // bitmap opacity

                // foreground
                {
                    D2D1::ColorF{ 0x000000 }, // color
                    0.65f // opacity
                },
                // background
                {
                    D2D1::ColorF{ 0x000000 }, // color
                    0.03f // opacity
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
            appearances[(size_t)State::Idle] =
            {
                appearances[(size_t)State::Idle].bitmap, // bitmap
                1.0f, // bitmap opacity

                // foreground
                {
                    D2D1::ColorF{ 0xe5e5e5 }, // color
                    1.0f // opacity
                },
                // background
                {
                    D2D1::ColorF{ 0x000000 }, // color
                    0.0f // opacity
                },
                // stroke
                {
                    0.0f, // width
                    D2D1::ColorF{ 0x000000 }, // color
                    0.0f // opacity
                }
            };
            appearances[(size_t)State::Hover] =
            {
                appearances[(size_t)State::Hover].bitmap, // bitmap
                1.0f, // bitmap opacity

                // foreground
                {
                    D2D1::ColorF{ 0xe5e5e5 }, // color
                    1.0f // opacity
                },
                // background
                {
                    D2D1::ColorF{ 0xffffff }, // color
                    0.06f // opacity
                },
                // stroke
                {
                    0.0f, // width
                    D2D1::ColorF{ 0x000000 }, // color
                    0.0f // opacity
                }
            };
            appearances[(size_t)State::Down] =
            {
                appearances[(size_t)State::Down].bitmap, // bitmap
                0.55f, // bitmap opacity

                // foreground
                {
                    D2D1::ColorF{ 0xe5e5e5 }, // color
                    0.55f // opacity
                },
                // background
                {
                    D2D1::ColorF{ 0xffffff }, // color
                    0.03f // opacity
                },
                // stroke
                {
                    0.0f, // width
                    D2D1::ColorF{ 0x000000 }, // color
                    0.0f // opacity
                }
            };
        }
        UpdateAppearanceSetting(State::Idle);
    }

    void FlatButton::UpdateAppearanceSetting(State state)
    {
        auto& setting = appearances[(size_t)state];

        bitmap = setting.bitmap;
        bitmapOpacity = setting.bitmapOpacity;

        textLabel->foreground = setting.foreground;
        background = setting.background;

        stroke = setting.stroke;
    }
}