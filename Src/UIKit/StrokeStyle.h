#pragma once

#include "Common/Precompile.h"

namespace d14engine::uikit
{
    struct StrokeStyle
    {
        StrokeStyle(
            float width = 1.0f,
            D2D1_COLOR_F color = D2D1::ColorF{ 0x000000 },
            float opacity = 1.0f);

        float width = {};

        D2D1_COLOR_F color = {};
        float opacity = {};
    };
}