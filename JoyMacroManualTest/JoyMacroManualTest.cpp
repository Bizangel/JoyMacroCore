// JoyMacroManualTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "JoyMacroCore.h"
#include "ExampleOverrider.h"
#include <vector>

int main()
{
    std::unique_ptr<ExampleOverrider> overrider = std::make_unique<ExampleOverrider>();
    std::unique_ptr<JoyMacroOverrideClient> overrideClient = std::make_unique<JoyMacroOverrideClient>();

    // set custom polling delay we want
    overrideClient->setPollingDelayMs(10);

    // can be used to disable hidhide. Of course this will mean TWO controllers will be detected at the same time. You can use hid hide manually if you'd like
    // NOTE: you need to reconnect your controller if you ARE using hidhide. 
    overrideClient->disableHidHide(); 

    int activeControllerIndex = overrideClient->getFirstActiveControllerIndex();
    if (activeControllerIndex == -1) {
        std::cout << "No connected controllers" << std::endl;
        return 0;
    }

    // basic paddle keymapping
    //std::vector<int> keymapping{ 'Q','W','E','R' };
    std::vector<int> keymapping{ 187, 221, 222 , 192 };

    if (overrideClient->StartOverride(activeControllerIndex, overrider.get(), keymapping) != JoyMacroExitCode::SUCCESS)
        return -1;

    std::cout << "Press To Stop" << std::endl;
    system("pause");

    overrideClient->StopOverride();
}