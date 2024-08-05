#pragma once

#include <Windows.h>
#include <string>
#include "WStringUtils.h"

class RegistryUtils
{
public:
	static std::string ReadRegistryString(HKEY regHkey, LPCWSTR regkeySubkey, const std::wstring& strValueName);
};

