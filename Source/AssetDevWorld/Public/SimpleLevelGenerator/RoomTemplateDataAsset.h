// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "RoomTemplateDataAsset.generated.h"

class UEntityDataAsset;

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class ASSETDEVWORLD_API URoomTemplateDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> WallAsset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> FloorAsset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> CeilingAsset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> DoorAsset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int SpawnersPerRoom;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UEntityDataAsset*> EntityAssets;

};
