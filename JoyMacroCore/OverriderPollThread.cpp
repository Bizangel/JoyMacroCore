#include "OverriderPollThread.h"

OverriderPollThread::OverriderPollThread(int polld, IGamepadOverrider* overrider, VigemController* outputCont, std::vector<bool>* paddleStateRef)
{
    _overriderController = overrider;
    _outputController = outputCont;
    _paddleStateRef = paddleStateRef;
    stopNotifier = std::make_unique<bool>(false);
    pollDelay = polld;
}

OverriderPollThread::~OverriderPollThread()
{
    LOG_DEBUG("OverriderPollThread clean up called");
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

PaddleState OverriderPollThread::getCurrentPaddleState() {
    PaddleState state;

    // Ensure the vector has at least 4 elements
    if (_paddleStateRef->size() >= 4) {
        state.P1 = (*_paddleStateRef)[0];
        state.P2 = (*_paddleStateRef)[0];
        state.P3 = (*_paddleStateRef)[0];
        state.P4 = (*_paddleStateRef)[0];
    }
    else {
        LOG_ERROR("INVALID STATE FOUND: PADDLE STATE VECTOR WITH LESS THAN 4 ELEMENTS");
    }

    return state;
}

void OverriderPollThread::Poll() {
    DWORD stateResult;
    XINPUT_STATE controllerState;
    ZeroMemory(&controllerState, sizeof(XINPUT_STATE)); // think this only needs to be done once
    stateResult = XInputGetState(indexToPoll, &controllerState);
    if (stateResult == ERROR_SUCCESS) {
        if (!_outputController->isPluggedIn())
            _outputController->PlugIn();

        _overriderController->OverrideInput(controllerState.Gamepad, getCurrentPaddleState());
        _outputController->UpdateState(controllerState.Gamepad);
    }
    else {
        if (_outputController->isPluggedIn())
            _outputController->UnPlug();
    }
}