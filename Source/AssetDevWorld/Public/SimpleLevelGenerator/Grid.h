// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Grid.generated.h"

class ARoom;

USTRUCT()
struct FGridChunk
{
	GENERATED_BODY()
public:
	TArray<FGridChunkEdge> Edges;
	FGridChunk* Previous;
	bool bVisited;
	bool bSpawned;
};

USTRUCT()
struct FGridChunkEdge
{
	GENERATED_BODY()
public:
	FGridChunk* Target;
	FVector Normal;
};

//used by AGrid to iterate through FGridChunk containers
USTRUCT()
struct FGridIterator
{
	GENERATED_BODY()
public:
	void (*IterDirection)();
public:
	FGridChunk* Target;
	void Up() {}
	void Down() {}
	void Left() {}
	void Right() {}
	void Forward() {}
	void Backward() {}
};

UCLASS()
class ASSETDEVWORLD_API AGrid : public AActor
{
	GENERATED_BODY()
	
private:
	TArray<FGridChunk*> Chunks;

public:	
	virtual void BeginPlay() override;

	void SetVisited(ARoom* room);
	void CarvePassageways();
	void ConnectDoorways();
	void SpawnAssets();
	void Cleanup();

};
