// Copyright 2021 Sam Carey. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ProjectModuleData.h"
#include "UObject/NoExportTypes.h"
#include "ModuleEditorTool.generated.h"


/**
 * 由引擎ELoadingPhase定义
 * 此处再定义一遍，不依赖引擎的代码
 */
UENUM(BlueprintType)
enum class EModuleLoadingPhase : uint8
{
	EarliestPossible		UMETA(DisplayName = "EarliestPossible"),
	PostConfigInit			UMETA(DisplayName = "PostConfigInit"),
	PostSplashScreen		UMETA(DisplayName = "PostSplashScreen"),
	PreEarlyLoadingScreen	UMETA(DisplayName = "PreEarlyLoadingScreen"),
	PreLoadingScreen		UMETA(DisplayName = "PreLoadingScreen"),
	PreDefault				UMETA(DisplayName = "PreDefault"),
	Default					UMETA(DisplayName = "Default"),
	PostDefault				UMETA(DisplayName = "PostDefault"),
	PostEngineInit			UMETA(DisplayName = "PostEngineInit"),
	None					UMETA(DisplayName = "None"),
	Max						UMETA(DisplayName = "Max")
};

/**
 * 由引擎EHostType定义
 * 此处再定义一遍，不依赖引擎的代码
 */
UENUM(BlueprintType)
enum class ECPPModuleType : uint8
{
	Runtime					UMETA(DisplayName = "Runtime"),
	RuntimeNoCommandlet		UMETA(DisplayName = "RuntimeNoCommandlet"),
	RuntimeAndProgram		UMETA(DisplayName = "RuntimeAndProgram"),
	CookedOnly				UMETA(DisplayName = "CookedOnly"),
	UncookedOnly			UMETA(DisplayName = "UncookedOnly"),
	Developer				UMETA(DisplayName = "Developer"),
	DeveloperTool			UMETA(DisplayName = "DeveloperTool"),
	Editor					UMETA(DisplayName = "Editor"),
	EditorNoCommandlet		UMETA(DisplayName = "EditorNoCommandlet"),
	EditorAndProgram		UMETA(DisplayName = "EditorAndProgram"),
	Program					UMETA(DisplayName = "Program"),
	ServerOnly				UMETA(DisplayName = "ServerOnly"),
	ClientOnly				UMETA(DisplayName = "ClientOnly"),
	ClientOnlyNoCommandlet	UMETA(DisplayName = "ClientOnlyNoCommandlet"),
	Max						UMETA(DisplayName = "Max"),
};

UENUM(BlueprintType)
enum class EModuleTemplates : uint8
{
	Custom		UMETA(DisplayName = "Custom"),
	Editor		UMETA(DisplayName = "Editor"),
	Runtime		UMETA(DisplayName = "Runtime")
};


UCLASS()
class MODULEGENERATOR_API UModuleEditorTool : public UObject
{
	GENERATED_BODY()
public:
	/**
	 * 是否添加到插件中
	 */
	UPROPERTY(EditAnywhere, Category="New Module")
	bool AddToPlugin = false;
	
	/**
	 * 插件名字
	 */
	FString PluginName = "ExamplePlugin";
	
	/**
	 * 将创建的新模块的名称
	 */
	UPROPERTY(EditAnywhere, Category = "New Module")
	FString NewModuleName = "";
	
	/**
	 * 模块预设
	 */
	UPROPERTY(EditAnywhere, Category = "New Module")
	EModuleTemplates ModuleTemplate = EModuleTemplates::Runtime;
	
	/**
	 * 要创建的模块的类型
	 */
	UPROPERTY(EditAnywhere, Category = "New Module", meta = (EditCondition = "ModuleTemplate == EModuleTemplates::Custom"))
	ECPPModuleType ModuleType = ECPPModuleType::Runtime;
	
	/**
	 * 模块加载的阶段
	 */
	UPROPERTY(EditAnywhere, Category = "New Module", meta = (EditCondition = "ModuleTemplate == EModuleTemplates::Custom"))
	EModuleLoadingPhase LoadingPhase = EModuleLoadingPhase::Default;

	bool ValidateInput(FString& ErrorMessage);
	
	/**
	 * 生成模块所需的所有文件
	 */
	void GenerateModule();
	
private:
	/**
	 * 取一个UPlugin或UProject字符串，并将JSON注入模块列表
	 * @param UProjectOrPluginString UPlugin或UProject字符串
	 * @param NewModule 模块数据
	 * @return 是否成功
	 */
	bool InjectModuleData(FString& UProjectOrPluginString, const FProjectModuleData& NewModule);
	
	/**
	 * 获取目标的路径并将ModuleNames注入其构造函数
	 * @param TargetPath 文件路径
	 * @param ExtraModuleName 模块名字
	 */
	void InjectExtraModuleNames(const FString& TargetPath, const FString& ExtraModuleName);
	
	/**
	 * 名称仅包含合法字符
	 * @param ModuleName 
	 * @param OutIllegalCharacters 
	 * @return 名称是否仅包含合法字符
	 */
	bool NameContainsOnlyLegalCharacters(const FString& ModuleName, FString& OutIllegalCharacters);
};
