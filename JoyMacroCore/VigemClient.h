#pragma once

#include "JoyMacroLogging.h"
#include <iostream>
#include <sstream>
#include <windows.h>
#include <ViGEm/Client.h>

class VigemController;

class VigemClient
{
private:
	PVIGEM_CLIENT client;
	std::unique_ptr<VigemController> _controller;
	bool initted = false;
public:
	~VigemClient();

	bool Initialize();
	bool CreateAndPlugController(int physicalControllerIndex);
	bool DeleteAndUnplugController();

	VigemController* getControllerRef();
	friend class VigemController;
};