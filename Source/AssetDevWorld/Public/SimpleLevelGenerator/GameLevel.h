// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameLevel.generated.h"

class ARoom;
class URoomDataAsset;

UCLASS()
class ASSETDEVWORLD_API AGameLevel : public AActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<ARoom*> RoomInstances;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UPrimaryDataAsset*> RoomData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<URoomDataAsset*> BeginningRooms;

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AGrid Grid;*/

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
	bool bFlatMode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool b3DMode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int MaxHeight;

public:	
	// creates a new game level
	AGameLevel();

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
