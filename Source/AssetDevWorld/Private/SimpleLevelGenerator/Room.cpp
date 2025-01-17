// Fill out your copyright notice in the Description page of Project Settings.


#include "SimpleLevelGenerator/Room.h"
#include "SimpleLevelGenerator/EntityDataAsset.h"
#include "SimpleLevelGenerator/Grid.h"

void ARoom::SetSocketLocation(FVector& ReturnVector, FName SocketName)
{
	ReturnVector = FindComponentByClass<UStaticMeshComponent>()->GetSocketTransform(SocketName).GetLocation();
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
	FVector RoomOrigin;
	FVector RoomBoxExtent;
	GetActorBounds(true, RoomOrigin, RoomBoxExtent, true);
	BoundsArea = RoomBoxExtent.X * RoomBoxExtent.Y;
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

void ARoom::CreateSpawnersAndDoorLocations()
{

}

void ARoom::CaptureSockets()
{
	//UStaticMeshComponent* Mesh = GetComponentByClass<UStaticMeshComponent>();
	//TArray<FName> Names = Mesh->GetAllSocketNames();

	//TArray<char> Chars;
	//TArray<char> Doorway = Cast<const char*, FString>(FString("doorwaysocket").begin());
	//TArray<char> Pathway;
	//TArray<char> Entry;
	/*for (FName Name : Names)
	{
		int i1 = 0, i2 = 0, i3 = 0, i4 = 0;
		for (char C : Chars)
		{
			if (CompareItems(C, ))
			{

			}
		}*/
	//}
}

