// JoyMacroManualTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "JoyMacroCore.h"
#include "ExampleOverrider.h"

int main()
{
    std::unique_ptr<ExampleOverrider> overrider = std::make_unique<ExampleOverrider>();
    std::unique_ptr<JoyMacroOverrideClient> overrideClient = std::make_unique<JoyMacroOverrideClient>();

    overrideClient->StartOverride(1, overrider.get());
    std::cout << "Press To Stop" << std::endl;
    system("pause");

    overrideClient->StopOverride();
    std::cout << "Press to Exit" << std::endl;
    system("pause");
}