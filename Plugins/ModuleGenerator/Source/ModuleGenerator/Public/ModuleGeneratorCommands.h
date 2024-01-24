// Copyright 2021 Sam Carey. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "ModuleGeneratorStyle.h"

/**
 * UI Command
 */
class FModuleGeneratorCommands : public TCommands<FModuleGeneratorCommands>
{
public:
	FModuleGeneratorCommands()
		: TCommands<FModuleGeneratorCommands>(
			TEXT("ModuleGenerator"),
			NSLOCTEXT("Contexts", "ModuleGenerator", "ModuleGenerator Plugin"),
			NAME_None,
			FModuleGeneratorStyle::GetStyleSetName())
	{
	}
	
	/**
	 * ×¢²áUI Command
	 */
	virtual void RegisterCommands() override;
	
	/**
	 * UI CommandInfo
	 */
	TSharedPtr<FUICommandInfo> OpenModuleGeneratorTool;
};
