#pragma once

// ========== Exposed Header File with Public Functions =======
#include <windows.h>

#pragma comment(lib,"setupapi.lib")
#pragma comment(lib,"XInput.lib")
#pragma comment(lib,"Xinput9_1_0.lib")

#include <memory>
#include <Xinput.h>
#include <vector>

// ===== Exit Codes =======

enum JoyMacroExitCode {
	SUCCESS = 0,
	VIGEM_UNABLE_INITIALIZE = 1,
	VIGEM_UNABLE_PLUG_CONTROLLER = 2,
	ALREADY_INITIALIZED = 3,
	HID_HIDE_MISSING = 4,
};

struct PaddleState {
	bool P1;
	bool P2;
	bool P3;
	bool P4;
};

// =============
// Publicly exposable interface to implement Gamepad Input Overriding
// =============
class IGamepadOverrider
{
public:
	virtual void OverrideInput(XINPUT_GAMEPAD& gamepadRef, const PaddleState& pState) = 0;
};

class VigemClient;
class OverriderPollThread;
class KeyboardHookThread;
class HidHideClient;


#define DEFAULT_POLLING_DELAY_MS 10
class JoyMacroOverrideClient
{
private:
	std::shared_ptr<VigemClient> _vigemClient;
	std::shared_ptr<OverriderPollThread> _poller;
	std::shared_ptr<KeyboardHookThread> _keyhookThread;
	std::shared_ptr<HidHideClient> _hidHideClient;
	bool _useHidHide = true;
	
	int _pollingDelayMs = DEFAULT_POLLING_DELAY_MS;

	int EnsureVigemInitialized();
public:
	void setPollingDelayMs(int delay);
	void disableHidHide();

	JoyMacroExitCode StartOverride(int overrideIndex, IGamepadOverrider* overrider);
	JoyMacroExitCode StartOverride(int overrideIndex, IGamepadOverrider* overrider, const std::vector<int>& paddleKeymapping);
	JoyMacroExitCode StopOverride();

	int getFirstActiveControllerIndex();

	JoyMacroOverrideClient();
	~JoyMacroOverrideClient();
};