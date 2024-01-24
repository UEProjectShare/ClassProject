// Copyright 2021 Sam Carey. All Rights Reserved.

#include "ModuleEditorTool.h"
#include "JsonObjectConverter.h"
#include "ProjectModuleData.h"
#include "Misc/FileHelper.h"
#include "Dom/JsonValue.h"
#include "Interfaces/IPluginManager.h"
#include "Internationalization/Regex.h"
#include "Kismet/KismetStringLibrary.h"
#include "Modules/ModuleManager.h"
#include "Serialization/JsonSerializer.h"

bool UModuleEditorTool::ValidateInput(FString &ErrorMessage)
{
	FString ErrorContent;

	const TSharedPtr<IPlugin> PluginToAddTo = IPluginManager::Get().FindPlugin(PluginName);
	const TSharedPtr<IPlugin> PluginTosAddTo = IPluginManager::Get().FindPlugin(TEXT("UdpMessaging"));

	FString LegalCharacters;
	const bool ContainsLegalCharacters = NameContainsOnlyLegalCharacters(NewModuleName, LegalCharacters);
	
	//Ensure the new name is not empty
	if(NewModuleName.IsEmpty())
	{
		ErrorContent = "Module names must not be empty";
	}
	//Ensure the new name does not contain spaces
	else if (UKismetStringLibrary::Contains(NewModuleName, " "))
	{
		ErrorContent = "Module names must not include spaces";
	}
	//Ensure that the first char is not a number
	else if (!FChar::IsAlpha(NewModuleName[0]))
	{
		ErrorContent = "Module names must begin with an alphabetic character";
	}
	else if (!ContainsLegalCharacters)
	{
		ErrorContent = FString::Printf(TEXT("Module names must not contain the following characters: %s"), *LegalCharacters);
	}
	//Ensure the selected plugin is part of this project
	else if (AddToPlugin && !PluginToAddTo.IsValid())
	{
		ErrorContent = "This plugin does not exist";
	}
	//Ensure C++ has been setup
	else if (!FPaths::DirectoryExists(*FPaths::GameSourceDir()))
	{
		ErrorContent = "Unable to create a module without C++ setup. Setup a C++ project to continue";
	}
	//Ensure that that the module does not exist
	else if (FModuleManager::Get().ModuleExists(*NewModuleName))
	{
		ErrorContent = "A module with this name already exists. Please select a new name";
	}

	if(ErrorContent.IsEmpty())
	{
		return true;
	}
	ErrorMessage = ErrorContent;

	return false;
}

void UModuleEditorTool::GenerateModule()
{
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	
	FString PluginToAddToPath;
	TSharedPtr<IPlugin> PluginToAddTo = IPluginManager::Get().FindPlugin(PluginName);
	if (PluginToAddTo.IsValid())
	{
		PluginToAddToPath = PluginToAddTo->GetBaseDir() + "/";
	}

	const FString BasePath = AddToPlugin ? PluginToAddToPath : FPaths::ProjectDir();
	const FString CodePath = BasePath + "/Source/" + NewModuleName;
	const FString PublicPath = CodePath + "/Public/";
	const FString PrivatePath = CodePath + "/Private/";

	
	PlatformFile.CreateDirectoryTree(*PublicPath);
	PlatformFile.CreateDirectoryTree(*PrivatePath);

	const FString BoilerplatesPath = IPluginManager::Get().FindPlugin(TEXT("ModuleGenerator"))->GetBaseDir() + "/Resources/Boilerplates/";
	FString LoadedBoilerplates;


	//Startup Header File
	const FString BoilerplateStartUpHPath = BoilerplatesPath + "BoilerplateStartUpH.txt";
	FFileHelper::LoadFileToString(LoadedBoilerplates, *BoilerplateStartUpHPath);
	LoadedBoilerplates = UKismetStringLibrary::Replace(LoadedBoilerplates, "BoilerplateModule", NewModuleName);
	FFileHelper::SaveStringToFile(LoadedBoilerplates, *FString::Printf(TEXT("%s%s%hs"), *PublicPath, *NewModuleName, ".h"));


	//Startup C++ File
	const FString BoilerplateStartUpCPPPath = BoilerplatesPath + "BoilerplateStartUpCPP.txt";
	FFileHelper::LoadFileToString(LoadedBoilerplates, *BoilerplateStartUpCPPPath);
	LoadedBoilerplates = UKismetStringLibrary::Replace(LoadedBoilerplates, "BoilerplateModule", NewModuleName);
	FFileHelper::SaveStringToFile(LoadedBoilerplates, *FString::Printf(TEXT("%s%s%hs"), *PrivatePath, *NewModuleName, ".cpp"));


	//Build File
	const FString BoilerplateModuleBuildPath = BoilerplatesPath + "BoilerplateModule.Build.txt";
	FFileHelper::LoadFileToString(LoadedBoilerplates, *BoilerplateModuleBuildPath);
	LoadedBoilerplates = UKismetStringLibrary::Replace(LoadedBoilerplates, "BoilerplateModule", NewModuleName);
	if (ModuleType != ECPPModuleType::Editor)
	{
		LoadedBoilerplates = UKismetStringLibrary::Replace(LoadedBoilerplates, ", \"UnrealEd\"", "");
	}
	FFileHelper::SaveStringToFile(LoadedBoilerplates, *FString::Printf(TEXT("%s/%s%hs"), *CodePath, *NewModuleName, ".build.cs"));


	//UProject or UPlugin file
	FString DescriptorString;
	const FString UProjectPath = FPaths::GetProjectFilePath();
	const FString UPluginPath = BasePath + PluginName + ".uplugin";

	const FString DescriptorPath = AddToPlugin ? UPluginPath : UProjectPath;
	FFileHelper::LoadFileToString(DescriptorString, *DescriptorPath);

	const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("ECPPModuleType"), true);
	FString ModuleTypeString = EnumPtr->GetDisplayNameTextByIndex(static_cast<int64>(ModuleType)).ToString();
	EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EModuleLoadingPhase"), true);
	FString LoadingPhaseString = EnumPtr->GetDisplayNameTextByIndex(static_cast<int64>(LoadingPhase)).ToString();

	FProjectModuleData NewModuleData = FProjectModuleData();
	NewModuleData.Name = NewModuleName;
	NewModuleData.Type = ModuleTypeString;
	NewModuleData.LoadingPhase = LoadingPhaseString;
	InjectModuleData(DescriptorString, NewModuleData);
	FFileHelper::SaveStringToFile(DescriptorString, *DescriptorPath);

	//.Target.cs
	if(!AddToPlugin)
	{
		const FString ProjectName = UKismetStringLibrary::Replace(FPaths::GetCleanFilename(FPaths::GetProjectFilePath()), ".uproject", "");
		const FString BaseTargetFilePath = BasePath + "/Source/" + ProjectName + ".target.cs";
		const FString BaseEditorTargetFilePath = BasePath + "/Source/" + ProjectName + "Editor.target.cs";

		UE_LOG(LogTemp, Warning, TEXT("Path = %s"), *BaseTargetFilePath);
		
		FString BoilerplateModuleTarget;
		const FString BoilerplateModuleTargetPath = BoilerplatesPath + "BoilerplateTargetFile.txt";
		FFileHelper::LoadFileToString(BoilerplateModuleTarget, *BoilerplateModuleTargetPath);
		BoilerplateModuleTarget = UKismetStringLibrary::Replace(BoilerplateModuleTarget, "BoilerplateModule", NewModuleName);

		InjectExtraModuleNames(BaseTargetFilePath, BoilerplateModuleTarget);
		InjectExtraModuleNames(BaseEditorTargetFilePath, BoilerplateModuleTarget);
	}
}

