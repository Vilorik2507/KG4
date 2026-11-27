#pragma once

#include <Windows.h>
#include <array>

class InputDevice
{
public:
    static constexpr int KeyCount = 256;

    InputDevice() = default;
    ~InputDevice() = default;

    //клавиатура
    void OnKeyDown(WPARAM key);
    void OnKeyUp(WPARAM key);

    bool IsKeyDown(int key) const;
    bool IsKeyPressed(int key) const;
    bool IsKeyReleased(int key) const;

    //мышь
    void OnMouseMove(int x, int y);
    void OnMouseWheel(short delta);

    void OnMouseButtonDown(int button);
    void OnMouseButtonUp(int button);

    POINT GetMousePosition() const noexcept;
    POINT GetMouseOffset() const noexcept;

    short GetMouseWheelDelta() const noexcept;

    bool IsMouseButtonDown(int button) const;

    //обновления
    void Update();

    void Reset();

private:
    std::array<bool, KeyCount> keys_{};
    std::array<bool, KeyCount> previousKeys_{};

    std::array<bool, 3> mouseButtons_{};

    POINT mousePosition_{};
    POINT previousMousePosition_{};
    POINT mouseOffset_{};

    short mouseWheelDelta_ = 0;
};
