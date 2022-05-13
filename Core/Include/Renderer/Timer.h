#pragma once

#include "Precompile.h"

namespace d14engine::renderer
{
    struct Timer
    {
        Timer();

        __int64 tickCntsPerSecond = 0;
        double secsPerTickCount = 0;

        __int64 baseTickCount = 0;
        __int64 currTickCount = 0;

        double elapsedSecsAtLastPause = 0;
        double elapsedSecsSinceResume = 0;

        double ElapsedSecs();

        double deltaSecs = 0;

        void Start();

        void Tick();

        void Stop();

        void Resume();

    private:
        bool mIsPause = false;
    };
}