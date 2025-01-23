// Fill out your copyright notice in the Description page of Project Settings.


#include "SimpleLevelGenerator/Room.h"
#include "SimpleLevelGenerator/EntityDataAsset.h"
#include "SimpleLevelGenerator/Grid.h"

void ARoom::SetSocketLocation(FVector& ReturnVector, FName SocketName)
{
	ReturnVector = FindComponentByClass<UStaticMeshComponent>()->GetSocketLocation(SocketName);
}

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
	
	BoundsArea = GetBoundsArea();

	CaptureSockets();
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

void ARoom::CaptureSockets()
{
	UStaticMeshComponent* Mesh = GetComponentByClass<UStaticMeshComponent>();
	if (!Mesh || !Mesh->HasAnySockets())
		return;
	TArray<FName> Names = Mesh->GetAllSocketNames();

	FString Doorway = FString("doorwaysocket");
	FString Spawner = FString("spawnersocket");
	FString Entry = FString("entrysocket");

	for (FName Name : Names)
	{
		int i1 = 0, i2 = 0, i3 = 0, i4 = 0;
		FName SocketType = FName(NAME_None);

		for (char i0 = 0; SocketType.IsNone() && i0 < Name.ToString().Len(); i0++)
		{
			char C = Name.ToString()[i0];
			if (Doorway.Len() == i1)
				DoorLocations.Add(Mesh->GetSocketLocation(Name));
			if (C == Doorway[i0])
			{
				i1++;
			}
			else
			{
				//reset and check again
				i1 = 0;
				if (C, Doorway[i1]) i1++;
			}

			if (Spawner.Len() == i2)
				SpawnerLocations.Add(Mesh->GetSocketLocation(Name));
			if (C == Spawner[i0])
			{
				i2++;
			}
			else
			{
				//reset and check again
				i2 = 0;
				if (C, Spawner[i2]) i2++;
			}

			if (Entry.Len() == i3)
				EntryLocation = Mesh->GetSocketLocation(Name);
			if (C == Entry[i0])
			{
				i3++;
			}
			else
			{
				//reset and check again
				i3 = 0;
				if (C, Entry[i3]) i3++;
			}
		}
	}
}

float ARoom::GetBoundsArea()
{
	FVector RoomOrigin;
	FVector RoomBoxExtent;
	GetActorBounds(true, RoomOrigin, RoomBoxExtent, true);
	return RoomBoxExtent.X * RoomBoxExtent.Y;
}

