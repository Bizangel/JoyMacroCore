#include "HidHideClient.h"

#include "json.hpp"

using json = nlohmann::json;

int HidHideClient::ExecuteHidHideCLI(const std::string parameters, std::string& stdOut)
{
    return Popen("\"" + _hidCLIPath + "\" " + parameters, stdOut);
}

std::string HidHideClient::getCurrentlyRunningExecutablePath()
{
    wchar_t path[MAX_PATH] = { 0 };
    GetModuleFileNameW(NULL, path, MAX_PATH);
    return WStringUtils::ConvertWideToUtf8(std::wstring(path));
}

bool HidHideClient::InitAndEnsureHidHideInstalled()
{
    std::string hidHideFoundRoot;
    for (HKEY hkeyRoot : {HKEY_CLASSES_ROOT, HKEY_LOCAL_MACHINE, HKEY_CURRENT_USER}) {
        hidHideFoundRoot = RegistryUtils::ReadRegistryString(hkeyRoot, L"SOFTWARE\\Nefarius Software Solutions e.U.\\HidHide", L"Path");
        if (!hidHideFoundRoot.empty())
            break;
    }

    if (hidHideFoundRoot.empty()) {
        LOG_ERROR("Hid Hide installation not found!");
        return false;
    }
    
    // check for CLI path, and check that it exists.
    std::string HidHideCLIPath = hidHideFoundRoot + "x64" + "\\" + "HidHideCLI.exe";
    if (!std::filesystem::exists(HidHideCLIPath)) {
        LOG_ERROR("Hid Hide CLI not found!");
        return false; // unable to find executable
    }

    LOG_DEBUG("Hid Hide CLI found at " + HidHideCLIPath);
    _hidCLIPath = HidHideCLIPath;
    _currentExePath = getCurrentlyRunningExecutablePath();
    return true; 
}

int HidHideClient::Popen(const std::string& command, std::string& stdOut) {
    std::array<char, 128> buffer;
    std::string result;
    std::string fullCommand = "cmd /c ^\"" + command + "\"";

    LOG_DEBUG("Executing command: " + fullCommand);

    FILE* pipe = _popen(fullCommand.c_str(), "r");
    if (!pipe)
    {
        LOG_ERROR("Couldn't start command: " + fullCommand);
        return 0;
    }
    while (fgets(buffer.data(), 128, pipe) != NULL) {
        result += buffer.data();
    }
    auto returnCode = _pclose(pipe);
    stdOut = result;

    return returnCode;
}

