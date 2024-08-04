#pragma once

#include <JoyMacroCore.h>
#include <Xinput.h>

class ExampleOverrider :
    public IGamepadOverrider
{
    void OverrideInput(XINPUT_GAMEPAD& gamepadRef, const PaddleState& state) override;
};

