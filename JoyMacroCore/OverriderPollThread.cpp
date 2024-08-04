#include "OverriderPollThread.h"

OverriderPollThread::OverriderPollThread(int polld, IGamepadOverrider* overrider, VigemController* outputCont)
{
    _overriderController = overrider;
    _outputController = outputCont;
    stopNotifier = std::make_unique<bool>(false);
    pollDelay = polld;
}

void OverriderPollThread::Initialize()
{
    LOG_DEBUG("Initializing polling thread!");
    indexToPoll = _outputController->controllerOverrideIndex;
    _runningThread = std::thread(&OverriderPollThread::PollLoop, this);
}

void OverriderPollThread::End()
{
    LOG_DEBUG("Stopping Polling thread!");
    *stopNotifier = true;
    _runningThread.join();
}

void OverriderPollThread::PollLoop() {
    while (!*stopNotifier) {
        Poll();
        Sleep(pollDelay);
    }
}

void OverriderPollThread::Poll() {
    DWORD stateResult;
    XINPUT_STATE controllerState;
    ZeroMemory(&controllerState, sizeof(XINPUT_STATE)); // think this only needs to be done once
    stateResult = XInputGetState(indexToPoll, &controllerState);
    if (stateResult == ERROR_SUCCESS) {
        //std::cout << "Controller connected!" << std::endl;
        _overriderController->OverrideInput(controllerState.Gamepad);
        _outputController->UpdateState(controllerState.Gamepad);
    }
}