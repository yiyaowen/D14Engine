#pragma once

#include "Precompile.h"

namespace d14engine::renderer
{
    struct Timer
    {
        Timer()
        {
            QueryPerformanceFrequency((LARGE_INTEGER*)&tickCntsPerSecond);
            secsPerTickCount = 1.0 / tickCntsPerSecond;
        }

        __int64 tickCntsPerSecond = 0;
        double secsPerTickCount = 0;

        __int64 baseTickCount = 0;
        __int64 currTickCount = 0;

        double elapsedSecsAtLastPause = 0;
        double elapsedSecsSinceResume = 0;

        double ElapsedSecs() { return elapsedSecsAtLastPause + elapsedSecsSinceResume; }

        double deltaSecs = 0;

        void Start()
        {
            QueryPerformanceCounter((LARGE_INTEGER*)&baseTickCount);
            currTickCount = baseTickCount;
            // Reset all performance counts.
            elapsedSecsAtLastPause = elapsedSecsSinceResume = deltaSecs = 0;
        }

        void Tick()
        {
            if (mIsPause) return; // Stike when stopped or paused.
            QueryPerformanceCounter((LARGE_INTEGER*)&currTickCount);
            double currElapsedSecs = (currTickCount - baseTickCount) * secsPerTickCount;
            deltaSecs = currElapsedSecs - elapsedSecsSinceResume;
            elapsedSecsSinceResume = currElapsedSecs;
        }

        void Stop()
        {
            mIsPause = true;
            elapsedSecsAtLastPause = ElapsedSecs();
            elapsedSecsSinceResume = deltaSecs = 0;
        }

        void Resume()
        {
            mIsPause = false;
        }

    private:
        bool mIsPause = false;
    };
}