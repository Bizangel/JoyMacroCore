#include "ExampleOverrider.h"


void ExampleOverrider::OverrideInput(XINPUT_GAMEPAD& gamepadRef)
{
	// This example overrides swaps A inputs with B inputs
	// 
	// Save unaltered input
	const XINPUT_GAMEPAD input(gamepadRef);

	gamepadRef.wButtons &= ~XINPUT_GAMEPAD_A; // unset A
	gamepadRef.wButtons &= ~XINPUT_GAMEPAD_B; // unset B

	if ((input.wButtons & XINPUT_GAMEPAD_A) != 0) // if A set
		gamepadRef.wButtons |= XINPUT_GAMEPAD_B; // set B

	if ((input.wButtons & XINPUT_GAMEPAD_B) != 0) // if B set
		gamepadRef.wButtons |= XINPUT_GAMEPAD_A; // set A
}
