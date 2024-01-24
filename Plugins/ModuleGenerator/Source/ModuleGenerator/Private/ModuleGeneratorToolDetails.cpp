// Copyright 2021 Sam Carey. All Rights Reserved.

#include "ModuleGeneratorToolDetails.h"
#include "DetailCategoryBuilder.h"
#include "DetailLayoutBuilder.h"
#include "ModuleEditorTool.h"
#include "IDetailGroup.h"
#include "Misc/MessageDialog.h"
#include "Widgets/Layout/SBorder.h"
#include "PropertyCustomizationHelpers.h"
#include "EditorStyleSet.h"
#include "Components/SlateWrapperTypes.h"
#include "Interfaces/IPluginManager.h"
#include "Kismet/KismetSystemLibrary.h"

void FModuleGeneratorToolDetails::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	//Find instance to tool being customised 
	TArray<TWeakObjectPtr<UObject>> ObjectsBeingCustomized;
	DetailBuilder.GetObjectsBeingCustomized(ObjectsBeingCustomized);
	for (int32 ObjectIndex = 0; !Tool.IsValid() &&ObjectIndex < ObjectsBeingCustomized.Num(); ++ObjectIndex)
	{
		const TWeakObjectPtr<UObject>& CurrentObject = ObjectsBeingCustomized[ObjectIndex];
		if (CurrentObject.IsValid())
		{
			Tool = Cast<UModuleEditorTool>(CurrentObject.Get());
		}
	}

	const FText FilterString = FText::FromString("Module Generator");
	IDetailCategoryBuilder& Category = DetailBuilder.EditCategory("New Module");

	
	//Base Props
	const TSharedPtr<IPropertyHandle> NewNameInput = Category.AddProperty("NewModuleName").GetPropertyHandle();
	NewNameInput.Get()->SetOnPropertyValueChanged(FSimpleDelegate::CreateRaw(this, &FModuleGeneratorToolDetails::OnChangeNewModuleName));
	NewNameInput.Get()->SetOnPropertyResetToDefault(FSimpleDelegate::CreateRaw(this, &FModuleGeneratorToolDetails::OnChangeNewModuleName));
	
	//Name Error Test
	Category.AddCustomRow(FilterString)
		.WholeRowContent()
		[
			SNew(SBorder)
			.Padding(5)
			.Visibility(this, &FModuleGeneratorToolDetails::GetNameErrorLabelVisibility)
			.BorderImage(FAppStyle::GetBrush("NewClassDialog.ErrorLabelBorder"))
			.Content()
			[
				SNew(STextBlock)
				.Text(this, &FModuleGeneratorToolDetails::GetNameErrorLabelText)
				.TextStyle(FAppStyle::Get(), "NewClassDialog.ErrorLabelFont")
			]
		];
	
	Category.AddProperty("ModuleTemplate");
	Category.AddProperty("ModuleType");
	Category.AddProperty("LoadingPhase");
	Category.AddCustomRow(FilterString)
		.WholeRowContent()
		[
			SNew(SSpacer)
		];


	//Plugin Props
	FillComboBoxWithPluginNames();
	
	Category.AddProperty("AddToPlugin").Visibility(Options.Num() > 0 ? EVisibility::Visible : EVisibility::Collapsed);
	if(Options.Num() > 0)
	{
		CurrentSelectedPlugin = Options[0];
		Tool->PluginName = *CurrentSelectedPlugin.Get();
	}
	FDetailWidgetRow a = Category.AddCustomRow(FilterString)
		.ValueContent()
		[
			SNew(SComboBox<FComboItemType>)
			.OptionsSource(&Options)
			.OnSelectionChanged(this, &FModuleGeneratorToolDetails::OnSelectionChanged)
			.OnGenerateWidget(this, &FModuleGeneratorToolDetails::MakeWidgetForOption)
			.InitiallySelectedItem(CurrentSelectedPlugin)
			[
				SNew(STextBlock)
				.Text(this, &FModuleGeneratorToolDetails::GetCurrentItemLabel)
			]
			.Visibility(this, &FModuleGeneratorToolDetails::GetBasicParentClassVisibility)
		]
		.NameContent()
		[
			SNew(STextBlock)
			.Text(FText::FromString("Plugin"))
			.Visibility(this, &FModuleGeneratorToolDetails::GetBasicParentClassVisibility)
		];

	Category.AddCustomRow(FilterString)
		.WholeRowContent()
		[
			SNew(SBox).HeightOverride(FOptionalSize(5))
		];
	
	Category.AddCustomRow(FilterString)
		.WholeRowContent()
		[
			SNew(SButton)
			.Text(FText::FromString("Generate New Module")).HAlign(HAlign_Center)
			.OnClicked_Raw(this, &FModuleGeneratorToolDetails::GenerateNewModuleCall)
		];

	//Bind Template change event
	const TSharedRef<IPropertyHandle> Template = DetailBuilder.GetProperty("ModuleTemplate");
	Template->SetOnPropertyValueChanged(FSimpleDelegate::CreateRaw(this, &FModuleGeneratorToolDetails::OnTemplateChange));
}

