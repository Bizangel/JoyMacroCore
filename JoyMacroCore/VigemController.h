#pragma once

#include <sstream>
#include "VigemClient.h"
#include <Xinput.h>

class VigemController
{
private:
	VigemClient* _parentClient;
	PVIGEM_TARGET _controlledPad;
	bool _pluggedIn;

	void RegisterVibrationCallback();
public:
	int controllerOverrideIndex;
	static int toSendVibration; // the controller to forward any vibration input from virtual controller.

	VigemController(VigemClient* parent, PVIGEM_TARGET cont, int physicalControllerIndex);
	~VigemController();

	void UpdateState(XINPUT_GAMEPAD& state);

	bool PlugIn();
	bool UnPlug();
	bool isPluggedIn() const;

	friend class VigemClient;
};

