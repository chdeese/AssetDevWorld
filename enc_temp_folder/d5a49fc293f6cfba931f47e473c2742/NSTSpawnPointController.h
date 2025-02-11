// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NSTSpawnPointController.generated.h"

class ANSTSpawnPoint;
class UNSTActorSpawnGroup;

UCLASS()
class ANSTSpawnPointController : public AActor
{
	GENERATED_BODY()
	
public:	
	UPROPERTY(EditAnywhere)
	TArray<UNSTActorSpawnGroup*> SpawnGroups;

	UPROPERTY(EditAnywhere)
	TArray<ANSTSpawnPoint*> SpawnPoints;

	UPROPERTY(EditAnywhere)
	float ForcedCooldownLength;

public:	
	UFUNCTION(CallInEditor, Category = "NSTSpawnPointController")
	void CombineAllGroups();

	UFUNCTION(CallInEditor, Category = "NSTSpawnPointController")
	void StopAllRepeating();	
	
	UFUNCTION(CallInEditor, Category = "NSTSpawnPointController")
	void StopAllRepeatSelection();

	UFUNCTION(CallInEditor, Category = "NSTSpawnPointController")
	void ForceAllRepeat();

	UFUNCTION(CallInEditor, Category = "NSTSpawnPointController")
	void ForceAllRepeatSelection();

	UFUNCTION(CallInEditor, Category = "NSTSpawnPointController")
	void ForceSetAllCooldowns();
};
