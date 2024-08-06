#include "JoyMacroLogging.h"

void PrintScreenLogDebug(std::string message) {
#if defined(_DEBUG)
	std::cout << "[DEBUG] " + message << std::endl;
#endif

#if !defined(_DEBUG)
	MessageBox(NULL, L"Debug Message", WStringUtils::ConvertUtf8ToWide(message).c_str(), MB_OK);
#endif
}

void PrintScreenLogWarn(std::string message) {
	std::cout << "[WARN] " + message << std::endl;
}

void PrintScreenLogError(std::string message) {
	std::cerr << "[ERROR] " + message << std::endl;
}
