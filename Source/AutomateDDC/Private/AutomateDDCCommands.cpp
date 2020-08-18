// ----------------------------------------------------------------------------
// Copyright (c) Studio Gobo Ltd 2020
// Licensed under the MIT license.  
// See LICENSE.TXT in the project root for license information.
// ----------------------------------------------------------------------------
// File			-> AutomateDDCCommands.cpp
// Created		-> July 2020
// Author		-> Terence Burns (Studio Gobo)


#if WITH_PERF_AUTOMATION_TESTS && WITH_EDITOR

#include "AutomateDDCCommands.h"
#include "AutomateDDCCommon.h"
#include "FillDDCForPIESettings.h"

#include "CoreMinimal.h"
#include "Tests/AutomationCommon.h"

#include "ShaderCompiler.h"
#include "Kismet2/DebuggerCommands.h"
#include "FileHelpers.h"
#include "Tests/AutomationEditorCommon.h"

bool FWaitForPIERunning::Update()
{
	return FPlayWorldCommandCallbacks::IsInPIE_AndRunning();
}


bool FWaitForShaders::Update()
{
	return !GShaderCompilingManager->IsCompiling();
}


bool FLogProgress::Update()
{
	UE_LOG(LogFillDDCForPIE, Log, TEXT("Step %d: %s"), Step, *StatusString);
	return true;
}


bool FLogInformation::Update()
{
	UE_LOG(LogFillDDCForPIE, Log, TEXT("%s"), *StatusString);
	return true;
}


bool FLoadLevelInEditorCommand::Update()
{
	UE_LOG(LogFillDDCForPIE, Log, TEXT("Opening map: %s"), *MapName);
	if (!FEditorFileUtils::LoadMap(MapName, false /* bLoadAsTemplate */, false /* bShowProgress */))
	{
		UE_LOG(LogFillDDCForPIE, Error, TEXT("Failed to open map: %s"), *MapName);
	}
	return true;
}


bool FLogStepInformation::Update()
{
	UE_LOG(LogFillDDCForPIE, Verbose, TEXT("Running Step: %s"), *AutomateStep.StepId);
	UE_LOG(LogFillDDCForPIE, Verbose, TEXT("Automation Step Information:"));
	UE_LOG(LogFillDDCForPIE, Verbose, TEXT("\tStep Id:\t\t%s"), *AutomateStep.StepId);
	UE_LOG(LogFillDDCForPIE, Verbose, TEXT("\tCommand:\t\t%s"), *AutomateStep.ExecCmd);
	FString EndConditionFormattedStr = TEXT("\tEnd Condition:\t");
	{
		bool bHadEndCondition = false;
		if (AutomateStep.CompleteCondition.LogStringForAdvance.Len() > 0)
		{
			EndConditionFormattedStr += FString::Printf(TEXT("Waiting for logged string: \"%s\""), *AutomateStep.CompleteCondition.LogStringForAdvance);
			bHadEndCondition = true;
		}

		if (AutomateStep.CompleteCondition.TimeoutSeconds > 0.0f)
		{
			EndConditionFormattedStr += FString::Printf(TEXT("%sTimeout set to %.2f seconds."), (bHadEndCondition ? TEXT(" & ") : TEXT("")), AutomateStep.CompleteCondition.TimeoutSeconds);
			bHadEndCondition = true;
		}

		if (!bHadEndCondition)
		{
			EndConditionFormattedStr += TEXT("None.");
		}
	}
	UE_LOG(LogFillDDCForPIE, Verbose, TEXT("%s"), *EndConditionFormattedStr);
	return true;
}


FAutomateDDCCommand::FAutomateDDCCommand(FAutomateDDCStep InAutomateDDCStep)
	: AutomateDDCStep(InAutomateDDCStep)
	, bShouldListenForLogString(false)
	, bLogStringFound(false)
	, bHasFiredExecCmd(false)
{
	// If we are listening for a log string as part of the command, we need to register this to the GLog to listen for
	// Serialize events.
	bShouldListenForLogString = AutomateDDCStep.CompleteCondition.LogStringForAdvance.Len() > 0;
	if (bShouldListenForLogString)
	{
		GLog->AddOutputDevice(this);
	}
}


FAutomateDDCCommand::~FAutomateDDCCommand()
{
	if (bShouldListenForLogString)
	{
		if (GLog->IsRedirectingTo(this))
		{
			GLog->RemoveOutputDevice(this);
		}
	}
}


void FAutomateDDCCommand::Serialize(const TCHAR* LoggedString, ELogVerbosity::Type /*Verbosity*/, const class FName& Category)
{
	if (Category != "LogFillDDCForPIE")
	{
		const FString FormattedString(LoggedString);
		bLogStringFound |= FormattedString.Contains(AutomateDDCStep.CompleteCondition.LogStringForAdvance);
	}
}


bool FAutomateDDCCommand::Update()
{
	// Only return true when we have reached our end condition for this command.
	bool bShouldAdvance = false;
	if (bHasFiredExecCmd)
	{
		if (bShouldListenForLogString && bLogStringFound)
		{
			bShouldAdvance = true;
			UE_LOG(LogFillDDCForPIE, Log, TEXT("Found \"%s\" in output log"), *AutomateDDCStep.CompleteCondition.LogStringForAdvance);
		}
		else if (AutomateDDCStep.CompleteCondition.TimeoutSeconds < (FPlatformTime::Seconds() - CommandStartTime))
		{
			UE_LOG(LogFillDDCForPIE, Log, TEXT("Command \"%s\" timed out after %.2f seconds"), *AutomateDDCStep.StepId, AutomateDDCStep.CompleteCondition.TimeoutSeconds);
			bShouldAdvance = true;
		}
	}
	else
	{
		// Fire off the console command
		GEngine->Exec(nullptr, *AutomateDDCStep.ExecCmd);
		bHasFiredExecCmd = true;

		// Register start time for timeouts
		CommandStartTime = FPlatformTime::Seconds();
	}
	
	return bShouldAdvance;
}


#endif // WITH_PERF_AUTOMATION_TESTS && WITH_EDITOR