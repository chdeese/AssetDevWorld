// Fill out your copyright notice in the Description page of Project Settings.


#include "SimpleLevelGenerator/GameLevel.h"
#include "SimpleLevelGenerator/Room.h"
#include "SimpleLevelGenerator/RoomDataAsset.h"
#include "SimpleLevelGenerator/RoomTemplateDataAsset.h"
#include "SimpleLevelGenerator/Grid.h"

AGameLevel::AGameLevel()
{
	//avalible in editor.
	//not implemented
	
	//leaves a forshadowing area??
	//creates a new level at runtime.
	bBuildOnPlay = true;

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RoomsToSpawn = TArray<TSubclassOf<ARoom>>();

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
	RoomsToSpawn = TArray<TSubclassOf<ARoom>>();

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
	//uses int32
	FMath::SRandInit(FDateTime::Now().GetSecond());
}

void AGameLevel::SelectRooms()
{
	float LevelBoundingBoxArea = b3DMode ? 
		MaxWidth * MaxLength * MaxHeight : 
		MaxWidth * MaxLength;
	float PassagewayAreaPercent = 0.25f;
	float EmptyAreaPercent = 0.1f;
	float AssetRoomArea = LevelBoundingBoxArea * Density;

	//subtracts passageway area and empty area from the total room area
	AssetRoomArea -= LevelBoundingBoxArea * PassagewayAreaPercent;
	AssetRoomArea -= LevelBoundingBoxArea * EmptyAreaPercent;
	UniqueRoomArea = AssetRoomArea * UniqueRoomPercent;
	AssetRoomArea -= UniqueRoomArea;

	TSubclassOf<ARoom> RoomClass;
	ARoom* Room;
	float RoomArea;
	float RandomNum;
	for (float CurrentFilledArea = 0; CurrentFilledArea < AssetRoomArea; CurrentFilledArea += RoomArea)
	{
		Room = nullptr;
		RoomArea = 0;
		if (!PriorityRooms.IsEmpty())
		{
			for (auto i = PriorityRooms.begin(); i != PriorityRooms.end(); ++i)
			{
				RoomClass = (*(*i)->RoomAsset);
				//casted only for BoundsArea float value
				Room = Cast<ARoom>(RoomClass->GetDefaultObject());
				RoomArea += Room->BoundsArea;
				RoomsToSpawn.Add(RoomClass);
			}
			Room = nullptr;
			PriorityRooms.Empty();
			continue;
		}

		TArray<TSubclassOf<ARoom>> RoomSelection = TArray<TSubclassOf<ARoom>>();
		if(RoomSelection.IsEmpty())
			RoomSelection = TArray<TSubclassOf<ARoom>>(RoomsToSpawn);

		RandomNum = FMath::SRand();
		RandomNum *= OptionalRooms.Num() - 1;

		RoomSelection.RemoveAt(RandomNum, EAllowShrinking::Yes);

		RoomArea += Room->BoundsArea;
		RoomsToSpawn.Add(RoomClass);
	}
}

//places the largest rooms first
void AGameLevel::SpawnRooms()
{
	// smallest -> largest (by area)
	RoomsToSpawn.Sort();
	for (TSubclassOf<ARoom> Room = RoomsToSpawn.Last(); !RoomsToSpawn.IsEmpty() ; RoomsToSpawn.Remove(Room))
	{
		UClass* actor = TSubclassOf<AActor>();

		ARoom* RoomToSpawn = Room.GetDefaultObject();

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		//needs to be replaced with custom location.
		FVector RoomSpawnLocation = GetActorLocation();

		GetWorld()->SpawnActor<ARoom>(*Room, RoomSpawnLocation, FRotator::ZeroRotator, SpawnParams);
	}
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
	AGameLevel::~AGameLevel();
}

void AGameLevel::Finalize()
{
	//check if priority rooms, a navmesh from start->end, and other stuff exists.
	AGameLevel::Cleanup();
	//other stuff
}

void AGameLevel::UpdateEntitySpawns()
{
	//needs to be attached to a periodic event system.
}

