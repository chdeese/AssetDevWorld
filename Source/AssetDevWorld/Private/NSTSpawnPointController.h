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

	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_AppendGroups)
	bool bSingleAppendedGroup;

	bool bTryAppendGroups = false;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Sets default values for this actor's properties
	ANSTSpawnPointController();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnRep_AppendGroups();

	UFUNCTION()
	bool AppendGroups();
};
