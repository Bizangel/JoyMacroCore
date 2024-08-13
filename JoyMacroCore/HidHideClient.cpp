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

// Not too familiar with winapi, this is mostly ChatGPT tbh
int CreateProcessWithPipes(const std::string& command, std::string& stdOut, std::string& stdErr)
{
    HANDLE hStdOutRead, hStdOutWrite;
    HANDLE hStdErrRead, hStdErrWrite;
    HANDLE hStdInRead, hStdInWrite; 
    SECURITY_ATTRIBUTES saAttr;
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;

    // Create a pipe for the child process's STDOUT.
    int stdOutPipeCreateResult = CreatePipe(&hStdOutRead, &hStdOutWrite, &saAttr, 0);
    int stdErrPipeCreateResult = CreatePipe(&hStdErrRead, &hStdErrWrite, &saAttr, 0);
    int stdInPipeCreateResult = CreatePipe(&hStdInRead, &hStdInWrite, &saAttr, 0);
    bool errorCreatingPipe = (stdOutPipeCreateResult == 0) || (stdErrPipeCreateResult == 0) || (stdInPipeCreateResult == 0);
    
    if (errorCreatingPipe) {
        if (!stdOutPipeCreateResult)
            CloseHandle(hStdOutWrite);

        if (!stdErrPipeCreateResult)
            CloseHandle(hStdErrWrite);

        if (!stdInPipeCreateResult)
            CloseHandle(hStdInWrite);

        stdErr = "Unable to create pipes for child process";
        return -1;
    }

    // Ensure the pipe handles are not inheritted
    SetHandleInformation(hStdOutRead, HANDLE_FLAG_INHERIT, 0);
    SetHandleInformation(hStdErrRead, HANDLE_FLAG_INHERIT, 0);
    SetHandleInformation(hStdInWrite, HANDLE_FLAG_INHERIT, 0);

    // Create the child process.
    PROCESS_INFORMATION piProcInfo;
    STARTUPINFO siStartInfo;

    ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));
    ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
    siStartInfo.cb = sizeof(STARTUPINFO);
    siStartInfo.hStdError = hStdErrWrite;
    siStartInfo.hStdOutput = hStdOutWrite;
    siStartInfo.hStdInput = hStdInRead;
    siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

    std::wstring cmd = WStringUtils::ConvertUtf8ToWide(command);
    
    // Create the child process.
    BOOL bSuccess = CreateProcess(
        NULL,           // No module name (use command line)
        &cmd[0],     // Command line
        NULL,           // Process handle not inheritable
        NULL,           // Thread handle not inheritable
        TRUE,           // Set handle inheritance to TRUE
        0,              // No creation flags
        NULL,           // Use parent's environment block
        NULL,           // Use parent's starting directory 
        &siStartInfo,   // Pointer to STARTUPINFO structure
        &piProcInfo);   // Pointer to PROCESS_INFORMATION structure

    // If an error occurs, exit the application.
    if (!bSuccess)
    {
        CloseHandle(hStdOutWrite);
        CloseHandle(hStdErrWrite);
        CloseHandle(hStdInWrite);
        return -1;
    }
    else
    {
        // Close the write end of the pipes now that it is no longer needed.
        CloseHandle(hStdOutWrite);
        CloseHandle(hStdErrWrite);
        CloseHandle(hStdInWrite);

        // Read output from the child process's pipe for STDOUT and STDERR.
        DWORD dwRead;
        CHAR chBuf[4096];
        std::string result;

        // Read from stdout
        while (ReadFile(hStdOutRead, chBuf, sizeof(chBuf), &dwRead, NULL) && dwRead > 0)
        {
            stdOut.append(chBuf, dwRead);
        }

        // Read from stderr
        while (ReadFile(hStdErrRead, chBuf, sizeof(chBuf), &dwRead, NULL) && dwRead > 0)
        {
            stdErr.append(chBuf, dwRead);
        }

        // Wait for the child process to exit.
        WaitForSingleObject(piProcInfo.hProcess, INFINITE);

        // Get the exit code.
        DWORD exitCode;
        GetExitCodeProcess(piProcInfo.hProcess, &exitCode);

        // Close handles.
        CloseHandle(piProcInfo.hProcess);
        CloseHandle(piProcInfo.hThread);

        return exitCode;
    }
}

int HidHideClient::Popen(const std::string& command, std::string& stdOut)
{
    std::string stdErrInt;
    std::string stdOutInt;
    std::string fullCommand = command;
    LOG_DEBUG("Executing Command: " + fullCommand);
    int code = CreateProcessWithPipes(fullCommand, stdOutInt, stdErrInt);
    stdOut = stdOutInt + stdErrInt;
    return code;
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
