// ----------------------------------------------------------------------------
// Copyright (c) Studio Gobo Ltd 2020
// Licensed under the MIT license.
// See LICENSE.TXT in the project root for license information.
// ----------------------------------------------------------------------------
// File			-> FillDDCForPIESettings.h
// Created		-> July 2020
// Author		-> Terence Burns (Studio Gobo)


#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Object.h"

#include "FillDDCForPIESettings.generated.h"


/** 
 * Completion information for the FAutomateDDCStep
 */
USTRUCT()
struct FAutomateDDCStepCompleteCondition
{
	GENERATED_BODY()
		
	// Log Output string that, if found, will mark this step complete.
	UPROPERTY()
	FString LogStringForAdvance;

	// Timeout for this step.
	UPROPERTY()
	float TimeoutSeconds;
};


/**
 * Customizable step for the Automate DDC Task. 
 * These are configured as part of the /Script/AutomateDDC.FillDDCForPIESettings Section of the *Engine.ini.
 */
USTRUCT()
struct FAutomateDDCStep
{
	GENERATED_BODY()

	// An identifier for this step. Useful for debugging via logging
	UPROPERTY()
	FString StepId;

	// Console Command to run as part of this step
	UPROPERTY()
	FString ExecCmd;

	// Information about how this step can complete.
	UPROPERTY()
	FAutomateDDCStepCompleteCondition CompleteCondition;
	
	// After this step has completed, should we wait for Shaders to compile etc...?
	UPROPERTY()
	bool bWaitForSettleAfterStep;
};


/**
 *
 */
UCLASS(config=Engine, defaultconfig)
class AUTOMATEDDC_API UFillDDCForPIESettings : public UObject
{
public:
	GENERATED_BODY()

	// The list of maps we wish to cache ddc data for.
	UPROPERTY(config, EditAnywhere, Category = "Maps")
	TArray<FString> Maps;

	// Steps run when PIE has launched as part of the task.
	UPROPERTY(config, EditAnywhere)
	TArray<FAutomateDDCStep> ExtraSteps;
};