TSharedRef<IDetailCustomization> FModuleGeneratorToolDetails::MakeInstance()
{
	return MakeShareable(new FModuleGeneratorToolDetails);
}

void FModuleGeneratorToolDetails::OnTemplateChange() const
{
	//Update ModuleTemplate and ModuleType to match template
	if (Tool->ModuleTemplate == EModuleTemplates::Runtime)
	{
		Tool->ModuleType = ECPPModuleType::Runtime;
		Tool->LoadingPhase = EModuleLoadingPhase::Default;
	}
	else if (Tool->ModuleTemplate == EModuleTemplates::Editor)
	{
		Tool->ModuleType = ECPPModuleType::Editor;
		Tool->LoadingPhase = EModuleLoadingPhase::PostEngineInit;
	}
}

void FModuleGeneratorToolDetails::FillComboBoxWithPluginNames()
{
	//Class to search for all plugins included within current project 
	class FSubDirectoriesAtPath : public IPlatformFile::FDirectoryVisitor
	{
	public:
		FSubDirectoriesAtPath() {}
		
		virtual bool Visit(const TCHAR* FilenameOrDirectory, bool bIsDirectory) override
		{
			if (bIsDirectory)
			{
				//TODO:插件搜索有问题
				//if(const TSharedPtr<IPlugin> PluginToAddTo = IPluginManager::Get().FindPlugin(FPaths::GetCleanFilename(FilenameOrDirectory)))
				{
					Directories.Add(MakeShareable(new FString(FPaths::GetCleanFilename(FilenameOrDirectory))));
				}
			}
			return true;
		}
		
		TArray<FComboItemType> Directories;
		
		FString PluginFolderPath;
	};

	const FString PluginFolder = UKismetSystemLibrary::GetProjectDirectory() + TEXT("Plugins");
	FSubDirectoriesAtPath Visitor;
	Visitor.PluginFolderPath = PluginFolder;
	FPlatformFileManager::Get().GetPlatformFile().IterateDirectory(*PluginFolder, Visitor);
	Options = Visitor.Directories;
}

FReply FModuleGeneratorToolDetails::GenerateNewModuleCall() const
{
	if(Tool.IsValid())
	{
		FString ErrorMessage;
		bool Validate = Tool->ValidateInput(ErrorMessage);
		//If input is valid and module has been created

		if(!ErrorMessage.IsEmpty())
		{
			const FText Title = FText::FromString("Failed to Create Module");
			FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(ErrorMessage), &Title);
		}
		else
		{
			Tool->GenerateModule();
			const FText Title = FText::FromString("Module Creation Complete");
			const auto Result = FMessageDialog::Open(EAppMsgType::YesNo, FText::FromString(FString::Printf(TEXT("A new module with the name \"%s\" was created. You must restart the editor for it to appear. \nDo you want to restart now?"), *Tool->NewModuleName)), &Title);
			if (Result == EAppReturnType::Yes)
			{
				FUnrealEdMisc::Get().RestartEditor(false);
			}
		}
	}
	return FReply::Handled();
}

TSharedRef<SWidget> FModuleGeneratorToolDetails::MakeWidgetForOption(FComboItemType InOption)
{
	return SNew(STextBlock).Text(FText::FromString(*InOption));
}

FText FModuleGeneratorToolDetails::GetCurrentItemLabel() const
{
	if (CurrentSelectedPlugin.IsValid())
	{
		return FText::FromString(*CurrentSelectedPlugin);
	}

	return FText::FromString("Invalid Selection");
}

void FModuleGeneratorToolDetails::OnSelectionChanged(FComboItemType NewValue, ESelectInfo::Type)
{
	CurrentSelectedPlugin = NewValue;
	if(Tool.IsValid())
	{
		Tool->PluginName = *CurrentSelectedPlugin.Get();
	}
}

EVisibility FModuleGeneratorToolDetails::GetBasicParentClassVisibility() const
{
	return !Tool->AddToPlugin ? EVisibility::Collapsed : EVisibility::Visible;
}

EVisibility FModuleGeneratorToolDetails::GetNameErrorLabelVisibility() const
{
	return WasError ? EVisibility::Visible : EVisibility::Hidden;
}

FText FModuleGeneratorToolDetails::GetNameErrorLabelText() const
{
	return FText::FromString(LastErrorText);
}

void FModuleGeneratorToolDetails::OnChangeNewModuleName()
{
	WasError = !Tool->ValidateInput(LastErrorText);
}