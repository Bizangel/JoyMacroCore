#include "RegistryUtils.h"

std::string RegistryUtils::ReadRegistryString(HKEY regHkey, LPCWSTR regkeySubkey, const std::wstring& valueName)
{
    HKEY hKey;
    LONG lRes = RegOpenKeyEx(regHkey, regkeySubkey, 0, KEY_READ, &hKey);
    if (lRes != ERROR_SUCCESS) {
        return ""; // empty string ;
    }

    std::wstring returnStringVal;
    WCHAR szBuffer[512];
    DWORD dwBufferSize = sizeof(szBuffer);
    ULONG nError;
    nError = RegQueryValueEx(hKey, valueName.c_str(), 0, NULL, (LPBYTE)szBuffer, &dwBufferSize);
    if (nError != ERROR_SUCCESS)
    {
        RegCloseKey(hKey);
        return ""; // failed return empty
    }

    returnStringVal = szBuffer;
    RegCloseKey(hKey);
    return WStringUtils::ConvertWideToUtf8(returnStringVal);
}