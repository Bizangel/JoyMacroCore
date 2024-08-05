#pragma once

#include <Windows.h>
#include <string>

class WStringUtils
{
public:
	static std::wstring ConvertUtf8ToWide(const std::string& string);
	static std::string ConvertWideToUtf8(const std::wstring& string);
};

