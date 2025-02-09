// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "NSTActorSpawnGroup.generated.h"

/**
 * 
 */
UCLASS()
class UNSTActorSpawnGroup : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<AActor>> Blueprints;
};
