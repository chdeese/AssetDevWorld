// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RoomDataAsset.h"
#include "RoomStructure.generated.h"

UCLASS()
class ASSETDEVWORLD_API ARoomStructure : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARoomStructure();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<URoomDataAsset*> RoomData;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
