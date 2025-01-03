// Fill out your copyright notice in the Description page of Project Settings.


#include "SimpleLevelGenerator/GameLevel.h"
#include "SimpleLevelGenerator/Room.h"
#include "SimpleLevelGenerator/RoomDataAsset.h"

// Sets default values
AGameLevel::AGameLevel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AGameLevel::BeginPlay()
{
	Super::BeginPlay();
}


void AGameLevel::Warmup()
{

}

void AGameLevel::SelectRooms()
{

}

void AGameLevel::SpawnRooms()
{

}

void AGameLevel::GenerateNewRooms()
{

}

void AGameLevel::CarvePassageways()
{

}

void AGameLevel::ConnectRooms()
{

}

void AGameLevel::Cleanup()
{

}

void AGameLevel::Finalize()
{

}

void AGameLevel::UpdateEntitySpawns()
{

}

