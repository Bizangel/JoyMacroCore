#include "JoyMacroLogging.h"

void PrintScreenLogDebug(std::string message) {
#if defined(_DEBUG)
	std::cout << "[DEBUG] " + message << std::endl;
#endif
}

void PrintScreenLogWarn(std::string message) {
#if defined(_DEBUG)
	std::cout << "[WARN] " + message << std::endl;
#endif

#if !defined(_DEBUG)
	MessageBox(NULL, WStringUtils::ConvertUtf8ToWide(message).c_str(), L"JoyMacroCore WARNING", MB_OK | MB_ICONWARNING);
#endif
}

void PrintScreenLogError(std::string message) {
#if defined(_DEBUG)
	std::cerr << "[ERROR] " + message << std::endl;
#endif

#if !defined(_DEBUG)
	MessageBox(NULL, WStringUtils::ConvertUtf8ToWide(message).c_str(), L"JoymacroCore ERROR", MB_OK | MB_ICONERROR);
#endif
}
