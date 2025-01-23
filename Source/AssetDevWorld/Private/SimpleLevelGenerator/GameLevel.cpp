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
	RoomsToSpawn = TArray<TSubclassOf<AActor>>();
}

AGameLevel::AGameLevel(FVector entryPosition)
{
	//not avaliable in editor.

	//creates new level at runtime.
	bBuildOnPlay = true;

	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bAlignEntry = true;
	RoomsToSpawn = TArray<TSubclassOf<AActor>>();
	EntryPosition = entryPosition;

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

//checks for a 3 chunk gap
bool AGameLevel::IsValidRoomPosition(FBoxSphereBounds Bounds)
{
	FBoxSphereBounds OtherBounds;
	float PassagewayAreaGrace = Grid->ChunkRootCM / 2;

	for (int i = 0; i < RoomInstances.Num(); i++)
	{
		OtherBounds = RoomInstances[i]->GetComponentByClass<UStaticMeshComponent>()->Bounds;
		FBoxSphereBounds ExtendedBounds = FBoxSphereBounds(Bounds.Origin, Bounds.BoxExtent, Bounds.SphereRadius);
		//fake a larger size so guarantees room for a 3 chunk gap
		ExtendedBounds.BoxExtent += 3 * PassagewayAreaGrace * FVector(1, 1, 1);
		if (Bounds.BoxesIntersect(ExtendedBounds, OtherBounds))
			return false;
	}
	return true;
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

	FVector RoomOrigin;
	FVector RoomBoxExtent;

	FActorSpawnParameters Params;
	TSubclassOf<AActor> RoomClass;
	float RoomArea;
	float RandomNum;
	for (float CurrentFilledArea = 0; CurrentFilledArea < AssetRoomArea; CurrentFilledArea += RoomArea)
	{
		RoomArea = 0;
		if (!PriorityRooms.IsEmpty())
		{
			for (URoomDataAsset* PriorityRoomData : PriorityRooms)
			{
				RoomClass = *PriorityRoomData->RoomAsset;
				//casted only for BoundsArea float value

				Cast<AActor>(RoomClass->GetDefaultObject())->GetActorBounds(true, RoomOrigin, RoomBoxExtent, true);
				RoomArea += RoomBoxExtent.X * RoomBoxExtent.Y;

				RoomsToSpawn.Add(RoomClass);
			}
			PriorityRooms.Empty();
			continue;
		}


		if (OptionalRooms.IsEmpty())
			return;
		RandomNum = FMath::RandRange(0, OptionalRooms.Num() - 1);
		RoomClass = OptionalRooms[RandomNum]->RoomAsset;
		OptionalRooms.RemoveAt(RandomNum, EAllowShrinking::Yes);

		Cast<AActor>(RoomClass->GetDefaultObject())->GetActorBounds(true, RoomOrigin, RoomBoxExtent, true);
		RoomArea += RoomBoxExtent.X * RoomBoxExtent.Y;
		RoomsToSpawn.Add(RoomClass);
	}
}

//places the largest rooms first
void AGameLevel::SpawnRooms()
{
	
	//use vector math to place beginning room on border facing outwards, then get entry socket.
	int RandomNum;
	FVector Offset = FVector::ZeroVector;
	FActorSpawnParameters Params = FActorSpawnParameters();
	Params.Owner = this;
	FVector BeginningRoomOrigin;
	FVector BeginningRoomBoxExtent;
	GetActorBounds(true, BeginningRoomOrigin, BeginningRoomBoxExtent, true);

	AActor* BeginningRoom;
	bool bFoundRoom = false;
	while (!BeginningRooms.IsEmpty() && !bFoundRoom)
	{
		RandomNum = FMath::RandRange(0, BeginningRooms.Num() - 1);
		if (BeginningRooms[RandomNum])
		{
			BeginningRoom = GetWorld()->SpawnActor<AActor>(BeginningRooms[RandomNum]->RoomAsset, GetRandomBorderAlignedRoomPosition(BeginningRoomBoxExtent.X * 2, BeginningRoomBoxExtent.Y * 2), FRotator::ZeroRotator, Params);
			RoomInstances.Add(Cast<ARoom>(BeginningRoom));
			bFoundRoom = true;
		}
		BeginningRooms.Remove(BeginningRooms[RandomNum]);
	}
	if (RoomsToSpawn.IsEmpty()) return;
	// smallest -> largest (by area)
	RoomsToSpawn.Sort();
	for (TSubclassOf<AActor> Room = RoomsToSpawn.Last(); !RoomsToSpawn.IsEmpty(); RoomsToSpawn.Remove(Room))
	{
		AActor* RoomToSpawn = Room.GetDefaultObject();
		if (!RoomToSpawn)
			continue;
		FVector BoundsOrigin;
		FVector BoundsExtent;
		RoomToSpawn->GetActorBounds(false, BoundsOrigin, BoundsExtent, true);

		FBoxSphereBounds Bounds;
		Bounds.Origin = BoundsOrigin;
		Bounds.BoxExtent = BoundsExtent;

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		for (bool bCollisionDetected = true; bCollisionDetected; bCollisionDetected = !IsValidRoomPosition(Bounds))
		{
			Bounds.Origin = GetRandomBorderAlignedRoomPosition(Bounds.BoxExtent.X * 2, Bounds.BoxExtent.Y * 2);	
		}

		AActor* SpawnedRoom = GetWorld()->SpawnActor<AActor>(*Room, Bounds.Origin, FRotator::ZeroRotator, SpawnParams);

		RoomInstances.Add(Cast<ARoom>(SpawnedRoom));
	}
}

void AGameLevel::GenerateNewRooms()
{
	if (RoomTemplates.IsEmpty())
		return;

	AActor* NewRoom;
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	TSubclassOf<AActor> ActorSubclass = TSubclassOf<ARoom>();
	
	FVector RandomGeneratedChunkCount;
	URoomTemplateDataAsset* RandomTemplate;

	float FilledArea;
	float MaxArea = UniqueRoomPercent * (MaxWidth * MaxLength);
	for (float TotalFilledArea = 0; TotalFilledArea < MaxArea; TotalFilledArea += FilledArea)
	{
		FilledArea = 0;
		RandomGeneratedChunkCount = { FMath::RandRange(UniqueRoomChunksMin.X, UniqueRoomChunksMax.X), FMath::RandRange(UniqueRoomChunksMin.Y, UniqueRoomChunksMax.Y), 3 };

		FVector NewPosition = GetRandomBorderAlignedRoomPosition(RandomGeneratedChunkCount.X, RandomGeneratedChunkCount.Y);
		NewRoom = GetWorld()->SpawnActor<AActor>(ActorSubclass, NewPosition, FRotator::ZeroRotator, SpawnParams);

		RandomTemplate = (RoomTemplates[FMath::RandRange(0, RoomTemplates.Num())]);
		if (!NewRoom->FindComponentByClass<UStaticMeshComponent>())
			NewRoom = GenerateNewRoom(NewRoom, NewPosition, RandomGeneratedChunkCount * Grid->ChunkRootCM, FRotator::ZeroRotator, RandomTemplate);
		
		RoomInstances.Add(Cast<ARoom, AActor>(*NewRoom));

		Grid->AddRoom(Cast<ARoom, AActor>(*NewRoom));

		FilledArea += RandomGeneratedChunkCount.X * RandomGeneratedChunkCount.Y;
	}
}

//creates a square room
//test vector math
ARoom* AGameLevel::GenerateNewRoom(AActor* OriginalOwner, FVector Position, FVector Scale, FRotator Rotation = FRotator::ZeroRotator, URoomTemplateDataAsset* Template = nullptr)
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = Owner;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ARoom* NewRoom = GetWorld()->SpawnActor<ARoom>(Position, Rotation, SpawnParams);
	SpawnParams.Owner = NewRoom;

	AActor* RoomPart;
	RoomPart = GetWorld()->SpawnActor<ARoom>(Template->FloorAsset, Position, Rotation, SpawnParams);
	RoomPart->SetActorRelativeScale3D(RoomPart->GetActorRelativeScale3D() * Scale);

	bool bPlaceDoor;
	FVector WallRotation = Rotation.Vector();
	int RandomDoorCount = FMath::RandRange(1, 3);
	int RandomChunkHeight = FMath::RandRange(UniqueRoomChunksMin.Z, UniqueRoomChunksMax.Z);
	for (int ChunksHigh = 0; ChunksHigh < RandomChunkHeight; ChunksHigh++)
		for (int i = 4; i > 0; i--)
		{
			bPlaceDoor = false;
			if (RandomDoorCount == i)
			{
				bPlaceDoor = true;
				RandomDoorCount--;
			}
			else
				bPlaceDoor = FMath::RandBool();
			
			//only places doors ground level
			if (bPlaceDoor && ChunksHigh == 0)
				RoomPart = GetWorld()->SpawnActor<ARoom>(Template->DoorAsset, Position, Rotation, SpawnParams);
			else
				RoomPart = GetWorld()->SpawnActor<ARoom>(Template->WallAsset, Position, Rotation, SpawnParams);

			RoomPart->SetActorRelativeScale3D(RoomPart->GetActorRelativeScale3D() * Scale);
			RoomPart->SetActorRotation(WallRotation.Rotation());

			WallRotation += FVector(0, 0, 90);
		}

	return NewRoom;
}

void AGameLevel::CarvePassageways()
{
	Grid->CarvePassageways(PassagewayAreaPercent * MaxWidth * MaxLength);
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

void AGameLevel::UpdateEntitySpawns()
{
	//needs to be attached to a periodic event system.
}

