#include "VigemController.h"

int VigemController::toSendVibration = 0; // 0 by default, will get updated on runtime 

VigemController::VigemController(VigemClient* parent, PVIGEM_TARGET cont, int overrideIndex)
{
    _pluggedIn = true;
    _parentClient = parent;
    _controlledPad = cont;
    controllerOverrideIndex = overrideIndex;
    RegisterVibrationCallback();
}

VigemController::~VigemController()
{
    LOG_DEBUG("De-initializing vigem controller");
    if (_pluggedIn)
        vigem_target_remove(_parentClient->client, _controlledPad);

    vigem_target_free(_controlledPad);
}

void VigemController::UpdateState(XINPUT_GAMEPAD& state)
{
    vigem_target_x360_update(_parentClient->client, _controlledPad, *reinterpret_cast<XUSB_REPORT*>(&state));
}

bool VigemController::PlugIn()
{
    LOG_DEBUG("Plugging in virtual controller");
    if (_pluggedIn) // already plugged in
        return false; 

    const auto pir = vigem_target_add(_parentClient->client, _controlledPad);
    if (!VIGEM_SUCCESS(pir))
        return false;

    _pluggedIn = true; // success
    return true;
}

bool VigemController::UnPlug()
{
    LOG_DEBUG("UnPlugging virtual controller");
    if (!_pluggedIn) // already disconnected
        return false; 

    vigem_target_remove(_parentClient->client, _controlledPad);
    _pluggedIn = false; 
    return true;
}

bool VigemController::isPluggedIn() const
{
    return _pluggedIn;
}

VOID CALLBACK onVibration(
    PVIGEM_CLIENT Client,
    PVIGEM_TARGET Target,
    UCHAR LargeMotor,
    UCHAR SmallMotor,
    UCHAR LedNumber,
    LPVOID UserData
)
{
    XINPUT_VIBRATION vibration;
    ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));
    vibration.wLeftMotorSpeed = 256 * SmallMotor; // use any value between 0-65535 here
    vibration.wRightMotorSpeed = 256 * LargeMotor; // use any value between 0-65535 here
    XInputSetState(VigemController::toSendVibration, &vibration);
}

void VigemController::RegisterVibrationCallback()
{
    toSendVibration = controllerOverrideIndex; // the controller to send the vibration. (physical)
    const auto retval = vigem_target_x360_register_notification(_parentClient->client, _controlledPad, &onVibration, nullptr);

    if (!VIGEM_SUCCESS(retval))
    {
        std::ostringstream oss; oss << "WARN: Registering for vibration failed with error code: 0x" << std::hex << retval << std::endl << "VIBRATION WILL NOT WORK!" << std::endl;
        LOG_WARN(oss.str());
    }
}

