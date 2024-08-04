#pragma once

#include <Windows.h>
#include <thread>
#include "JoyMacroLogging.h"

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

    KeyboardHookThread();
    ~KeyboardHookThread();

    void InitHooking();
    void StopHooking();

    bool isPressed(int keyCode);
};



