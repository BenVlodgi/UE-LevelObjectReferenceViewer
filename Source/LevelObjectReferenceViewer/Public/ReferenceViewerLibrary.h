// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ReferenceViewerLibrary.generated.h"

/**
 * 
 */
UCLASS()
class LEVELOBJECTREFERENCEVIEWER_API UReferenceViewerLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable, Category = "Reference Viewer")
	void GetReferencesToSelected(AActor* Actor, TArray<AActor*>& ReferencingActors, TArray<UObject*>& SoftReferencingObjects);
	
};
