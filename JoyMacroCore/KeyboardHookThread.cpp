#include "KeyboardHookThread.h"

KeyboardHookThread::KeyboardHookThread(const std::vector<int>& paddleKeycodes)
{
    paddleKeymapping = paddleKeycodes; // make a copy
    paddlePressState = std::vector<bool>(PADDLE_COUNT, false);
    _winHookInstalled = false;
}

KeyboardHookThread::~KeyboardHookThread()
{
    LOG_DEBUG("KeyboardHookThread clean up called");
}

HHOOK KeyboardHookThread::keyboardHook;
DWORD KeyboardHookThread::childThreadId;
std::vector<int> KeyboardHookThread::paddleKeymapping;
std::vector<bool> KeyboardHookThread::paddlePressState;

LRESULT __stdcall KEYHookCallback(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode >= 0 && (wParam == WM_KEYDOWN || wParam == WM_KEYUP))
    {
        KBDLLHOOKSTRUCT* kbdStruct = (KBDLLHOOKSTRUCT*)lParam;
        for (int i = 0; i < KeyboardHookThread::paddleKeymapping.size(); i++) {
            DWORD keyCode = KeyboardHookThread::paddleKeymapping[i];

            if (kbdStruct->vkCode == keyCode) // Virtual-Key code for 'L'
            {
                if (wParam == WM_KEYDOWN)
                {
                    KeyboardHookThread::paddlePressState[i] = true;
                }
                else if (wParam == WM_KEYUP)
                {
                    KeyboardHookThread::paddlePressState[i] = false;
                }
                return 1; // Block the key
            }
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

std::vector<bool>* KeyboardHookThread::getPaddleStateRef()
{
    return &KeyboardHookThread::paddlePressState;
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




