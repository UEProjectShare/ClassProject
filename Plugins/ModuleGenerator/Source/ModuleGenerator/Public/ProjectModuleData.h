// Copyright 2021 Sam Carey. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ProjectModuleData.generated.h"

/**
 * 模块数据
 */
USTRUCT()
struct FProjectModuleData
{
	GENERATED_BODY()

	/**
	 * 模块名字
	 */
	UPROPERTY()
	FString Name;
	
	/**
	 * 模块类型
	 */
	UPROPERTY()
	FString Type;
	
	/**
	 * 模块加载阶段
	 */
	UPROPERTY()
	FString LoadingPhase;
};