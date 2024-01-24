// Copyright 2021 Sam Carey. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ProjectModuleData.h"
#include "UObject/NoExportTypes.h"
#include "ModuleEditorTool.generated.h"


/**
 * ������ELoadingPhase����
 * �˴��ٶ���һ�飬����������Ĵ���
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
 * ������EHostType����
 * �˴��ٶ���һ�飬����������Ĵ���
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
	 * �Ƿ���ӵ������
	 */
	UPROPERTY(EditAnywhere, Category="New Module")
	bool AddToPlugin = false;
	
	/**
	 * �������
	 */
	FString PluginName = "ExamplePlugin";
	
	/**
	 * ����������ģ�������
	 */
	UPROPERTY(EditAnywhere, Category = "New Module")
	FString NewModuleName = "";
	
	/**
	 * ģ��Ԥ��
	 */
	UPROPERTY(EditAnywhere, Category = "New Module")
	EModuleTemplates ModuleTemplate = EModuleTemplates::Runtime;
	
	/**
	 * Ҫ������ģ�������
	 */
	UPROPERTY(EditAnywhere, Category = "New Module", meta = (EditCondition = "ModuleTemplate == EModuleTemplates::Custom"))
	ECPPModuleType ModuleType = ECPPModuleType::Runtime;
	
	/**
	 * ģ����صĽ׶�
	 */
	UPROPERTY(EditAnywhere, Category = "New Module", meta = (EditCondition = "ModuleTemplate == EModuleTemplates::Custom"))
	EModuleLoadingPhase LoadingPhase = EModuleLoadingPhase::Default;

	bool ValidateInput(FString& ErrorMessage);
	
	/**
	 * ����ģ������������ļ�
	 */
	void GenerateModule();
	
private:
	/**
	 * ȡһ��UPlugin��UProject�ַ���������JSONע��ģ���б�
	 * @param UProjectOrPluginString UPlugin��UProject�ַ���
	 * @param NewModule ģ������
	 * @return �Ƿ�ɹ�
	 */
	bool InjectModuleData(FString& UProjectOrPluginString, const FProjectModuleData& NewModule);
	
	/**
	 * ��ȡĿ���·������ModuleNamesע���乹�캯��
	 * @param TargetPath �ļ�·��
	 * @param ExtraModuleName ģ������
	 */
	void InjectExtraModuleNames(const FString& TargetPath, const FString& ExtraModuleName);
	
	/**
	 * ���ƽ������Ϸ��ַ�
	 * @param ModuleName 
	 * @param OutIllegalCharacters 
	 * @return �����Ƿ�������Ϸ��ַ�
	 */
	bool NameContainsOnlyLegalCharacters(const FString& ModuleName, FString& OutIllegalCharacters);
};
