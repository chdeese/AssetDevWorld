// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameLevel.generated.h"

class ARoom;
class AGrid;
class URoomDataAsset;

UCLASS()
class ASSETDEVWORLD_API AGameLevel : public AActor
{
	GENERATED_BODY()
	
private:
	bool bAlignEntry;
	bool bBuildOnPlay;
	FVector m_entryPosition;
	float UniqueRoomArea;
	TArray<ARoom*> RoomsToSpawn;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<ARoom*> RoomInstances;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UPrimaryDataAsset*> RoomData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<URoomDataAsset*> BeginningRooms;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<URoomDataAsset*> PriorityRooms;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AGrid* Grid;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int MaxActiveEntities;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Density;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int MaxWidth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int MaxLength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bSpawnUniqueRooms;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float UniqueRoomPercent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bFlatMode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool b3DMode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int MaxHeight;

public:	
	// creates a new game level
	AGameLevel();

	AGameLevel(FVector entryPosition);

	virtual void BeginPlay() override;

protected:
	UFUNCTION(BlueprintCallable)
	void Warmup();
	UFUNCTION(BlueprintCallable)
	void SelectRooms();
	UFUNCTION(BlueprintCallable)
	void SpawnRooms();
	UFUNCTION(BlueprintCallable)
	void GenerateNewRooms();
	UFUNCTION(BlueprintCallable)
	void CarvePassageways();
	UFUNCTION(BlueprintCallable)
	void ConnectRooms();
	UFUNCTION(BlueprintCallable)
	void Cleanup();
	UFUNCTION(BlueprintCallable)
	void Finalize();
	UFUNCTION(BlueprintCallable)
	void UpdateEntitySpawns();

};
