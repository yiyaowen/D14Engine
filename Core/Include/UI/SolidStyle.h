#pragma once

#include "Precompile.h"

namespace d14engine::ui
{
    struct SolidStyle
    {
        SolidStyle(
            D2D1_COLOR_F solidColor = (D2D1::ColorF)D2D1::ColorF::Black,
            float solidColorOpaque = 1.0f);

        D2D1_COLOR_F solidColor;

        float solidColorOpaque;
    };
}