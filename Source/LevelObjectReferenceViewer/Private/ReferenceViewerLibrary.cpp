// Fill out your copyright notice in the Description page of Project Settings.

#include "ReferenceViewerLibrary.h"

#include "AssetToolsModule.h"
#include "Editor/GroupActor.h"
#include "Engine/LevelScriptActor.h"
#include "Misc/ScopedSlowTask.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Layers/LayersSubsystem.h"


void UReferenceViewerLibrary::GetReferencesToSelected(AActor* Actor, TArray<AActor*>& ReferencingActors, TArray<UObject*>& SoftReferencingObjects)
{
	/*
	UWorld* InWorld = Actor->GetWorld();

	bool bCheckReferences = true;
	bool bCheckSoftReferences = true;

	const double StartSeconds = FPlatformTime::Seconds();

	TArray<AActor*> ActorsToCheck;

	TMap<AActor*, TArray<AActor*>> ReferencingActorsMap;
	TMap<AActor*, TArray<UObject*>> SoftReferencingObjectsMap;

	TArray<UClass*> ClassTypesToIgnore;
	ClassTypesToIgnore.Add(ALevelScriptActor::StaticClass());
	ClassTypesToIgnore.Add(AGroupActor::StaticClass());

	// It is a lot more efficient to query the world first and build a map of actors referenced by other actors. We can then quickly look this up later on in the loop.
	if (bCheckReferences)
	{
		FBlueprintEditorUtils::GetActorReferenceMap(InWorld, ClassTypesToIgnore, ReferencingActorsMap);

		if (bCheckSoftReferences)
		{
			FScopedSlowTask SlowTask(ActorsToCheck.Num(), LOCTEXT("ComputeActorSoftReferences", "Computing References"));
			SlowTask.MakeDialogDelayed(1.0f);

			FAssetToolsModule& AssetToolsModule = FModuleManager::GetModuleChecked<FAssetToolsModule>(TEXT("AssetTools"));

			for (int32 ActorIndex = 0; ActorIndex < ActorsToCheck.Num(); ++ActorIndex)
			{
				SlowTask.EnterProgressFrame();

				TArray<UObject*> SoftReferencingObjects;
				AActor* Actor = ActorsToCheck[ActorIndex];

				AssetToolsModule.Get().FindSoftReferencesToObject(Actor, SoftReferencingObjects);

				if (SoftReferencingObjects.Num() > 0)
				{
					SoftReferencingObjectsMap.Add(Actor, SoftReferencingObjects);
				}
			}
		}
	}

	ULayersSubsystem* LayersSubsystem = GEditor->GetEditorSubsystem<ULayersSubsystem>();
	for (int32 ActorIndex = 0; ActorIndex < ActorsToCheck.Num(); ++ActorIndex)
	{
		AActor* Actor = ActorsToCheck[ActorIndex];

		//If actor is referenced by script, ask user if they really want to delete
		ULevelScriptBlueprint* LSB = Actor->GetLevel()->GetLevelScriptBlueprint(true);

		// Get the array of actors that reference this actor from the cached map we built above.
		TArray<AActor*>* ReferencingActors = nullptr;
		if (bCheckReferences)
		{
			ReferencingActors = ReferencingActorsMap.Find(Actor);
		}

		TArray<UK2Node*> ReferencedToActorsFromLevelScriptArray;
		FBlueprintEditorUtils::FindReferencesToActorFromLevelScript(LSB, Actor, ReferencedToActorsFromLevelScriptArray);

		bool bReferencedByLevelScript = bCheckReferences && (nullptr != LSB && ReferencedToActorsFromLevelScriptArray.Num() > 0);
		bool bReferencedByActor = false;
		bool bReferencedByLODActor = false;
		bool bReferencedBySoftReference = false;
		TArray<UObject*>* SoftReferencingObjects = nullptr;

		if (bCheckSoftReferences)
		{
			SoftReferencingObjects = SoftReferencingObjectsMap.Find(Actor);

			if (SoftReferencingObjects)
			{
				bReferencedBySoftReference = true;
			}
		}

		// If there are any referencing actors, make sure that they are reference types that we care about.
		if (ReferencingActors != nullptr)
		{
			for (AActor* ReferencingActor : (*ReferencingActors))
			{
				// Skip to next if we are referencing ourselves
				if (ReferencingActor == Actor)
				{
					continue;
				}
				else if (Cast<ALODActor>(ReferencingActor))
				{
					bReferencedByLODActor = true;
				}
				else
				{
					// If the referencing actor is a child actor that is referencing us, do not treat it
					// as referencing for the purposes of warning about deletion
					UChildActorComponent* ParentComponent = ReferencingActor->GetParentComponent();
					if (ParentComponent == nullptr || ParentComponent->GetOwner() != Actor)
					{
						bReferencedByActor = true;

						FText ActorReferencedMessage = FText::Format(LOCTEXT("ActorDeleteReferencedMessage", "Actor {0} is referenced by {1}."),
							FText::FromString(Actor->GetActorLabel()),
							FText::FromString(ReferencingActor->GetActorLabel())
						);
						UE_LOG(LogEditorActor, Log, TEXT("%s"), *ActorReferencedMessage.ToString());
					}
				}
			}
		}

		// We have references from one or more sources, prompt the user for feedback.
		if (bReferencedByLevelScript || bReferencedByActor || bReferencedBySoftReference || bReferencedByLODActor)
		{
			{
				FText ConfirmDelete;

				FString LevelScriptReferenceString;

				for (int32 i = 0; i < ReferencedToActorsFromLevelScriptArray.Num(); ++i)
				{
					LevelScriptReferenceString += ReferencedToActorsFromLevelScriptArray[i]->GetFindReferenceSearchString();

					if (bReferencedByLevelScript && bReferencedByActor)
					{
						LevelScriptReferenceString += TEXT(" (Level Blueprint)");
					}

					LevelScriptReferenceString += TEXT("\n");
				}

				LevelScriptReferenceString.TrimEndInline();

				FString ActorReferenceString;

				if (ReferencingActors != nullptr)
				{
					for (int32 i = 0; i < ReferencingActors->Num(); ++i)
					{
						ActorReferenceString += (*ReferencingActors)[i]->GetActorLabel();

						if (bReferencedByLevelScript && bReferencedByActor)
						{
							ActorReferenceString += TEXT(" (Other Actor)");
						}

						ActorReferenceString += TEXT("\n");
					}
				}

				if (bReferencedBySoftReference)
				{
					for (UObject* ReferencingObject : *SoftReferencingObjects)
					{
						if (AActor* ReferencingActor = Cast<AActor>(ReferencingObject))
						{
							ActorReferenceString += FString::Printf(TEXT("(Soft) Actor %s in %s\n"), *ReferencingActor->GetActorLabel(), *FPackageName::GetLongPackageAssetName(ReferencingActor->GetOutermost()->GetName()));
						}
						else
						{
							ActorReferenceString += FString::Printf(TEXT("(Soft) Object %s\n"), *ReferencingObject->GetPathName());
						}
					}
				}
			}
		}
	}

	UE_LOG(LogEditorActor, Log, TEXT("Deleted %d Actors (%3.3f secs)"), DeleteCount, (FPlatformTime::Seconds() - StartSeconds));

	//*/
}