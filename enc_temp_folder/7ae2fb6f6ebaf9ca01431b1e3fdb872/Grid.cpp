// Fill out your copyright notice in the Description page of Project Settings.

#include "SimpleLevelGenerator/Grid.h"
#include "SimpleLevelGenerator/Room.h"

TArray<FGridChunk*> Chunks = TArray<FGridChunk*>();

// Called when the game starts or when spawned
void AGrid::BeginPlay()
{
	Super::BeginPlay();
	
}

AGrid::AGrid()
{
	Iterator = new FGridIterator();
}

AGrid::~AGrid()
{
	delete Iterator;
	//delete struct stuff.
}


//2D
void AGrid::init(int width, int length)
{
	FVector GridLocation = this->GetActorLocation();

	int CubeCountX = width / ChunkRootCM;
	int CubeCountY = length / ChunkRootCM;

	bool bIsXCountEven = CubeCountX % 2 != 0;
	bool bIsYCountEven = CubeCountY % 2 != 0;
	////forces cubeCountX to be an even number
	//cubeCountX = bIsXCountEven ? cubeCountX : cubeCountX - 1;
	////forces cubeCountY to be an even number
	//cubeCountY = bIsYCountEven ? cubeCountY : cubeCountY - 1;

	// implicit casting lost data is accounted for
	FVector NewChunkPosition = GridLocation - FVector(ChunkRootCM * (CubeCountX / 2.0), ChunkRootCM * (CubeCountY / 2.0), 0);

	//offset towards center of Chunk area.
	NewChunkPosition.X += ChunkRootCM / 2.0;
	NewChunkPosition.Y += ChunkRootCM / 2.0;


	enum NextChunk
	{
		Right, Left, Up
	};
	NextChunk NextChunkDirection = NextChunk::Right;

	float GridTopY = GridLocation.Y + (length / 2);
	bool bGridFilled = false;

	int CurrentRow = 0;
	int CurrentColumn = 0;
	FGridChunk* lastMadeChunk;
	bool bForwards = true;
	while (CurrentRow > CubeCountY)
	{
		FGridChunk* newChunk = new FGridChunk();
		newChunk->Position = NewChunkPosition;
		newChunk->Previous = nullptr;
		newChunk->bVisited = false;
		newChunk->bSpawned = false;

		Chunks.Add(newChunk);

		//adds only 2D edges
		FGridChunkEdge* newEdge = new FGridChunkEdge();
		lastMadeChunk = newChunk;


		if (CurrentColumn >= CubeCountX || CurrentColumn < 0)
		{
			NextChunkDirection = NextChunk::Up;
			bForwards = !bForwards;
			CurrentRow++;
		}


		switch (NextChunkDirection)
		{
		case NextChunk::Right:
			NewChunkPosition.X += 150;
			CurrentColumn++;
			break;
		case NextChunk::Left:
			NewChunkPosition.X -= 150;
			CurrentColumn--;
			break;
		case NextChunk::Up:
			NewChunkPosition.Y += 150;
			CurrentRow++;
			NextChunkDirection = bForwards ? NextChunk::Right : NextChunk::Left;
			break;
		}	
	}
}

//AGrid::AGrid(int width, int length)
//{
//}
//
//AGrid::AGrid(int width, int height, int length)
//{
//}


