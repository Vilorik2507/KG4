
#pragma once

#include <Windows.h>

class Timer
{
public:
    Timer();

    void Reset();
    void Tick();

    float GetDeltaTime() const noexcept;
    float GetTotalTime() const noexcept;

private:
    LARGE_INTEGER frequency_{};
    LARGE_INTEGER startTime_{};
    LARGE_INTEGER previousTime_{};

    float deltaTime_ = 0.0f;
    float totalTime_ = 0.0f;
};
