// Copyright Epic Games, Inc. All Rights Reserved.

#include "LevelObjectReferenceViewerCommands.h"

#define LOCTEXT_NAMESPACE "FLevelObjectReferenceViewerModule"

void FLevelObjectReferenceViewerCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "LevelObjectReferenceViewer", "Bring up LevelObjectReferenceViewer window", EUserInterfaceActionType::Button, FInputGesture());
}

#undef LOCTEXT_NAMESPACE
