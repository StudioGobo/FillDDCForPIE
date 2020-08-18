// ----------------------------------------------------------------------------
// Copyright (c) Studio Gobo Ltd 2020
// Licensed under the MIT license.  
// See LICENSE.TXT in the project root for license information.
// ----------------------------------------------------------------------------
// File			-> AutomateDDCTest.cpp
// Created		-> July 2020
// Author		-> Terence Burns (Studio Gobo)


#if WITH_PERF_AUTOMATION_TESTS && WITH_EDITOR

#include "CoreMinimal.h"
#include "Tests/AutomationCommon.h"

#include "ShaderCompiler.h"
#include "Tests/AutomationEditorCommon.h"

#include "AutomateDDCCommon.h"
#include "AutomateDDCCommands.h"
#include "FillDDCForPIESettings.h"

// Constants used as part of the Test
namespace AutomateDDCTestConstants
{
	// How long should we wait between each command.
	const float SecondsToAllowSettle = 3.0f;

	// We wait when the game has settled to allow any post load shaders etc to start up.
	const float SecondsForLongWaitAfterSettle = 10.0f;
}


///////////////////////////////////////////////////////////////
// Test Implementation


IMPLEMENT_SIMPLE_AUTOMATION_TEST(FFillDDCForPIETest, "FillDDCForPIETest", EAutomationTestFlags::ClientContext | EAutomationTestFlags::PerfFilter | EAutomationTestFlags::EditorContext)

bool FFillDDCForPIETest::RunTest(const FString& Parameters)
{
	const UFillDDCForPIESettings* AutomateDDCSettings = GetDefault<UFillDDCForPIESettings>();
	UE_LOG(LogFillDDCForPIE, Log, TEXT("Iterating on %d maps."), AutomateDDCSettings->Maps.Num());
	UE_LOG(LogFillDDCForPIE, Log, TEXT("Fill DDC For PIE Test has %d extra steps."), AutomateDDCSettings->ExtraSteps.Num());

	// General Editor Wait for shaders etc...
	auto WaitForSettle = [](const FString WaitReason, const float MinWaitTime)
	{
		ADD_LATENT_AUTOMATION_COMMAND(FLogInformation(FString::Printf(TEXT("Waiting for editor to settle: %s."), *WaitReason)));
		ADD_LATENT_AUTOMATION_COMMAND(FEngineWaitLatentCommand(MinWaitTime));
		ADD_LATENT_AUTOMATION_COMMAND(FStreamAllResourcesLatentCommand(0.0f));
		ADD_LATENT_AUTOMATION_COMMAND(FWaitForShaders());
		ADD_LATENT_AUTOMATION_COMMAND(FLogInformation(TEXT("Editor has settled.")));
	};

	// Add your Maps to the /Script/AutomateDDC.AutomateDDCSettings Section of the *Engine.ini
	for (const FString& MapName : AutomateDDCSettings->Maps)
	{
		// Open the map
		ADD_LATENT_AUTOMATION_COMMAND(FLoadLevelInEditorCommand(MapName));
		{
			WaitForSettle(TEXT("After Opening Map"), AutomateDDCTestConstants::SecondsForLongWaitAfterSettle);

			// Run a Play In Editor Session with the current map.
			ADD_LATENT_AUTOMATION_COMMAND(FLogInformation(TEXT("Launching PIE.")));
			ADD_LATENT_AUTOMATION_COMMAND(FStartPIECommand(false));
			ADD_LATENT_AUTOMATION_COMMAND(FWaitForPIERunning());
			WaitForSettle(TEXT("After Launching PIE"), AutomateDDCTestConstants::SecondsForLongWaitAfterSettle);

			// Allow user to extend functionality.
			// Anything configured in the /Script/AutomateDDC.AutomateDDCSettings Section of the *Engine.ini is run here.
			for (const FAutomateDDCStep& AutomateDDCStep : AutomateDDCSettings->ExtraSteps)
			{
				ADD_LATENT_AUTOMATION_COMMAND(FLogStepInformation(AutomateDDCStep));
				ADD_LATENT_AUTOMATION_COMMAND(FAutomateDDCCommand(AutomateDDCStep));
				ADD_LATENT_AUTOMATION_COMMAND(FLogInformation(FString::Printf(TEXT("Test step complete [%s]"), *AutomateDDCStep.StepId)));
				if (AutomateDDCStep.bWaitForSettleAfterStep)
				{
					WaitForSettle(TEXT("Custom Step Requirement"), AutomateDDCTestConstants::SecondsToAllowSettle);
				}
			}
			ADD_LATENT_AUTOMATION_COMMAND(FLogInformation(TEXT("Closing PIE.")));
			ADD_LATENT_AUTOMATION_COMMAND(FEndPlayMapCommand());
			WaitForSettle(TEXT("After Closing PIE"), AutomateDDCTestConstants::SecondsForLongWaitAfterSettle);
		}
	}
	ADD_LATENT_AUTOMATION_COMMAND(FLogInformation(TEXT("Test Complete.")));
	
	// Quit at the end?
	if (FApp::IsUnattended())
	{
		ADD_LATENT_AUTOMATION_COMMAND(FLogInformation(TEXT("Quitting the Editor after AutomateDDCTest.")));
		ADD_LATENT_AUTOMATION_COMMAND(FExecStringLatentCommand(TEXT("QUIT_EDITOR")));
	}
	return true;
}

#endif // WITH_PERF_AUTOMATION_TESTS && WITH_EDITOR