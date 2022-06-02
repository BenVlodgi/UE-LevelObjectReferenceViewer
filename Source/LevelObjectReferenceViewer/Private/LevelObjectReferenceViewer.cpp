// Copyright Epic Games, Inc. All Rights Reserved.

#include "LevelObjectReferenceViewer.h"
#include "LevelObjectReferenceViewerStyle.h"
#include "LevelObjectReferenceViewerCommands.h"
#include "LevelEditor.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "ToolMenus.h"

static const FName LevelObjectReferenceViewerTabName("LevelObjectReferenceViewer");

#define LOCTEXT_NAMESPACE "FLevelObjectReferenceViewerModule"

void FLevelObjectReferenceViewerModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FLevelObjectReferenceViewerStyle::Initialize();
	FLevelObjectReferenceViewerStyle::ReloadTextures();

	FLevelObjectReferenceViewerCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FLevelObjectReferenceViewerCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FLevelObjectReferenceViewerModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FLevelObjectReferenceViewerModule::RegisterMenus));
	
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(LevelObjectReferenceViewerTabName, FOnSpawnTab::CreateRaw(this, &FLevelObjectReferenceViewerModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("FLevelObjectReferenceViewerTabTitle", "LevelObjectReferenceViewer"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);
}

void FLevelObjectReferenceViewerModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FLevelObjectReferenceViewerStyle::Shutdown();

	FLevelObjectReferenceViewerCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(LevelObjectReferenceViewerTabName);
}

TSharedRef<SDockTab> FLevelObjectReferenceViewerModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	FText WidgetText = FText::Format(
		LOCTEXT("WindowWidgetText", "Add code to {0} in {1} to override this window's contents"),
		FText::FromString(TEXT("FLevelObjectReferenceViewerModule::OnSpawnPluginTab")),
		FText::FromString(TEXT("LevelObjectReferenceViewer.cpp"))
		);

	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			// Put your tab content here!
			SNew(SBox)
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(WidgetText)
			]
		];
}

void FLevelObjectReferenceViewerModule::PluginButtonClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(LevelObjectReferenceViewerTabName);
}

void FLevelObjectReferenceViewerModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FLevelObjectReferenceViewerCommands::Get().OpenPluginWindow, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("Settings");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FLevelObjectReferenceViewerCommands::Get().OpenPluginWindow));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FLevelObjectReferenceViewerModule, LevelObjectReferenceViewer)