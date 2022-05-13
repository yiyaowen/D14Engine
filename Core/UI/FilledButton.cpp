#include "Precompile.h"

#include "UI/FilledButton.h"

namespace d14engine::ui
{
    FilledButton::FilledButton(
        WstrParam text,
        const D2D1_RECT_F& rect,
        float roundRadius,
        ComPtrParam<ID2D1Bitmap1> icon,
        const D2D1_COLOR_F& normalColor,
        const D2D1_COLOR_F& activeColor)
        :
        Button(text, rect, roundRadius)
    {
        appearances[(size_t)State::Idle] =
        {
            normalColor, // solid color
            1.0f, // solid color opaque
            icon, // bitmap
            1.0f, // bitmap opaque
            (D2D1::ColorF)D2D1::ColorF::Black, // text color
            1.0f, // text color opaque
            (D2D1::ColorF)D2D1::ColorF::Black, // stroke color
            0.0f, // stroke color opaque
            0.0f // stroke width
        };
        appearances[(size_t)State::Hover] =
        {
            activeColor, // solid color
            1.0f, // solid color opaque
            icon, // bitmap
            1.0f, // bitmap opaque
            (D2D1::ColorF)D2D1::ColorF::Black, // text color
            1.0f, // text color opaque
            (D2D1::ColorF)D2D1::ColorF::Black, // stroke color
            0.0f, // stroke color opaque
            0.0f // stroke width
        };
        appearances[(size_t)State::Down] =
        {
            activeColor, // solid color
            1.0f, // solid color opaque
            icon, // bitmap
            0.5f, // bitmap opaque
            (D2D1::ColorF)D2D1::ColorF::Black, // text color
            0.5f, // text color opaque
            (D2D1::ColorF)D2D1::ColorF::Black, // stroke color
            0.0f, // stroke color opaque
            0.0f // stroke width
        };
    }
}