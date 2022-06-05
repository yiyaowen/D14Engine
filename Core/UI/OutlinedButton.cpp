#include "Precompile.h"

#include "UI/OutlinedButton.h"

#include "UI/Application.h"

namespace d14engine::ui
{
    OutlinedButton::OutlinedButton(
        WstrParam text,
        const D2D1_RECT_F& rect,
        float roundRadius,
        ComPtrParam<ID2D1Bitmap1> icon,
        const D2D1_COLOR_F& strokeColor,
        float strokeWidth)
        :
        Button(text, rect, roundRadius)
    {
        appearances[(size_t)State::Idle] =
        {
            (D2D1::ColorF)D2D1::ColorF::Gray, // background color
            0.0f, // background opacity
            icon, // bitmap
            1.0f, // bitmap opacity
            strokeColor, // foreground color
            1.0f, // foreground opacity
            strokeColor, // stroke color
            1.0f, // stroke opacity
            strokeWidth // stroke width
        };
        appearances[(size_t)State::Hover] =
        {
            (D2D1::ColorF)D2D1::ColorF::Gray, // background color
            0.1f, // background opacity
            icon, // bitmap
            1.0f, // bitmap opacity
            strokeColor, // foreground color
            1.0f, // foreground opacity
            strokeColor, // stroke color
            1.0f, // stroke opacity
            strokeWidth // stroke width
        };
        appearances[(size_t)State::Down] =
        {
            (D2D1::ColorF)D2D1::ColorF::Gray, // background color
            0.1f, // background opacity
            icon, // bitmap
            0.5f, // bitmap opacity
            strokeColor, // foreground color
            0.5f, // foreground opacity
            strokeColor, // stroke color
            0.5f, // stroke opacity
            strokeWidth // stroke width
        };
    }

    void OutlinedButton::OnChangeThemeHelper(WstrViewParam themeName)
    {
        // Call Panel's OnChangeThemeHelper instead of Button's since the appearance states
        // would be configed in this method (no need to use the states configed in Button).
        Panel::OnChangeThemeHelper(themeName);

        if (themeName == L"Light")
        {
            appearances[(size_t)State::Idle] =
            {
                (D2D1::ColorF)D2D1::ColorF::Gray, // background color
                0.0f, // background opacity
                appearances[(size_t)State::Idle].bitmap, // bitmap
                1.0f, // bitmap opacity
                { 0.78f, 0.12f, 0.2f, 1.0f }, // foreground color
                1.0f, // foreground opacity
                { 0.78f, 0.12f, 0.2f, 1.0f }, // stroke color
                1.0f, // stroke opacity
                1.5f // stroke width
            };
            appearances[(size_t)State::Hover] =
            {
                (D2D1::ColorF)D2D1::ColorF::Gray, // background color
                0.1f, // background opacity
                appearances[(size_t)State::Hover].bitmap, // bitmap
                1.0f, // bitmap opacity
                { 0.78f, 0.12f, 0.2f, 1.0f }, // foreground color
                1.0f, // foreground opacity
                { 0.78f, 0.12f, 0.2f, 1.0f }, // stroke color
                1.0f, // stroke opacity
                1.5f // stroke width
            };
            appearances[(size_t)State::Down] =
            {
                (D2D1::ColorF)D2D1::ColorF::Gray, // background color
                0.1f, // background opacity
                appearances[(size_t)State::Down].bitmap, // bitmap
                0.5f, // bitmap opacity
                { 0.78f, 0.12f, 0.2f, 1.0f }, // foreground color
                0.5f, // foreground opacity
                { 0.78f, 0.12f, 0.2f, 1.0f }, // stroke color
                0.5f, // stroke opacity
                1.5f // stroke width
            };
        }
        else if (themeName == L"Dark")
        {
            appearances[(size_t)State::Idle] =
            {
                (D2D1::ColorF)D2D1::ColorF::Gray, // background color
                0.0f, // background opacity
                appearances[(size_t)State::Idle].bitmap, // bitmap
                1.0f, // bitmap opacity
                { 0.2f, 0.62f, 0.39f, 1.0f }, // foreground color
                1.0f, // foreground opacity
                { 0.2f, 0.62f, 0.39f, 1.0f }, // stroke color
                1.0f, // stroke opacity
                1.5f // stroke width
            };
            appearances[(size_t)State::Hover] =
            {
                (D2D1::ColorF)D2D1::ColorF::Gray, // background color
                0.1f, // background opacity
                appearances[(size_t)State::Hover].bitmap, // bitmap
                1.0f, // bitmap opacity
                { 0.2f, 0.62f, 0.39f, 1.0f }, // foreground color
                1.0f, // foreground opacity
                { 0.2f, 0.62f, 0.39f, 1.0f }, // stroke color
                1.0f, // stroke opacity
                1.5f // stroke width
            };
            appearances[(size_t)State::Down] =
            {
                (D2D1::ColorF)D2D1::ColorF::Gray, // background color
                0.1f, // background opacity
                appearances[(size_t)State::Down].bitmap, // bitmap
                0.5f, // bitmap opacity
                { 0.2f, 0.62f, 0.39f, 1.0f }, // foreground color
                0.5f, // foreground opacity
                { 0.2f, 0.62f, 0.39f, 1.0f }, // stroke color
                0.5f, // stroke opacity
                1.5f // stroke width
            };
        }
        UpdateAppearanceSetting(State::Idle);
    }
}