#pragma once

// ========== Exposed Header File with Public Functions =======
#include <windows.h>

#pragma comment(lib,"setupapi.lib")
#pragma comment(lib,"XInput.lib")
#pragma comment(lib,"Xinput9_1_0.lib")

#include <memory>
#include <Xinput.h>

// ===== Exit Codes =======

enum JoyMacroExitCode {
	SUCCESS = 0,
	VIGEM_UNABLE_INITIALIZE = 1,
	VIGEM_UNABLE_PLUG_CONTROLLER = 2,
	ALREADY_INITIALIZED = 3,
};

// =============
// Publicly exposable interface to implement Gamepad Input Overriding
// =============
class IGamepadOverrider
{
public:
	virtual void OverrideInput(XINPUT_GAMEPAD& gamepadRef) = 0;
};

class VigemClient;
class OverriderPollThread;


#define POLLING_DELAY_MS 10
class JoyMacroOverrideClient
{
private:


	std::unique_ptr<VigemClient> _vigemClient = nullptr;
	std::unique_ptr<OverriderPollThread> _poller = nullptr;

	int EnsureVigemInitialized();
public:
	JoyMacroExitCode StartOverride(int overrideIndex, IGamepadOverrider* overrider);
	JoyMacroExitCode StopOverride();

	~JoyMacroOverrideClient();
};