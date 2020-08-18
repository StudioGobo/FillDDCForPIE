// ----------------------------------------------------------------------------
// Copyright (c) Studio Gobo Ltd 2020
// Licensed under the MIT license.  
// See LICENSE.TXT in the project root for license information.
// ----------------------------------------------------------------------------
// File			-> AutomateDDC.cpp
// Created		-> July 2020
// Author		-> Terence Burns (Studio Gobo)


#include "AutomateDDC.h"
#include "AutomateDDCCommon.h"

DEFINE_LOG_CATEGORY(LogFillDDCForPIE);
#define LOCTEXT_NAMESPACE "FAutomateDDCModule"

void FAutomateDDCModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FAutomateDDCModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FAutomateDDCModule, AutomateDDC)