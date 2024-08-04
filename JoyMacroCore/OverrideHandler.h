#pragma once

#include "Include/JoyMacroCore.h"
#include "VigemClient.h"
#include "OverriderPollThread.h"

#define POLLING_DELAY_MS 10

class OverrideHandler
{
	// == Singleton
private:
	static std::shared_ptr<OverrideHandler> _instance;
public:
	static std::shared_ptr<OverrideHandler> getInstance();
	// ==

private:
	std::shared_ptr<VigemClient> _vigemClient = nullptr;
	std::unique_ptr<OverriderPollThread> _poller = nullptr;

	int EnsureVigemInitialized();
public:
	JoyMacroExitCode StartOverride(int overrideIndex, IGamepadOverrider* overrider);
	JoyMacroExitCode StopOverride();
};

