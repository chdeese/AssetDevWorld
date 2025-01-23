// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "RoomDataAsset.generated.h"


/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class ASSETDEVWORLD_API URoomDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> RoomAsset;
};
