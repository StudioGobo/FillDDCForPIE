// ----------------------------------------------------------------------------
// Copyright (c) Studio Gobo Ltd 2020
// Licensed under the MIT license.  
// See LICENSE.TXT in the project root for license information.
// ----------------------------------------------------------------------------
// File			-> AutomateDDC.h
// Created		-> July 2020
// Author		-> Terence Burns (Studio Gobo)


#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FAutomateDDCModule : public IModuleInterface
{
public:
	// Begin IModuleInterface interface
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	// End IModuleInterface interface
};