bool HidHideClient::EnableGamepadHiding()
{
    LOG_DEBUG("Enabling HidHide Gamepad Hiding");

    // ====== GATHER INFO =======
    int exitCode;
    std::string stdOut;

    // ==> Get original inverted state 
    exitCode = ExecuteHidHideCLI("--inv-state", stdOut);
    if (exitCode != 0) {
        LOG_ERROR("HID HIDE CLI ERROR: " + stdOut);
        return false;
    }
    _originalInvertedState = stdOut;
    _originalInvertedState.erase(std::remove(_originalInvertedState.begin(), _originalInvertedState.end(), '\n'), _originalInvertedState.cend()); // trim

    // ==> Get original cloak state
    exitCode = ExecuteHidHideCLI("--cloak-state", stdOut);
    if (exitCode != 0) {
        LOG_ERROR("HID HIDE CLI ERROR: " + stdOut);
        return false;
    }
    _originalCloakState = stdOut;
    _originalCloakState.erase(std::remove(_originalCloakState.begin(), _originalCloakState.end(), '\n'), _originalCloakState.cend()); // trim

    // ==> Get list of hidden devices right now
    exitCode = ExecuteHidHideCLI("--dev-list", stdOut);
    if (exitCode != 0) {
        LOG_ERROR("HID HIDE CLI ERROR: " + stdOut);
        return false;
    }

    std::vector<std::string> devicesLineOut = WStringUtils::splitString(stdOut, "\n");
    std::vector<std::string> hiddenDevices;
    for (auto& str : devicesLineOut) {
        size_t idx = str.find("\"");
        if (idx == -1)
            continue;
        hiddenDevices.push_back(str.substr((idx + 1), str.length() - idx - 2));
    }

    // ==> Get list of gamepad devices TO HIDE (those already not hidden )
    exitCode = ExecuteHidHideCLI("--dev-gaming ", stdOut);
    if (exitCode != 0) {
        LOG_ERROR("HID HIDE CLI ERROR: " + stdOut);
        return false;
    }
    
    _toHideDevices.clear();
    auto gamepadDevices = json::parse(stdOut);
    for (const auto& item : gamepadDevices) {
        if (item.contains("devices")) {
            for (const auto& device : item["devices"]) {
                // exclude if hidden
                std::string instancePath = device["deviceInstancePath"].get<std::string>();
                if (std::find(hiddenDevices.begin(), hiddenDevices.end(), instancePath) != hiddenDevices.end())
                    continue; // already hidden, skip

                _toHideDevices.push_back(device["deviceInstancePath"].get<std::string>());
            }
        }
    }


    // ======== ACTUAL HIDING ========
    // 1. Grant current executable Hid-Hide whitelist permisison
    exitCode = ExecuteHidHideCLI(std::string("--app-reg ") + "\"" + _currentExePath + "\"", stdOut);
    if (exitCode != 0) {
        LOG_ERROR("HID HIDE CLI ERROR: " + stdOut);
        return false;
    }

    // 2. Hide devices needed to be hidden
    for (auto& device : _toHideDevices) {
        exitCode = ExecuteHidHideCLI(std::string("--dev-hide ") + "\"" + device + "\"", stdOut);
        if (exitCode != 0) {
            LOG_ERROR("HID HIDE CLI ERROR: " + stdOut);
            return false;
        }
    }

    // 3. Set whitelist mode (i.e. ensure everything is blocked)
    exitCode = ExecuteHidHideCLI("--inv-off ", stdOut);
    if (exitCode != 0) {
        LOG_ERROR("HID HIDE CLI ERROR: " + stdOut);
        return false;
    }

    // 4. Actually enable the device hiding!
    exitCode = ExecuteHidHideCLI("--cloak-on ", stdOut);
    if (exitCode != 0) {
        LOG_ERROR("HID HIDE CLI ERROR: " + stdOut);
        return false;
    }

    return true;
}

bool HidHideClient::DisableGamepadHiding()
{
    LOG_DEBUG("Disabling HidHide Gamepad Hiding");
    int exitCode;
    std::string stdOut;

    // 1. Unhide previously hidden devices
    for (auto& device : _toHideDevices) {
        exitCode = ExecuteHidHideCLI(std::string("--dev-unhide ") + "\"" + device + "\"", stdOut);
        if (exitCode != 0) {
            LOG_ERROR("HID HIDE CLI ERROR: " + stdOut);
            return false;
        }
    }

    // 2. Remove executable hid-hide whitelist permission
    exitCode = ExecuteHidHideCLI(std::string("--app-unreg ") + "\"" + _currentExePath + "\"", stdOut);
    if (exitCode != 0)
        LOG_ERROR("HID HIDE CLI ERROR: " + stdOut);

    // 3. Restore original inv state
    exitCode = ExecuteHidHideCLI(_originalInvertedState, stdOut);
    if (exitCode != 0) {
        LOG_ERROR("HID HIDE CLI ERROR: " + stdOut);
        return false;
    }

    // restore original cloak state (if cloak was previously ON, will be left as -is)
    exitCode = ExecuteHidHideCLI(_originalCloakState, stdOut);
    if (exitCode != 0) {
        LOG_ERROR("HID HIDE CLI ERROR: " + stdOut);
        return false;
    }


    return true;
}
