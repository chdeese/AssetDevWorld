// Fill out your copyright notice in the Description page of Project Settings.


#include "SimpleLevelGenerator/Room.h"
#include "SimpleLevelGenerator/EntityDataAsset.h"
#include "SimpleLevelGenerator/Grid.h"

bool ARoom::operator<(ARoom const& rhs)
{
	return BoundsArea < rhs.BoundsArea;
}

bool ARoom::operator>(ARoom const& rhs)
{
	return BoundsArea > rhs.BoundsArea;
}

// Sets default values
ARoom::ARoom()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Chunks = TArray<FGridChunk*>();
}

// Called when the game starts or when spawned
void ARoom::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARoom::SpawnEntity()
{
}

