// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "RoomTemplateDataAsset.generated.h"

class UEntityDataAsset;

/**
 * 
 */
UCLASS()
class ASSETDEVWORLD_API URoomTemplateDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* WallAsset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* FloorAsset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* CeilingAsset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* DoorAsset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int SpawnersPerRoom;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UEntityDataAsset*> EntityAssets;

};
