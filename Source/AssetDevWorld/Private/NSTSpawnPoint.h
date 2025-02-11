// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NSTSpawnPoint.generated.h"

class UNSTActorSpawnGroup;

UCLASS()
class ANSTSpawnPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANSTSpawnPoint();

	UPROPERTY(EditAnywhere)
	UNSTActorSpawnGroup* ActorBlueprints;

	UPROPERTY(EditAnywhere)
	bool bRepeatSpawns = false;

	UPROPERTY(EditAnywhere)
	bool bRepeatSelect = false;

	UPROPERTY(EditAnywhere)
	float RepeatCooldown = 5.0f;

	UPROPERTY(BlueprintReadOnly)
	float ElapsedSinceSpawn;

	UPROPERTY(BlueprintReadOnly)
	int ActorSpawnIndex = -1;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UFUNCTION()
	bool SpawnActor();

};
