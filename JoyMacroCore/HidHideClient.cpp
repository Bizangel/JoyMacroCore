#include "HidHideClient.h"

#include "json.hpp"
using json = nlohmann::json;

int HidHideClient::ExecuteHidHideCLI(const std::string parameters, std::string& stdOut)
{
    

    return 0;
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
    return true; 
}


// working popen 
int Popen(const std::string& command) {
    std::array<char, 128> buffer;
    std::string result;

    FILE* pipe = _popen(command.c_str(), "r");
    if (!pipe)
    {
        std::cerr << "Couldn't start command." << std::endl;
        return 0;
    }
    while (fgets(buffer.data(), 128, pipe) != NULL) {
        result += buffer.data();
    }
    auto returnCode = _pclose(pipe);

    std::cout << "Output: " << result << std::endl;
    std::cout << "ExitCode" << returnCode << std::endl;
}

bool HidHideClient::EnableGamepadHiding()
{
    LOG_DEBUG("Enabling HidHide Gamepad Hiding");


    // json parse example boilerplate
    //auto j3 = json::parse(R"({"happy": "something", "pi": 3.141})");

    return true;
}

bool HidHideClient::DisableGamepadHiding()
{
    LOG_DEBUG("Disabling HidHide Gamepad Hiding");
    return true;
}
