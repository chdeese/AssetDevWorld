// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Room.generated.h"

class UEntityDataAsset;
struct FGridChunk;

UCLASS(Placeable)
class ASSETDEVWORLD_API ARoom : public AActor
{
	GENERATED_BODY()
	
private:
	TArray<FGridChunk*> Chunks;
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<AActor*> OwnedActiveEntities;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UEntityDataAsset*> EntityData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bGenerated;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LastSpawnSecondsElapsed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FVector> SpawnerLocations;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FVector> DoorLocations;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bRepeatSpawns;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SpawnCooldownSeconds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BoundsArea;

	bool operator<(ARoom const& rhs);

	bool operator>(ARoom const& rhs);

public:	
	// Sets default values for this actor's properties
	ARoom();

	void BeginPlay() override;

public:
	UFUNCTION()
	void SpawnEntity();
};
