// Fill out your copyright notice in the Description page of Project Settings.


#include "RoomStructure.h"

// Sets default values
ARoomStructure::ARoomStructure()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ARoomStructure::BeginPlay()
{
	Super::BeginPlay();
	
	int Size = RoomData.Num();

	if (Size <= 0)
	{
		return;
	}

	UClass* room = RoomData.Last()->RoomClass.Get();

	if (!room)
	{
		return;
	}

	FActorSpawnParameters spawnParams = FActorSpawnParameters();
	spawnParams.Owner = this;

	FTransform spawnTransform = FTransform();
	GetWorld()->SpawnActor<AActor>(room, spawnTransform, spawnParams);
}

// Called every frame
void ARoomStructure::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

