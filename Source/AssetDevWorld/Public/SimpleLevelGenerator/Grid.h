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
	TArray<FGridChunkEdge*>* Edges;
	FVector Position;
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
	FGridChunk* Target;
	FGridChunk* operator*() { return Target; }
	void Iterate(AGrid::Directions dir) { for (auto i = Target->Edges->begin(); i != Target->Edges->end(); ++i) { if ((AGrid::LastMadeInstance->IterDirections.FindRef(dir) - (*i)->Normal).Size() < 0.1f) Target = (*i)->Target; } };
};

UCLASS(Placeable)
class ASSETDEVWORLD_API AGrid : public AActor
{
	GENERATED_BODY()
	
private:
	TArray<FGridChunk*> Chunks;
	static const int ChunkRootCM = 150;
	FGridIterator* Iterator;
	FGridChunk* StartChunk;
	FGridChunk* Start() { return StartChunk; };
public:	
	enum Directions
	{
		Up,
		Left,
		Down,
		Right,
		Backwards,
		Forwards
	};
	static TMap<Directions, FVector> IterDirections;
	static AGrid* LastMadeInstance;

	virtual void BeginPlay() override;

	AGrid();
	~AGrid();

	void init(int width, int length);

	//AGrid(int width, int length);
	////maybe swap because of unreal xyz cordinate scheme.
	//AGrid(int width, int height, int length);

	FGridChunk* GetChunkNearest(FVector position);
	Directions NextDirectionTowards(FVector direction);
	void ConnectAdjacentChunks(FGridChunk* origin, FGridChunk* target);
	bool AreAdjacent(FGridChunk* Chunk1, FGridChunk* Chunk2);
	void ReserveChunks(ARoom* room);
	void CarvePassageways();
	void ConnectDoorways();
	void SpawnAssets();
	void Cleanup();

};
