#pragma once

#include "Common/Precompile.h"

namespace d14engine::uikit
{
    struct AnimationUtils
    {
        // Return the expected displacement in the next frame.
        static float AccelUniformDecelMotion(
            float currDisplacement,
            float lastFrameDeltaSecs,
            float totalDistance,
            float variableMotionSecs,
            float uniformMotionSecs);
    };
    // Introduce this alias to avoid too long prefix.
    using Animu = AnimationUtils;
}