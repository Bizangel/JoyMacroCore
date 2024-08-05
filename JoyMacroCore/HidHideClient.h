#pragma once

#include <string>
#include "JoyMacroLogging.h"
#include "RegistryUtils.h"
#include <filesystem>

class HidHideClient
{
private:
	std::string _hidCLIPath;
	int ExecuteHidHideCLI(const std::string parameters, std::string& stdOut);


public:
	bool InitAndEnsureHidHideInstalled();

	bool EnableGamepadHiding();
	bool DisableGamepadHiding();
};

