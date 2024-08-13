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
    std::string stdOut;
    int exitCode = ExecuteHidHideCLI("--cloak-on ", stdOut);
    if (exitCode != 0) {
        LOG_ERROR("HID HIDE CLI ERROR: " + stdOut);
        return false;
    }

    LOG_INFO("Hid Hide Hiding Enabled! Make sure to add executable to HidHide Config. Remember to reconnect controller for hiding to work!");
    return true;
    // TODO: update logic to automate exe including etc. I had this implemented but not robust enough. See git history for more.
}

bool HidHideClient::DisableGamepadHiding()
{
    LOG_DEBUG("Disabling HidHide Gamepad Hiding");
    std::string stdOut;
    int exitCode = ExecuteHidHideCLI("--cloak-off ", stdOut);
    if (exitCode != 0) {
        LOG_ERROR("HID HIDE CLI ERROR: " + stdOut);
        return false;
    }

    return true;
}
