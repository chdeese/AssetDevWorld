// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Room.generated.h"

class UEntityDataAsset;
struct FGridChunk;

UCLASS()
class ASSETDEVWORLD_API ARoom : public AActor
{
	GENERATED_BODY()
	
public:
	TArray<FGridChunk*> Chunks;
	UPROPERTY()
	float BoundsArea = 0;

	int ChunkRootCM = 150;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int ChunkCountWidth;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int ChunkCountLength;

	UFUNCTION(BlueprintCallable)
	void SetSocketLocation(FVector& ReturnVector, FName SocketName);
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<AActor*> OwnedActiveEntities;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UEntityDataAsset*> EntityData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bGenerated = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LastSpawnSecondsElapsed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FVector> SpawnerLocations;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FVector> DoorLocations;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector EntryLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bRepeatSpawns = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SpawnCooldownSeconds = 5.0f;

	bool operator<(ARoom const& rhs);

	bool operator>(ARoom const& rhs);

public:	
	// Sets default values for this actor's properties
	ARoom();

	void BeginPlay() override;

public:
	UFUNCTION()
	void SpawnEntity();
	UFUNCTION()
	void CaptureSockets();
	UFUNCTION()
	float GetBoundsArea();

};
