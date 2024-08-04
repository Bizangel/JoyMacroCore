#pragma once

#include <windows.h>
#include <Xinput.h>
#include <thread>
#include "VigemController.h"
#include "include/JoyMacroCore.h"

class OverriderPollThread
{
private:
    std::thread _runningThread;
    std::unique_ptr<bool> stopNotifier;
    VigemController* _outputController;
    IGamepadOverrider* _overriderController;

    int pollDelay;
    int indexToPoll = -1;

    void Poll();
    void PollLoop();
public:
    OverriderPollThread(int polld, IGamepadOverrider* overrider, VigemController* outputCont);

    void Initialize();
    void End();
};

