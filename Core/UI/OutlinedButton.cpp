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
            (D2D1::ColorF)D2D1::ColorF::Gray, // solid color
            0.0f, // solid color opaque
            icon, // bitmap
            1.0f, // bitmap opaque
            strokeColor, // text color
            1.0f, // text color opaque
            strokeColor, // stroke color
            1.0f, // stroke color opaque
            strokeWidth // stroke width
        };
        appearances[(size_t)State::Hover] =
        {
            (D2D1::ColorF)D2D1::ColorF::Gray, // solid color
            0.1f, // solid color opaque
            icon, // bitmap
            1.0f, // bitmap opaque
            strokeColor, // text color
            1.0f, // text color opaque
            strokeColor, // stroke color
            1.0f, // stroke color opaque
            strokeWidth // stroke width
        };
        appearances[(size_t)State::Down] =
        {
            (D2D1::ColorF)D2D1::ColorF::Gray, // solid color
            0.1f, // solid color opaque
            icon, // bitmap
            0.5f, // bitmap opaque
            strokeColor, // text color
            0.5f, // text color opaque
            strokeColor, // stroke color
            0.5f, // stroke color opaque
            strokeWidth // stroke width
        };
    }
}