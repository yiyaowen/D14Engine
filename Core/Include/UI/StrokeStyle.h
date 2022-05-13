#pragma once

#include "Precompile.h"

namespace d14engine::ui
{
    struct StrokeStyle
    {
        StrokeStyle(
            float strokeWidth = 1.0f,
            D2D1_COLOR_F strokeColor = (D2D1::ColorF)D2D1::ColorF::Black,
            float strokeColorOpaque = 1.0f);

        float strokeWidth;

        D2D1_COLOR_F strokeColor;

        float strokeColorOpaque;
    };
}