#pragma once

#include <Windows.h>
#include <thread>
#include <vector>
#include "JoyMacroLogging.h"

#define PADDLE_COUNT 4

class KeyboardHookThread
{
private:
    std::thread _runningThread;
    //std::unique_ptr<bool> running;
    bool _winHookInstalled;

    void ThreadLoop();

    bool SetKeyboardHook();
    void FreeKeyboardHook();
public:
    static HHOOK keyboardHook;
    static DWORD childThreadId;
    static std::vector<int> paddleKeymapping;
    static std::vector<bool> paddlePressState;

    KeyboardHookThread(const std::vector<int>& paddleKeycodes);
    ~KeyboardHookThread();

    void InitHooking();
    void StopHooking();

    std::vector<bool>* getPaddleStateRef();
};



