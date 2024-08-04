#include "KeyboardHookThread.h"

KeyboardHookThread::KeyboardHookThread()
{
    _winHookInstalled = false;
    //running = std::make_unique<bool>(false);
}

KeyboardHookThread::~KeyboardHookThread()
{
    LOG_DEBUG("KeyboardHookThread clean up called");
}

HHOOK KeyboardHookThread::keyboardHook;
DWORD KeyboardHookThread::childThreadId;

LRESULT __stdcall KEYHookCallback(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode >= 0 && (wParam == WM_KEYDOWN || wParam == WM_KEYUP))
    {
        KBDLLHOOKSTRUCT* kbdStruct = (KBDLLHOOKSTRUCT*)lParam;
        if (kbdStruct->vkCode == 0x4C) // Virtual-Key code for 'L'
        {
            if (wParam == WM_KEYDOWN)
            {
                std::cout << "L key down!" << std::endl;
            }
            else if (wParam == WM_KEYUP)
            {
                std::cout << "L key up!" << std::endl;
            }
            return 1; // Block the key
        }
    }

    return CallNextHookEx(KeyboardHookThread::keyboardHook, nCode, wParam, lParam);
}

void KeyboardHookThread::InitHooking()
{
    LOG_DEBUG("Initializing Keyboard Hook Thread!");
    _winHookInstalled = false;
    //*running = true;
    _runningThread = std::thread(&KeyboardHookThread::ThreadLoop, this);
}

void KeyboardHookThread::StopHooking()
{
    LOG_DEBUG("Stopping Keyboard Hook Thread!");
    PostThreadMessage(childThreadId, WM_QUIT, 0, 0);
    _runningThread.join();
}

bool KeyboardHookThread::isPressed(int keyCode)
{
    return false;
}

void KeyboardHookThread::ThreadLoop()
{
    childThreadId = GetCurrentThreadId();
    SetKeyboardHook();
    MSG msg;

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    FreeKeyboardHook();
}

bool KeyboardHookThread::SetKeyboardHook()
{
    if (!(keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KEYHookCallback, NULL, 0)))
    {
        return false;
    }
    LOG_DEBUG("Win Keyboard hook installed");
    _winHookInstalled = true;
    return true;
}

void KeyboardHookThread::FreeKeyboardHook()
{
    if (_winHookInstalled) {
        LOG_DEBUG("Freeing Win Keyboard Hook");
        UnhookWindowsHookEx(keyboardHook);
    }
}




