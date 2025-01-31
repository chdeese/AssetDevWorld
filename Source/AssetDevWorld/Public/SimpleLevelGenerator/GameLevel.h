// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameLevel.generated.h"

class ARoom;
class AGrid;
struct FGridIterator;
class URoomDataAsset;
class URoomTemplateDataAsset;
class UPassagewayDataAsset;

UCLASS(Blueprintable, BlueprintType)
class ASSETDEVWORLD_API AGameLevel : public AActor
{
	GENERATED_BODY()

private:
	UPROPERTY()
	bool bAlignEntry;
	UPROPERTY()
	bool bBuildOnPlay;
	UPROPERTY()
	FVector EntryPosition;
	UPROPERTY()
	float UniqueRoomArea;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<URoomDataAsset*> RoomsToSpawn;

	UPROPERTY(BlueprintReadOnly)
	TArray<ARoom*> RoomInstances;

	UPROPERTY(BlueprintReadOnly)
	TArray<TSubclassOf<ARoom>> ReadyRooms;

	UPROPERTY(BlueprintReadOnly)
	AGrid* Grid;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int MaxWidth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int MaxLength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int ChunkRootCM = 150;

public:	
	// creates a new game level
	AGameLevel();

	virtual void BeginPlay() override;

protected:
	UFUNCTION(BlueprintCallable)
	bool IsValidRoomPosition(FBoxSphereBounds Bounds);	\
	UFUNCTION(BlueprintCallable)
	TSubclassOf<AActor> GetRandomRoomWithinChunkCount(FBoxSphereBounds Bounds);
	UFUNCTION(BlueprintCallable)
	void Warmup();
	UFUNCTION(BlueprintCallable)
	void SpawnRooms();
	UFUNCTION(BlueprintCallable)
	void ConnectRooms();
	UFUNCTION(BlueprintCallable)
	void Cleanup();
	UFUNCTION(BlueprintCallable)
	void Finalize();


};