//TODO:大小写有问题
bool UModuleEditorTool::InjectModuleData(FString& UProjectOrPluginString, const FProjectModuleData& NewModule)
{
	TSharedPtr<FJsonValue> JsonValue;
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(UProjectOrPluginString);

	if (FJsonSerializer::Deserialize(Reader, JsonValue))
	{
		const TSharedPtr<FJsonObject> NewModuleObject = FJsonObjectConverter::UStructToJsonObject(NewModule);
		const TSharedRef<FJsonValueObject> NewModuleValue = MakeShareable(new FJsonValueObject(NewModuleObject));


		const TSharedPtr<FJsonObject> UProjectJSON = JsonValue->AsObject();
		TArray<TSharedPtr<FJsonValue>> Modules = UProjectJSON->GetArrayField("Modules");
		Modules.Add(NewModuleValue);
		UProjectJSON->SetArrayField("Modules", Modules);

		FString OutputString;
		const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
		FJsonSerializer::Serialize(UProjectJSON.ToSharedRef(), Writer);

		UProjectOrPluginString = OutputString;
		return true;
	}
	return false;
}

void UModuleEditorTool::InjectExtraModuleNames(const FString& TargetPath, const FString& ExtraModuleName)
{
	const FRegexPattern myPattern(TEXT("(Type\\s*=\\s*TargetType.*)"));

	FString Target;
	FFileHelper::LoadFileToString(Target, *TargetPath);

	
	FRegexMatcher myMatcher(myPattern, *Target);
	if (myMatcher.FindNext())
	{
		int32 End = myMatcher.GetMatchEnding();
		Target = UKismetStringLibrary::GetSubstring(Target, 0, End) + "\n" + ExtraModuleName + UKismetStringLibrary::GetSubstring(Target, End, Target.Len());
		FFileHelper::SaveStringToFile(Target, *TargetPath);
	}
}

//Copy of GameProjectUtils version as not accessible outside of class
bool UModuleEditorTool::NameContainsOnlyLegalCharacters(const FString& ModuleName, FString& OutIllegalCharacters)
{
	bool bContainsIllegalCharacters = false;

	// Only allow alphanumeric characters in the project name
	bool bFoundAlphaNumericChar = false;
	for (int32 CharIdx = 0; CharIdx < ModuleName.Len(); ++CharIdx)
	{
		const FString& Char = ModuleName.Mid(CharIdx, 1);
		if (!FChar::IsAlnum(Char[0]) && Char != TEXT("_"))
		{
			if (!OutIllegalCharacters.Contains(Char))
			{
				OutIllegalCharacters += Char;
			}

			bContainsIllegalCharacters = true;
		}
	}

	return !bContainsIllegalCharacters;
}