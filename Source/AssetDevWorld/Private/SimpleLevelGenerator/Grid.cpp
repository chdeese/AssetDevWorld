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
	TMap<AGrid::Directions, FVector> IterDirections = TMap<AGrid::Directions, FVector>();
	IterDirections = { {Directions::Up, FVector(0, 0, 1)}, {Directions::Down, FVector(0, 0, -1)},
					   {Directions::Forwards, FVector(0, 1, 0)}, {Directions::Backwards, FVector(0, -1, 0)},
					   {Directions::Left, FVector(-1, 0, 0)}, {Directions::Right, FVector(1, 0, 0)} };

	LastMadeInstance = this;
}

AGrid::~AGrid()
{
	delete Iterator;
	//delete struct stuff.
}


//flat mode
void AGrid::init(int width, int length)
{
	enum NextChunk
	{
		Right, Left, Forward
	};

	// implicit casting lost data is accounted for
	int CubeCountX = width / ChunkRootCM;
	int CubeCountY = length / ChunkRootCM;

	NextChunk NextChunkDirection = NextChunk::Right;
	FVector NewChunkPosition = this->GetActorLocation() - FVector(ChunkRootCM * (CubeCountX / 2.0), ChunkRootCM * (CubeCountY / 2.0), 0);
	//offset towards center of Chunk area.
	NewChunkPosition.X += ChunkRootCM / 2.0;
	NewChunkPosition.Y += ChunkRootCM / 2.0;

	int CurrentRow = 0;
	int CurrentColumn = 0;
	bool bGridFilled = false;
	bool bForwards = true;
	bool bFirstIteration = true;
	FGridChunk* lastMadeChunk = nullptr;
	while (!bGridFilled)
	{
		//exits loop
		if (CurrentRow == CubeCountY)
		{
			bGridFilled = true;
			continue;
		}

		FGridChunk* newChunk = new FGridChunk();
		newChunk->Position = NewChunkPosition;
		newChunk->Previous = nullptr;
		newChunk->bVisited = false;
		newChunk->bSpawned = false;
		{
			newChunk->Edges = new TArray<FGridChunkEdge*>();

			//right
			FGridChunkEdge* newEdge = new FGridChunkEdge();
			newEdge->Target = nullptr;
			newEdge->Normal = FVector(1, 0, 0);
			newChunk->Edges->Add(newEdge);

			//left
			newEdge = new FGridChunkEdge();
			newEdge->Target = nullptr;
			newEdge->Normal = FVector(-1, 0, 0);
			newChunk->Edges->Add(newEdge);

			//forwards
			newEdge = new FGridChunkEdge();
			newEdge->Target = nullptr;
			newEdge->Normal = FVector(0, 1, 0);
			newChunk->Edges->Add(newEdge);

			//backwards
			newEdge = new FGridChunkEdge();
			newEdge->Target = nullptr;
			newEdge->Normal = FVector(0, -1, 0);
			newChunk->Edges->Add(newEdge);
		}

		//set start chunk if this is the first iteration
		if (bFirstIteration) StartChunk = newChunk, bFirstIteration = !bFirstIteration;

		// connects last made chunk
		if (lastMadeChunk) 
			ConnectAdjacentChunks(lastMadeChunk, newChunk);

		// connects lower adjacent chunks
		if (CurrentRow > 0 && CurrentColumn != 0 && CurrentColumn != CubeCountX - 1)
		{
			Iterator->Target = lastMadeChunk;
			//iterate in approximate normal direction
			Iterator->Iterate((newChunk->Position - lastMadeChunk->Position).GetUnsafeNormal());

			ConnectAdjacentChunks(Iterator->Target, newChunk);
		}

		Chunks.Add(newChunk);

		//if heading out of bounds head forward to the next row
		if (CurrentColumn == CubeCountX - 1 || CurrentColumn == 0)
			NextChunkDirection = NextChunk::Forward;

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
		case NextChunk::Forward:
			NewChunkPosition.Y += 150;
			CurrentRow++;
			bForwards = !bForwards;
			NextChunkDirection = bForwards ? NextChunk::Right : NextChunk::Left;
			break;
		}	
	}
}

FGridChunk* AGrid::GetChunkNearest(FVector position)
{
	Iterator->Target = StartChunk;

	FVector VectorTo = position - Iterator->Target->Position;
	FVector ChunkBorderPosTowardsPos = VectorTo.GetUnsafeNormal() * ChunkRootCM;
	if (VectorTo.Size() < ChunkBorderPosTowardsPos.Size())
		return Iterator->Target;

	bool keepgoing;
	while (keepgoing)
	{
		Iterator->Iterate(NextDirectionTowards(position));
	}
}

AGrid::Directions AGrid::NextDirectionTowards(FVector direction)
{
	if (FMath::Abs(direction.X) > FMath::Abs(direction.Y))
	{
		if (direction.X > 0)
			return Directions::Right;
		else
			return Directions::Left;
	}
	else
	{
		if (direction.Y > 0)
			return Directions::Up;
		else
			return Directions::Down;
	}
}

//only works if axis aligned with the unreal world.
void AGrid::ConnectAdjacentChunks(FGridChunk* Chunk1, FGridChunk* Chunk2)
{
	FVector C1ToC2 = Chunk2->Position - Chunk1->Position;
	C1ToC2 = C1ToC2.GetUnsafeNormal();

	auto C1 = Chunk1->Edges->begin();
	auto C1End = Chunk2->Edges->end();
	auto C2 = Chunk1->Edges->begin();
	auto C2End = Chunk2->Edges->end();

	//iterates fully between each list
	while (C1 != C1End || C2 != C2End)
	{ 
		//forces axis alignement requirment
		FVector difference = (*C1)->Normal - C1ToC2;
		if (difference.Size() < 0.1f)
			(*C1)->Target = Chunk2;

		difference = (*C2)->Normal - (-C1ToC2);
		if (difference.Size() < 0.1f)
			(*C2)->Target = Chunk1;

		++C1;
		++C2;
	}
}

bool AGrid::AreAdjacent(FGridChunk* Chunk1, FGridChunk* Chunk2)
{
	FVector DirectionTo = (Chunk2->Position - Chunk1->Position).GetUnsafeNormal();
	for (auto i = Chunk1->Edges->begin(); i != Chunk1->Edges->end(); ++i)
	{
		if (DirectionTo.Dot((*i)->Normal) > 0.9f)
			return (*i)->Target == Chunk2 ? true : false;
	}
	return false;
}

void AGrid::ReserveChunks(ARoom* room)
{
	bool bRoomCenterChunkFound = false;
	float DistanceTo;
	float CurrentArea;

	for (float VisitedArea = 0; !bRoomCenterChunkFound; VisitedArea += CurrentArea)
	{
		if (VisitedArea == 0)
			Iterator->Target = GetChunkNearest(room->GetActorLocation());
	
	}
}

void AGrid::CarvePassageways()
{

}

void AGrid::ConnectDoorways()
{
}

void AGrid::SpawnAssets()
{
}

void AGrid::Cleanup()
{
}

//AGrid::AGrid(int width, int length)
//{
//}
//
//AGrid::AGrid(int width, int height, int length)
//{
//}


