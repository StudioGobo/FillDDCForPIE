// ----------------------------------------------------------------------------
// Copyright (c) Studio Gobo Ltd 2020
// Licensed under the MIT license.  
// See LICENSE.TXT in the project root for license information.
// ----------------------------------------------------------------------------
// File			-> AutomateDDCCommands.h
// Created		-> July 2020
// Author		-> Terence Burns (Studio Gobo)


#pragma once

#if WITH_PERF_AUTOMATION_TESTS && WITH_EDITOR

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "Misc/OutputDevice.h"
#include "FillDDCForPIESettings.h"

/**
 * Latent command to wait for PIE to start running
 */
DEFINE_LATENT_AUTOMATION_COMMAND(FWaitForPIERunning);

/**
 * Latent command to wait for Shader compilation to complete
 */
DEFINE_LATENT_AUTOMATION_COMMAND(FWaitForShaders);

/**
 * Latent command to log progress of test.
 */
DEFINE_LATENT_AUTOMATION_COMMAND_TWO_PARAMETER(FLogProgress, uint32, Step, FString, StatusString);

/**
 * Latent command to log specific information for the test.
 */
DEFINE_LATENT_AUTOMATION_COMMAND_ONE_PARAMETER(FLogInformation, FString, StatusString);

/**
 * Latent command to log specific information for the test.
 */
DEFINE_LATENT_AUTOMATION_COMMAND_ONE_PARAMETER(FLogStepInformation, FAutomateDDCStep, AutomateStep);

/**
 * Latent command to load a level in the editor
 */
DEFINE_LATENT_AUTOMATION_COMMAND_ONE_PARAMETER(FLoadLevelInEditorCommand, FString, MapName);


/**
 * Extensible element of our tests.
 * These commands are queued to allow users to extend functionality whilst in PIE.
 * You can execute a console command, and wait for a timeout, or wait for a log entry in the log.
 */
class FAutomateDDCCommand : public IAutomationLatentCommand, public FOutputDevice
{
public:
	/**
	 * Latent automation command which is based on a FAutomateDDCStep object defined in the Engine.ini.
	 */
	FAutomateDDCCommand(FAutomateDDCStep InAutomateDDCStep);

	/**
	 * Cleanup anything we may need to after the command.
	 */
	virtual ~FAutomateDDCCommand();

	// Begin FOutputDevice interface
	virtual void Serialize(const TCHAR* LoggedString, ELogVerbosity::Type /*Verbosity*/, const class FName& /*Category*/) override;
	// End FOutputDevice interface

	// Begin IAutomationLatentCommand interface
	virtual bool Update() override;
	// End IAutomationLatentCommand interface

private:
	// Execution info for this step.
	FAutomateDDCStep AutomateDDCStep;

	// Can this step be advanced via a log output string?
	bool bShouldListenForLogString;

	// Did we find the log string we could be listening for
	bool bLogStringFound;

	// Time that this command was created.
	double CommandStartTime;

	// We need to track this as we exec on the first tick.
	bool bHasFiredExecCmd;
};

#endif // WITH_PERF_AUTOMATION_TESTS && WITH_EDITOR