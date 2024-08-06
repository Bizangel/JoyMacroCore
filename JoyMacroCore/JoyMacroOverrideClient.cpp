#include "Include/JoyMacroCore.h"
#include <memory.h>
#include "VigemClient.h"
#include "OverriderPollThread.h"
#include "KeyboardHookThread.h"
#include "RegistryUtils.h"
#include "HidHideClient.h"

int JoyMacroOverrideClient::EnsureVigemInitialized()
{
    if (_vigemClient != nullptr)
        return 0; // already initialized

    _vigemClient = std::make_shared<VigemClient>();
    return _vigemClient->Initialize();
}

void JoyMacroOverrideClient::setPollingDelayMs(int delay)
{
    _pollingDelayMs = delay;
}

void JoyMacroOverrideClient::disableHidHide()
{
    _useHidHide = false;
}

JoyMacroExitCode JoyMacroOverrideClient::StartOverride(int overrideIndex, IGamepadOverrider* overrider)
{
    return StartOverride(overrideIndex, overrider, {});
}

JoyMacroExitCode JoyMacroOverrideClient::StartOverride(int overrideIndex, IGamepadOverrider* overrider, const std::vector<int>& paddleKeymapping)
{
    LOG_DEBUG("Start Override Called!");
    if (_poller != nullptr)
        return JoyMacroExitCode::ALREADY_INITIALIZED;

    // Ensure initialized, if unable to initialize then stop execution. Nothing we can do about that.
    if (!EnsureVigemInitialized()) 
        return JoyMacroExitCode::VIGEM_UNABLE_INITIALIZE;

    if (_useHidHide) {
        if (!_hidHideClient->InitAndEnsureHidHideInstalled())
            return JoyMacroExitCode::HID_HIDE_MISSING;

        // Enable HidHide (must be done BEFORE virtual controller is initialized)
        _hidHideClient->EnableGamepadHiding();
    }

    if (!_vigemClient->CreateAndPlugController(overrideIndex)) {
        _hidHideClient->DisableGamepadHiding();
        return JoyMacroExitCode::VIGEM_UNABLE_PLUG_CONTROLLER;
    }
    
    _keyhookThread = std::make_shared<KeyboardHookThread>(paddleKeymapping);
    _keyhookThread->InitHooking();

    _poller = std::make_shared<OverriderPollThread>(
        _pollingDelayMs, overrider, _vigemClient->getControllerRef(), _keyhookThread->getPaddleStateRef()
    );
    _poller->Initialize(); 

    return JoyMacroExitCode::SUCCESS;
}

JoyMacroExitCode JoyMacroOverrideClient::StopOverride()
{
    LOG_DEBUG("StopOverride Called!");
    // Only stops currently connected controller. Vigem subsystem will be cleanup'd by smart pointers.
    if (_poller != nullptr) {
        _poller->End();
        _poller = nullptr; // reset poller and trigger cleanup
    }

    if (_keyhookThread != nullptr) {
        _keyhookThread->StopHooking();
        _keyhookThread = nullptr; // reset thread and trigger cleanup
    }

    if (_vigemClient != nullptr) {
        _vigemClient->DeleteAndUnplugController();
    }

    if (_useHidHide)
        _hidHideClient->DisableGamepadHiding();

    return JoyMacroExitCode::SUCCESS;
}

int JoyMacroOverrideClient::getFirstActiveControllerIndex()
{
    DWORD stateResult;
    for (int i = 0; i < 4; i++) {
        XINPUT_STATE controllerState;
        ZeroMemory(&controllerState, sizeof(XINPUT_STATE));
        stateResult = XInputGetState(i, &controllerState);

        if (stateResult == ERROR_SUCCESS)
            return i;
    }

    return -1;
}


JoyMacroOverrideClient::JoyMacroOverrideClient()
{
    _hidHideClient = std::make_shared<HidHideClient>();
}

JoyMacroOverrideClient::~JoyMacroOverrideClient() = default;