#pragma once

#include <Windows.h>
#include <string>
#include <vector>

class WStringUtils
{
public:
	static std::wstring ConvertUtf8ToWide(const std::string& string);
	static std::string ConvertWideToUtf8(const std::wstring& string);

	static std::vector<std::string> splitString(const std::string& s, const std::string& delimiter);
};

