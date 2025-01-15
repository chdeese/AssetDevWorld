// Fill out your copyright notice in the Description page of Project Settings.


#include "SimpleLevelGenerator/GameLevel.h"
#include "SimpleLevelGenerator/Room.h"
#include "SimpleLevelGenerator/RoomDataAsset.h"
#include "SimpleLevelGenerator/RoomTemplateDataAsset.h"
#include "SimpleLevelGenerator/PassagewayDataAsset.h"
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

	bAlignEntry = false;
	RoomsToSpawn = TArray<TSubclassOf<ARoom>>();
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

	if (bBuildOnPlay)
	{
		UClass* GridClass = CreateDefaultSubobject<UClass, AGrid>(TEXT("AGrid"), false);
		FActorSpawnParameters Params = FActorSpawnParameters();
		Params.Owner = this;

		Grid = GetWorld()->SpawnActor<AGrid>(GridClass, GetActorLocation(), FRotator::ZeroRotator, Params);

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

FGridIterator* AGameLevel::GetIterator()
{
	return Grid->GetIterator();
}


FVector AGameLevel::GetRandomBorderAlignedRoomPosition(float Width, float Length)
{
	FVector RandomDirection = FVector(FMath::Rand32(), FMath::Rand32(), FMath::Rand32());
	RandomDirection = RandomDirection.GetSafeNormal();
	FVector RandomDirectionAbs = RandomDirection;
	if (RandomDirectionAbs.X < 0)
		RandomDirectionAbs.X = -RandomDirectionAbs.X;
	if (RandomDirectionAbs.Y < 0)
		RandomDirectionAbs.Y = -RandomDirectionAbs.Y;
	if (RandomDirectionAbs.Z < 0)
		RandomDirectionAbs.Z = -RandomDirectionAbs.Z;

	float Min = GetActorLocation().X - ((MaxWidth - Width) / 2);
	float Max = GetActorLocation().X + ((MaxWidth - Width) / 2);
	float RandomXPosLevelAligned = FMath::RandRange(Min, Max);
	Min = GetActorLocation().Y - ((MaxLength - Length) / 2);
	Max = GetActorLocation().Y + ((MaxLength - Length) / 2);
	float RandomYPosLevelAligned = FMath::RandRange(Min, Max);
	if (RandomDirection.X > RandomDirection.Y)
		if (RandomDirection.X > 0)
			RandomXPosLevelAligned = GetActorLocation().X + ((MaxWidth - Width) / 2);
		else
			RandomXPosLevelAligned = GetActorLocation().X - ((MaxWidth - Width) / 2);
	else
		if (RandomDirection.Y > 0)
			RandomYPosLevelAligned = GetActorLocation().X + ((MaxLength - Length) / 2);
		else
			RandomYPosLevelAligned = GetActorLocation().X - ((MaxLength - Length) / 2);

	return GetActorLocation() + FVector(RandomXPosLevelAligned, RandomYPosLevelAligned, 0);
}

void AGameLevel::Warmup()
{
}

void AGameLevel::SelectRooms()
{
	float LevelBoundingBoxArea = b3DMode ? 
		MaxWidth * MaxLength * MaxHeight : 
		MaxWidth * MaxLength;
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
			for (URoomDataAsset* PriorityRoomData : PriorityRooms)
			{
				RoomClass = PriorityRoomData->RoomAsset;
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

		RandomNum = FMath::RandRange(0, OptionalRooms.Num() - 1);

		RoomSelection.RemoveAt(RandomNum, EAllowShrinking::Yes);

		RoomArea += Room->BoundsArea;
		RoomsToSpawn.Add(RoomClass);
	}
}

//places the largest rooms first
void AGameLevel::SpawnRooms()
{
	//use vector math to place beginning room on border facing outwards, then get entry socket.
	int RandomNum = FMath::RandRange(0, BeginningRooms.Num() - 1);
	FVector Offset = FVector::ZeroVector;
	FActorSpawnParameters Params = FActorSpawnParameters();
	FVector BeginningRoomOrigin;
	FVector BeginningRoomBoxExtent;
	GetActorBounds(true, BeginningRoomOrigin, BeginningRoomBoxExtent, true);

	ARoom* BeginningRoom;
	if (!BeginningRooms.IsEmpty())
	{
		BeginningRoom = GetWorld()->SpawnActor<ARoom>(BeginningRooms[RandomNum]->RoomAsset, GetRandomBorderAlignedRoomPosition(BeginningRoomBoxExtent.X * 2, BeginningRoomBoxExtent.Y * 2), FRotator::ZeroRotator, Params);
		RoomInstances.Add(BeginningRoom);
	}

	// smallest -> largest (by area)
	RoomsToSpawn.Sort();
	for (TSubclassOf<ARoom> Room = RoomsToSpawn.Last(); !RoomsToSpawn.IsEmpty(); RoomsToSpawn.Remove(Room))
	{
		ARoom* RoomToSpawn = Room.GetDefaultObject();

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		//needs to be replaced with custom location.
		FVector RoomSpawnLocation = GetActorLocation();

		ARoom* SpawnedRoom = GetWorld()->SpawnActor<ARoom>(*Room, RoomSpawnLocation, FRotator::ZeroRotator, SpawnParams);

		//setup socket locations.
		

		RoomInstances.Add(SpawnedRoom);
	}
}

void AGameLevel::GenerateNewRooms()
{
	//float GeneratedRoomArea;
	//float RoomArea;
	//for (float CurrentFilledArea = 0; CurrentFilledArea < GeneratedRoomArea; CurrentFilledArea += RoomArea)
	//{

	//}
}

void AGameLevel::CarvePassageways()
{
	//float GeneratedRoomArea;
	//float RoomArea;
	//for (float CurrentFilledArea = 0; CurrentFilledArea < GeneratedRoomArea; CurrentFilledArea += RoomArea)
	//{

	//}
}

void AGameLevel::ConnectRooms()
{
	//get entry point chunk and connect with other chunk.
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

