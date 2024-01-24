// Copyright 2021 Sam Carey. All Rights Reserved.

#include "ModuleGeneratorCommands.h"

#define LOCTEXT_NAMESPACE "FModuleGeneratorModule"

void FModuleGeneratorCommands::RegisterCommands()
{
	UI_COMMAND(OpenModuleGeneratorTool, "Module Generator", "Bring up ModuleGenerator window", EUserInterfaceActionType::Button, FInputGesture());
}