#pragma once

#include <string>
#include "JoyMacroLogging.h"
#include "RegistryUtils.h"
#include <filesystem>
#include <algorithm>

class HidHideClient
{
private:
	std::string _hidCLIPath;
	std::string _currentExePath;
	std::vector<std::string> _toHideDevices;
	std::string _originalInvertedState;
	std::string _originalCloakState;

	int ExecuteHidHideCLI(const std::string parameters, std::string& stdOut);

	int Popen(const std::string& command, std::string& stdOut);
	std::string getCurrentlyRunningExecutablePath();
public:
	bool InitAndEnsureHidHideInstalled();

	bool EnableGamepadHiding();
	bool DisableGamepadHiding();
};

