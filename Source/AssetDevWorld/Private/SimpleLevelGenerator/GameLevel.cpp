// Fill out your copyright notice in the Description page of Project Settings.


#include "SimpleLevelGenerator/GameLevel.h"
#include "SimpleLevelGenerator/Room.h"
#include "SimpleLevelGenerator/RoomDataAsset.h"
//#include "SimpleLevelGenerator/Grid.h"

AGameLevel::AGameLevel()
{
	//avalible in editor.
	//not implemented
	
	//leaves a forshadowing area??
	//creates a new level at runtime.
	bBuildOnPlay = true;

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bAlignEntry = false;
}

AGameLevel::AGameLevel(FVector entryPosition)
{
	//not avaliable in editor.

	//creates new level at runtime.
	bBuildOnPlay = true;

	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bAlignEntry = true;
	m_entryPosition = entryPosition;

}

// Called when the game starts or when spawned
void AGameLevel::BeginPlay()
{
	Super::BeginPlay();

	if(bAlignEntry)
	{
		//if beginning rooms exist and have entry point socket, align socket position with m_entryPosition.
		//bug forseen with intializing this constructor in editor, be advised.
	}

	if (bBuildOnPlay)
	{
		//Grid is stored by reference here.
		//dies out of scope.
		//Grid = &AGrid(MaxWidth, MaxLength);
		Warmup();
		SelectRooms();
		SpawnRooms();
		GenerateNewRooms();
		CarvePassageways();
		ConnectRooms();
		Cleanup();
		Finalize();
	}
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
	//needs to be attached to a periodic event system.
}

