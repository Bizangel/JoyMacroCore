#include "Include/JoyMacroCore.h"
#include <memory.h>
#include "VigemClient.h"
#include "OverriderPollThread.h"

int JoyMacroOverrideClient::EnsureVigemInitialized()
{
    if (_vigemClient != nullptr)
        return 0; // already initialized

    _vigemClient = std::make_unique<VigemClient>();
    return _vigemClient->Initialize();
}

JoyMacroExitCode JoyMacroOverrideClient::StartOverride(int overrideIndex, IGamepadOverrider* overrider)
{
    LOG_DEBUG("Start Override Called!");
    if (_poller != nullptr)
        return JoyMacroExitCode::ALREADY_INITIALIZED;

    // Ensure initialized, if unable to initialize then stop execution. Nothing we can do about that.
    if (!EnsureVigemInitialized())
        return JoyMacroExitCode::VIGEM_UNABLE_INITIALIZE;

    if (!_vigemClient->CreateAndPlugController(overrideIndex))
        return JoyMacroExitCode::VIGEM_UNABLE_PLUG_CONTROLLER;
    
    // TODO: start HID-HIDE
    _poller = std::make_unique<OverriderPollThread>(POLLING_DELAY_MS, overrider, _vigemClient->getControllerRef());
    _poller->Initialize(); 

    return JoyMacroExitCode::SUCCESS;
}

JoyMacroExitCode JoyMacroOverrideClient::StopOverride()
{
    LOG_DEBUG("StopOverride Called!");
    // Only stops currently connected controller. Vigem subsystem will be cleanup'd by smart pointers.
    if (_poller != nullptr) {
        LOG_DEBUG("Stopping poller thread from outside...");
        _poller->End();
        _poller = nullptr; // reset poller and trigger cleanup
    }
    else {
        LOG_DEBUG("Stopping poller with NULL!");
    }

    if (_vigemClient != nullptr) {
        _vigemClient->DeleteAndUnplugController();
    }

    // TODO: stop-hid hide
    return JoyMacroExitCode::SUCCESS;
}

JoyMacroOverrideClient::~JoyMacroOverrideClient() = default;