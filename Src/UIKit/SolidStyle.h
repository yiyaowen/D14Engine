#pragma once

#include "Common/Precompile.h"

namespace d14engine::uikit
{
    struct SolidStyle
    {
        SolidStyle(
            D2D1_COLOR_F backgroundColor = (D2D1::ColorF)D2D1::ColorF::Black,
            float backgroundOpacity = 1.0f);

        D2D1_COLOR_F backgroundColor = {};
        float backgroundOpacity = {};
    };
}