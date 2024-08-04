#include "OverrideHandler.h"

std::shared_ptr<OverrideHandler> OverrideHandler::getInstance()
{
    return _instance == nullptr ? std::make_shared<OverrideHandler>() : _instance;
}

int OverrideHandler::EnsureVigemInitialized()
{
    if (_vigemClient != nullptr)
        return 0; // already initialized

    _vigemClient = std::make_unique<VigemClient>();
    return _vigemClient->Initialize();
}

JoyMacroExitCode OverrideHandler::StartOverride(int overrideIndex, IGamepadOverrider* overrider)
{
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

JoyMacroExitCode OverrideHandler::StopOverride()
{
    // Only stops currently connected controller. Vigem subsystem will be cleanup'd by smart pointers.
    _poller->End();
    _poller = nullptr; // reset poller and trigger cleanup
    _vigemClient->DeleteAndUnplugController();

    // TODO: stop-hid hide

    return JoyMacroExitCode::SUCCESS;
}