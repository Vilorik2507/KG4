
#include "Timer.h"

Timer::Timer()
{
    QueryPerformanceFrequency(&frequency_);
    Reset();
}

void Timer::Reset()
{
    LARGE_INTEGER currentTime{};
    QueryPerformanceCounter(&currentTime);

    startTime_ = currentTime;
    previousTime_ = currentTime;

    deltaTime_ = 0.0f;
    totalTime_ = 0.0f;
}

void Timer::Tick()
{
    LARGE_INTEGER currentTime{};
    QueryPerformanceCounter(&currentTime);

    deltaTime_ = static_cast<float>(
        static_cast<double>(currentTime.QuadPart - previousTime_.QuadPart) /
        static_cast<double>(frequency_.QuadPart)
        );

    previousTime_ = currentTime;

    totalTime_ = static_cast<float>(
        static_cast<double>(currentTime.QuadPart - startTime_.QuadPart) /
        static_cast<double>(frequency_.QuadPart)
        );

    if (deltaTime_ < 0.0f)
        deltaTime_ = 0.0f;
}

float Timer::GetDeltaTime() const noexcept
{
    return deltaTime_;
}

float Timer::GetTotalTime() const noexcept
{
    return totalTime_;
}
