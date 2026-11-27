#include "InputDevice.h"

void InputDevice::OnKeyDown(WPARAM key)
{
    if (key < KeyCount)
    {
        keys_[key] = true;
    }
}

void InputDevice::OnKeyUp(WPARAM key)
{
    if (key < KeyCount)
    {
        keys_[key] = false;
    }
}

bool InputDevice::IsKeyDown(int key) const
{
    if (key < 0 || key >= KeyCount)
        return false;

    return keys_[key];
}

bool InputDevice::IsKeyPressed(int key) const
{
    if (key < 0 || key >= KeyCount)
        return false;

    return keys_[key] && !previousKeys_[key];
}

bool InputDevice::IsKeyReleased(int key) const
{
    if (key < 0 || key >= KeyCount)
        return false;

    return !keys_[key] && previousKeys_[key];
}

void InputDevice::OnMouseMove(int x, int y)
{
    mousePosition_.x = x;
    mousePosition_.y = y;
}

void InputDevice::OnMouseWheel(short delta)
{
    mouseWheelDelta_ += delta;
}

void InputDevice::OnMouseButtonDown(int button)
{
    if (button >= 0 && button < 3)
    {
        mouseButtons_[button] = true;
    }
}

void InputDevice::OnMouseButtonUp(int button)
{
    if (button >= 0 && button < 3)
    {
        mouseButtons_[button] = false;
    }
}

POINT InputDevice::GetMousePosition() const noexcept
{
    return mousePosition_;
}

POINT InputDevice::GetMouseOffset() const noexcept
{
    return mouseOffset_;
}

short InputDevice::GetMouseWheelDelta() const noexcept
{
    return mouseWheelDelta_;
}

bool InputDevice::IsMouseButtonDown(int button) const
{
    if (button < 0 || button >= 3)
        return false;

    return mouseButtons_[button];
}

void InputDevice::Update()
{
    for (int i = 0; i < KeyCount; ++i)
    {
        previousKeys_[i] = keys_[i];
    }

    mouseOffset_.x =
        mousePosition_.x - previousMousePosition_.x;

    mouseOffset_.y =
        mousePosition_.y - previousMousePosition_.y;

    previousMousePosition_ = mousePosition_;

    mouseWheelDelta_ = 0;
}

void InputDevice::Reset()
{
    keys_.fill(false);
    previousKeys_.fill(false);

    mouseButtons_.fill(false);

    mousePosition_ = {};
    previousMousePosition_ = {};
    mouseOffset_ = {};

    mouseWheelDelta_ = 0;
}
