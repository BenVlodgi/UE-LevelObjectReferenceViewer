// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "LevelObjectReferenceViewerStyle.h"

class FLevelObjectReferenceViewerCommands : public TCommands<FLevelObjectReferenceViewerCommands>
{
public:

	FLevelObjectReferenceViewerCommands()
		: TCommands<FLevelObjectReferenceViewerCommands>(TEXT("LevelObjectReferenceViewer"), NSLOCTEXT("Contexts", "LevelObjectReferenceViewer", "LevelObjectReferenceViewer Plugin"), NAME_None, FLevelObjectReferenceViewerStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > OpenPluginWindow;
};