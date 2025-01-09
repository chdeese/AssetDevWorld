// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PassagewayDataAsset.generated.h"

class UEntityDataAsset;
/**
 * 
 */
UCLASS()
class ASSETDEVWORLD_API UPassagewayDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> WallAsset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> FloorAsset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> CeilingAsset;

};
