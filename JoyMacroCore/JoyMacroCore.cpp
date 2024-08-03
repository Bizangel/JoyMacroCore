// JoyMacroCore.cpp : Defines the functions for the static library.
//

#include "pch.h"
#include "framework.h"
#include "JoyMacroCore.h"

#include <iostream>
#include "JoyMacroCore.h"
#include "InternalHeader.h"

// TODO: This is an example of a library function
__declspec(dllexport) void fnJoyMacroCore()
{
	std::cout << "iprint changed 4" << std::endl;
}

void fnJoyMacroCoreWithDep()
{
	someFunctionIneed();
}
