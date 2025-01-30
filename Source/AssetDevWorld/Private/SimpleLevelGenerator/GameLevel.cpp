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
}



// Called when the game starts or when spawned
void AGameLevel::BeginPlay()
{
	Super::BeginPlay();

	if (bBuildOnPlay)
	{
		FActorSpawnParameters Params = FActorSpawnParameters();
		Params.Owner = this;

		Grid = GetWorld()->SpawnActor<AGrid>(GetActorLocation(), FRotator::ZeroRotator, Params);
		Grid->init(this, MaxWidth, MaxLength);

		Warmup();
		SpawnRooms();
		//ConnectRooms();
		//Cleanup();
		//Finalize();
	}
}


//
//FVector AGameLevel::GetRandomBorderAlignedRoomPosition(float Width, float Length)
//{
//	FVector RandomDirection = FVector(FMath::Rand32(), FMath::Rand32(), FMath::Rand32());
//	RandomDirection = RandomDirection.GetSafeNormal();
//	FVector RandomDirectionAbs = RandomDirection;
//	if (RandomDirectionAbs.X < 0)
//		RandomDirectionAbs.X = -RandomDirectionAbs.X;
//	if (RandomDirectionAbs.Y < 0)
//		RandomDirectionAbs.Y = -RandomDirectionAbs.Y;
//	if (RandomDirectionAbs.Z < 0)
//		RandomDirectionAbs.Z = -RandomDirectionAbs.Z;
//
//	float Min = GetActorLocation().X - ((MaxWidth - Width) / 2);
//	float Max = GetActorLocation().X + ((MaxWidth - Width) / 2);
//	float RandomXPosLevelAligned = FMath::RandRange(Min, Max);
//	Min = GetActorLocation().Y - ((MaxLength - Length) / 2);
//	Max = GetActorLocation().Y + ((MaxLength - Length) / 2);
//	float RandomYPosLevelAligned = FMath::RandRange(Min, Max);
//	if (RandomDirection.X > RandomDirection.Y)
//		if (RandomDirection.X > 0)
//			RandomXPosLevelAligned = GetActorLocation().X + ((MaxWidth - Width) / 2);
//		else
//			RandomXPosLevelAligned = GetActorLocation().X - ((MaxWidth - Width) / 2);
//	else
//		if (RandomDirection.Y > 0)
//			RandomYPosLevelAligned = GetActorLocation().X + ((MaxLength - Length) / 2);
//		else
//			RandomYPosLevelAligned = GetActorLocation().X - ((MaxLength - Length) / 2);
//
//	return GetActorLocation() + FVector(RandomXPosLevelAligned, RandomYPosLevelAligned, 0);
//}

bool AGameLevel::IsValidRoomPosition(FBoxSphereBounds Bounds)
{
	//
	for (int i = 0; i < ReadyRooms.Num() - 1; i++)
	{
		ARoom* ReadyRoom = ReadyRooms[i].GetDefaultObject();
		if(Bounds.BoxesIntersect(Bounds, (ReadyRoom->FindComponentByClass<UStaticMeshComponent>()->Bounds)))
			return false;
	}
	//no overlaps found
	return true;
}

TSubclassOf<AActor> AGameLevel::GetRandomRoomWithinChunkCount(FBoxSphereBounds Bounds)
{
	int RandomRoomIndex;
	TSubclassOf<AActor> RoomToReturn;

	for (int i = 0; i < RoomsToSpawn.Num(); i++)
	{
		RandomRoomIndex = FMath::RandRange(0, RoomsToSpawn.Num() - 1);

		RoomToReturn = RoomsToSpawn[RandomRoomIndex]->RoomAsset;

		// cast to actor, then to room.
		ARoom* Room = Cast<ARoom>(RoomToReturn.GetDefaultObject());
		if (Room && Room->ChunkCountWidth <= Bounds.BoxExtent.X && Room->ChunkCountLength <= Bounds.BoxExtent.Y)
		// if this random Room exists and is inside of given bounds
		{ 
			return RoomToReturn; 
		}
	}
	//unsuccessful
	return nullptr;
}

void AGameLevel::Warmup()
{
}

//minimum 5x5 room
//places the largest rooms first
void AGameLevel::SpawnRooms()
{

	float RoomHeight = 5 * AGrid::ChunkRootCM;
	float OriginZ = 5;

	float BorderOffset = 2.5f * AGrid::ChunkRootCM;

	//check length and width of a randomly selected position.
	int RoomPlacementAttempts = 1000;
	
	float RandX;
	float RandY;

	URoomDataAsset* RandomRoom;


	bool bXIsPositiveRandom;
	bool bYIsPositiveRandom;


	AActor* LastSpawnedRoom;
	FActorSpawnParameters Params;
	Params.Owner = this;
	//we determine if its valid to spawn, not the physics engine
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	FVector LevelOrigin;
	FVector LevelExtent;
	GetActorBounds(false, LevelOrigin, LevelExtent, true);

	for (int i = 0; i < RoomPlacementAttempts; i++)
	{
		RandX = FMath::RandRange((LevelOrigin.X - LevelExtent.X) + BorderOffset, (LevelOrigin.X + LevelExtent.X) - BorderOffset);
		RandY = FMath::RandRange((LevelOrigin.Y - LevelExtent.Y) + BorderOffset, (LevelOrigin.Y + LevelExtent.Y) - BorderOffset);

		bXIsPositiveRandom = FMath::RandBool();
		bYIsPositiveRandom = FMath::RandBool();

		

		FBoxSphereBounds Bounds;

		Bounds.Origin.X = RandX;
		Bounds.Origin.Y = RandY;
		Bounds.Origin.Z = OriginZ + (RoomHeight / 2);

		Bounds.BoxExtent = Grid->FindChunkCountExtentFromOrigin(Bounds.Origin);

		TSubclassOf<AActor> RoomActor = GetRandomRoomWithinChunkCount(Bounds);
		FVector temp;
		//gets the centimeter extent of the room bounding box.
		RoomActor.GetDefaultObject()->GetActorBounds(false, temp, Bounds.BoxExtent, true);

		//passes in bounds
		if (RoomActor && IsValidRoomPosition(Bounds))
		{
			RandomRoom = RoomsToSpawn[FMath::RandRange(0, RoomsToSpawn.Num() - 1)];
			LastSpawnedRoom = GetWorld()->SpawnActor<AActor>(RandomRoom->RoomAsset, GetActorLocation(), GetActorRotation(), Params);

			RoomInstances.Add(Cast<ARoom>(LastSpawnedRoom));
		}
	}
}

void AGameLevel::ConnectRooms()
{
	if (RoomInstances.Num() < 1) return;
		
	Grid->ConnectDoorways();
}

void AGameLevel::Finalize()
{
	//check if priority rooms, a navmesh from start->end, and other stuff exists.
	AGameLevel::Cleanup();
	
	Grid->SpawnAssets();
}

void AGameLevel::Cleanup()
{
}


