// JoyMacroManualTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "JoyMacroCore.h"
#include "ExampleOverrider.h"

#include <Windows.h>

int main()
{
    std::unique_ptr<ExampleOverrider> overrider = std::make_unique<ExampleOverrider>();
    std::unique_ptr<JoyMacroOverrideClient> overrideClient = std::make_unique<JoyMacroOverrideClient>();

    int activeControllerIndex = overrideClient->getFirstActiveControllerIndex();
    if (activeControllerIndex == -1) {
        std::cout << "No connected controllers" << std::endl;
        return 0;
    }

    overrideClient->StartOverride(activeControllerIndex, overrider.get());
    std::cout << "Press To Stop" << std::endl;
    system("pause");

    overrideClient->StopOverride();
}