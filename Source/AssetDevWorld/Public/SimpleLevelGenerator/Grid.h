// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Grid.generated.h"

class ARoom;
class AGameLevel;

USTRUCT()
struct FGridChunkEdge
{
	GENERATED_BODY()
public:
	FGridChunk* Target = nullptr;
	FVector Normal = FVector::ZeroVector;
};

USTRUCT()
struct FGridChunk
{
	GENERATED_BODY()
public:
	TArray<FGridChunkEdge*> Edges;
	//if edge not found create new edge target->nullptr
	void GetEdge(FGridChunkEdge& edge, FVector normal) { 
		for (auto i = Edges.begin(); i != Edges.end(); ++i) 
		{ if ((*i)->Normal.Dot(normal) > 0.9f) edge = *(*i); return; } /*cant find edge*/edge = FGridChunkEdge(); return; }
	FVector Position;
	FGridChunk* Previous;
	bool bVisited;
	bool bSpawned;
};


//used by AGrid to iterate through FGridChunk containers
USTRUCT()
struct FGridIterator
{
	GENERATED_BODY()
public:
	FGridChunk* Target;
	FGridChunk* operator*() { return Target; }
	bool Iterate(FVector dir) { for (auto i = Target->Edges.begin(); i != Target->Edges.end(); ++i) { if (0.1f > (dir - (*i)->Normal).Size()) { Target = (*i)->Target; return true; } } /*cant find dir*/return false; };
};

UCLASS(Placeable)
class ASSETDEVWORLD_API AGrid : public AActor
{
	GENERATED_BODY()
	
public:
	TArray<FGridChunk*> Chunks;
	static const int ChunkRootCM = 150;
	FGridIterator* Iterator;
	FGridChunk* StartChunk;
	AGameLevel* GameLevel;
	AActor* EntryPositionActor;
public:	
	enum Directions
	{
		Forwards,
		Backwards,
		Left,
		Right,
		Up,
		Down
	};

	AGrid::Directions RandomDirection(int Dimensions);

	TMap<AGrid::Directions, FVector> IterDirections = { {Directions::Up, FVector(0, 0, 1)}, {Directions::Down, FVector(0, 0, -1)},
					   {Directions::Forwards, FVector(0, 1, 0)}, {Directions::Backwards, FVector(0, -1, 0)},
					   {Directions::Left, FVector(-1, 0, 0)}, {Directions::Right, FVector(1, 0, 0)} };


	virtual void BeginPlay() override;

	AGrid();
	~AGrid();

	void init(AGameLevel* level, int width, int length);

	//AGrid(int width, int length);
	////maybe swap because of unreal xyz cordinate scheme.
	//AGrid(int width, int height, int length);

	FGridChunk* Start() { return StartChunk; };
	FGridIterator* GetIterator();
	FGridChunk* GetChunkNearest(FVector position);
	FVector NextDirectionTowards(FVector direction);
	FGridChunkEdge* GetRandomValidEdge();
	void ConnectAdjacentChunks(FGridChunk* origin, FGridChunk* target);
	bool AreAdjacent(FGridChunk* Chunk1, FGridChunk* Chunk2);
	bool IsChunkWithinRoomBounds(ARoom* Room, FGridChunk* Chunk);
	bool ShouldBeReserved(FGridChunk* Chunk, ARoom* Room);
	void AddRoom(ARoom* room);
	void CarvePassageways(float MaxArea);
	void ConnectDoorways();
	void Cleanup();
	void SpawnAssets();

};
