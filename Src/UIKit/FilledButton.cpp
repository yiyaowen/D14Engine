#include "Common/Precompile.h"

#include "UIKit/FilledButton.h"

namespace d14engine::uikit
{
    FilledButton::FilledButton(
        WstrParam text,
        const D2D1_RECT_F& rect,
        float roundRadius,
        ComPtrParam<ID2D1Bitmap1> icon)
        :
        Panel(rect, Resu::SOLID_COLOR_BRUSH),
        FlatButton(text, rect, roundRadius, icon) { }

    void FilledButton::OnChangeThemeHelper(WstrViewParam themeName)
    {
        // Call Button's OnChangeThemeHelper instead of FlatButton's since the appearance states
        // would be configed in this method (no need to use the states configed in FlatButton).
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
                    D2D1::ColorF{ 0xfafafa }, // color
                    1.0f // opacity
                },
                // stroke
                {
                    1.0f, // width
                    D2D1::ColorF{ 0xc2c2c2 }, // color
                    1.0f // opacity
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
                    D2D1::ColorF{ 0xf5f5f5 }, // color
                    1.0f // opacity
                },
                // stroke
                {
                    1.0f, // width
                    D2D1::ColorF{ 0xc2c2c2 }, // color
                    1.0f // opacity
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
                    D2D1::ColorF{ 0xebebeb }, // color
                    1.0f // opacity
                },
                // stroke
                {
                    1.0f, // width
                    D2D1::ColorF{ 0xc2c2c2 }, // color
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

                // foreground
                {
                    D2D1::ColorF{ 0xe5e5e5 }, // color
                    1.0f // opacity
                },
                // background
                {
                    D2D1::ColorF{ 0x262626 }, // color
                    1.0f // opacity
                },
                // stroke
                {
                    1.0f, // width
                    D2D1::ColorF{ 0x3d3d3d }, // color
                    1.0f // opacity
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
                    D2D1::ColorF{ 0x292929 }, // color
                    1.0f // opacity
                },
                // stroke
                {
                    1.0f, // width
                    D2D1::ColorF{ 0x3d3d3d }, // color
                    1.0f // opacity
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
                    D2D1::ColorF{ 0x1a1a1a }, // color
                    1.0f // opacity
                },
                // stroke
                {
                    1.0f, // width
                    D2D1::ColorF{ 0x3d3d3d }, // color
                    1.0f // opacity
                }
            };
        }
        UpdateAppearanceSetting(State::Idle);
    }
}