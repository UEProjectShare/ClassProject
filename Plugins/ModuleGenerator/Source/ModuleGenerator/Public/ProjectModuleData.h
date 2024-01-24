// Copyright 2021 Sam Carey. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ProjectModuleData.generated.h"

/**
 * ģ������
 */
USTRUCT()
struct FProjectModuleData
{
	GENERATED_BODY()

	/**
	 * ģ������
	 */
	UPROPERTY()
	FString Name;
	
	/**
	 * ģ������
	 */
	UPROPERTY()
	FString Type;
	
	/**
	 * ģ����ؽ׶�
	 */
	UPROPERTY()
	FString LoadingPhase;
};