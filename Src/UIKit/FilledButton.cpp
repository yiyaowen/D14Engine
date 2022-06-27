#include "Common/Precompile.h"

#include "UIKit/FilledButton.h"

namespace d14engine::uikit
{
    FilledButton::FilledButton(
        WstrParam text,
        const D2D1_RECT_F& rect,
        float roundRadius,
        ComPtrParam<ID2D1Bitmap1> icon,
        const D2D1_COLOR_F& idleColor,
        const D2D1_COLOR_F& hoverColor,
        const D2D1_COLOR_F& downColor)
        :
        Button(text, rect, roundRadius)
    {
        appearances[(size_t)State::Idle] =
        {
            idleColor, // background color
            1.0f, // background opacity
            icon, // bitmap
            1.0f, // bitmap opacity
            (D2D1::ColorF)D2D1::ColorF::Black, // foreground color
            1.0f, // foreground opacity
            { 0.8f, 0.8f, 0.8f, 1.0f }, // stroke color
            1.0f, // stroke opacity
            1.0f // stroke width
        };
        appearances[(size_t)State::Hover] =
        {
            hoverColor, // background color
            1.0f, // background opacity
            icon, // bitmap
            1.0f, // bitmap opacity
            (D2D1::ColorF)D2D1::ColorF::Black, // foreground color
            1.0f, // foreground opacity
            { 0.8f, 0.8f, 0.8f, 1.0f }, // stroke color
            1.0f, // stroke opacity
            1.0f // stroke width
        };
        appearances[(size_t)State::Down] =
        {
            downColor, // background color
            1.0f, // background opacity
            icon, // bitmap
            0.5f, // bitmap opacity
            (D2D1::ColorF)D2D1::ColorF::Black, // foreground color
            0.5f, // foreground opacity
            { 0.8f, 0.8f, 0.8f, 1.0f }, // stroke color
            1.0f, // stroke opacity
            1.0f // stroke width
        };
    }

    void FilledButton::OnChangeThemeHelper(WstrViewParam themeName)
    {
        // Call Panel's OnChangeThemeHelper instead of Button's since the appearance states
        // would be configed in this method (no need to use the states configed in Button).
        Panel::OnChangeThemeHelper(themeName);

        if (themeName == L"Light")
        {
            appearances[(size_t)State::Idle] =
            {
                { 0.98f, 0.98f, 0.98f, 1.0f }, // background color
                1.0f, // background opacity
                appearances[(size_t)State::Idle].bitmap, // bitmap
                1.0f, // bitmap opacity
                (D2D1::ColorF)D2D1::ColorF::Black, // foreground color
                1.0f, // foreground opacity
                { 0.8f, 0.8f, 0.8f, 1.0f }, // stroke color
                1.0f, // stroke opacity
                1.0f // stroke width
            };
            appearances[(size_t)State::Hover] =
            {
                { 0.95f, 0.95f, 0.95f, 1.0f }, // background color
                1.0f, // background opacity
                appearances[(size_t)State::Hover].bitmap, // bitmap
                1.0f, // bitmap opacity
                (D2D1::ColorF)D2D1::ColorF::Black, // foreground color
                1.0f, // foreground opacity
                { 0.8f, 0.8f, 0.8f, 1.0f }, // stroke color
                1.0f, // stroke opacity
                1.0f // stroke width
            };
            appearances[(size_t)State::Down] =
            {
                { 0.95f, 0.95f, 0.95f, 1.0f }, // background color
                1.0f, // background opacity
                appearances[(size_t)State::Down].bitmap, // bitmap
                0.5f, // bitmap opacity
                (D2D1::ColorF)D2D1::ColorF::Black, // foreground color
                0.5f, // foreground opacity
                { 0.8f, 0.8f, 0.8f, 1.0f }, // stroke color
                1.0f, // stroke opacity
                1.0f // stroke width
            };
        }
        else if (themeName == L"Dark")
        {
            appearances[(size_t)State::Idle] =
            {
                { 0.17f, 0.17f, 0.17f, 1.0f }, // background color
                1.0f, // background opacity
                appearances[(size_t)State::Idle].bitmap, // bitmap
                1.0f, // bitmap opacity
                { 0.9f, 0.9f, 0.9f, 1.0f }, // foreground color
                1.0f, // foreground opacity
                { 0.25f, 0.25f, 0.25f, 1.0f }, // stroke color
                1.0f, // stroke opacity
                1.0f // stroke width
            };
            appearances[(size_t)State::Hover] =
            {
                { 0.2f, 0.2f, 0.2f, 1.0f }, // background color
                1.0f, // background opacity
                appearances[(size_t)State::Hover].bitmap, // bitmap
                1.0f, // bitmap opacity
                { 0.9f, 0.9f, 0.9f, 1.0f }, // foreground color
                1.0f, // foreground opacity
                { 0.25f, 0.25f, 0.25f, 1.0f }, // stroke color
                1.0f, // stroke opacity
                1.0f // stroke width
            };
            appearances[(size_t)State::Down] =
            {
                { 0.15f, 0.15f, 0.15f, 1.0f }, // background color
                1.0f, // background opacity
                appearances[(size_t)State::Down].bitmap, // bitmap
                0.5f, // bitmap opacity
                { 0.9f, 0.9f, 0.9f, 1.0f }, // foreground color
                0.5f, // foreground opacity
                { 0.25f, 0.25f, 0.25f, 1.0f }, // stroke color
                1.0f, // stroke opacity
                1.0f // stroke width
            };
        }
        UpdateAppearanceSetting(State::Idle);
    }
}