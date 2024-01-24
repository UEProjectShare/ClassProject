// Copyright 2021 Sam Carey. All Rights Reserved.

#include "ModuleGenerator.h"
#include "ModuleEditorTool.h"
#include "LevelEditor.h"
#include "ModuleGeneratorCommands.h"
#include "ModuleGeneratorToolDetails.h"
#include "ToolMenus.h"

#define LOCTEXT_NAMESPACE "FModuleGeneratorModule"

void FModuleGeneratorModule::StartupModule()
{
	//Init Commands
	FModuleGeneratorCommands::Register();
	PluginCommands = MakeShareable(new FUICommandList);
	PluginCommands->MapAction(
		FModuleGeneratorCommands::Get().OpenModuleGeneratorTool,
		FExecuteAction::CreateRaw(this, &FModuleGeneratorModule::OnOpenTool),
		FCanExecuteAction());
	
	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FModuleGeneratorModule::RegisterMenus));
	
	//Init Style
	FModuleGeneratorStyle::Initialize();
	FModuleGeneratorStyle::ReloadTextures();
	
	//Register Custom Details for tool
	FPropertyEditorModule& PM = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PM.RegisterCustomClassLayout("ModuleEditorTool", FOnGetDetailCustomizationInstance::CreateStatic(FModuleGeneratorToolDetails::MakeInstance));
	PM.NotifyCustomizationModuleChanged();

}

void FModuleGeneratorModule::ShutdownModule()
{
	FModuleGeneratorStyle::Shutdown();
	FPropertyEditorModule& PM = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PM.UnregisterCustomClassLayout("ModuleEditorTool");
}

void FModuleGeneratorModule::OnOpenTool()
{
	if(!WindowCreated)
	{
		UModuleEditorTool* ToolInstance = NewObject<UModuleEditorTool>(GetTransientPackage(), UModuleEditorTool::StaticClass());

		//·ÀÖ¹GC
		ToolInstance->AddToRoot();

		FPropertyEditorModule& PM = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

		TArray<UObject*> ObjectsToView;
		ObjectsToView.Add(ToolInstance);
		const TSharedRef<SWindow> Window = PM.CreateFloatingDetailsView(ObjectsToView, false);
		Window->SetTitle(FText::FromString("Create New Module"));
		Window->SetOnWindowClosed(FOnWindowClosed::CreateRaw(this, &FModuleGeneratorModule::ToolWindowClosed));
		WindowCreated = true;
	}
}

void FModuleGeneratorModule::ToolWindowClosed(const TSharedRef<SWindow>&)
{
	WindowCreated = false;
}

void FModuleGeneratorModule::RegisterMenus()
{
	FToolMenuOwnerScoped OwnerScoped(this);

	UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
	{
		FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
		Section.AddMenuEntryWithCommandList(FModuleGeneratorCommands::Get().OpenModuleGeneratorTool, PluginCommands);
	}

	UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
	{
		FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("Game");
		{
			FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FModuleGeneratorCommands::Get().OpenModuleGeneratorTool));
			Entry.SetCommandList(PluginCommands);
		}
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FModuleGeneratorModule, ModuleGenerator)