#include "Common/Precompile.h"

#include "Renderer/Timer.h"

namespace d14engine::renderer
{
    Timer::Timer()
    {
        QueryPerformanceFrequency((LARGE_INTEGER*)&tickCntsPerSecond);
        secsPerTickCount = 1.0 / tickCntsPerSecond;
    }

    double Timer::ElapsedSecs()
    {
        return elapsedSecsAtLastPause + elapsedSecsSinceResume;
    }

    void Timer::Start()
    {
        m_isPause = false;
        QueryPerformanceCounter((LARGE_INTEGER*)&baseTickCount);
        currTickCount = baseTickCount;
        // Reset all performance counts.
        elapsedSecsAtLastPause = elapsedSecsSinceResume = deltaSecs = 0;
    }

    void Timer::Tick()
    {
        if (m_isPause) return; // Stike when stopped or paused.
        QueryPerformanceCounter((LARGE_INTEGER*)&currTickCount);
        double currElapsedSecs = (currTickCount - baseTickCount) * secsPerTickCount;
        deltaSecs = currElapsedSecs - elapsedSecsSinceResume;
        elapsedSecsSinceResume = currElapsedSecs;
    }

    void Timer::Stop()
    {
        m_isPause = true;
        elapsedSecsAtLastPause = ElapsedSecs();
        elapsedSecsSinceResume = deltaSecs = 0;
    }

    void Timer::Resume()
    {
        if (m_isPause)
        {
            m_isPause = false;
            QueryPerformanceCounter((LARGE_INTEGER*)&baseTickCount);
            currTickCount = baseTickCount;
        }
    }
}