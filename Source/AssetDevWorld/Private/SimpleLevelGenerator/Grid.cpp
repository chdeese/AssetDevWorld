// Fill out your copyright notice in the Description page of Project Settings.

#include "SimpleLevelGenerator/Grid.h"
#include "SimpleLevelGenerator/Room.h"
#include "SimpleLevelGenerator/GameLevel.h"
#include "SimpleLevelGenerator/PassagewayDataAsset.h"



AGrid::Directions AGrid::RandomDirection(int Dimensions = 0)
{
	Dimensions *= 2;
	switch (FMath::RandRange(0, Dimensions - 1))
	{
	default:
		return AGrid::Directions::Forwards;
	case 1:
		return AGrid::Directions::Backwards;
	case 2:
		return AGrid::Directions::Left;
	case 3:
		return AGrid::Directions::Right;
	case 4:
		return AGrid::Directions::Up;
	case 5:
		return AGrid::Directions::Down;
	}
}

// Called when the game starts or when spawned
void AGrid::BeginPlay()
{
	Super::BeginPlay();
	
}

AGrid::AGrid()
{
	Chunks = TArray<FGridChunk*>();
}

AGrid::~AGrid()
{
}


//flat mode
void AGrid::init(AGameLevel* level, int width, int length)
{
	GameLevel = level;

	enum NextChunk
	{
		Right, Left, Forward
	};

	// implicit casting lost data is accounted for
	int CubeCountX = width / GameLevel->ChunkRootCM;
	int CubeCountY = length / GameLevel->ChunkRootCM;

	NextChunk NextChunkDirection = NextChunk::Right;
	FVector NewChunkPosition = this->GetActorLocation() - FVector(GameLevel->ChunkRootCM * (CubeCountX / 2.0), GameLevel->ChunkRootCM * (CubeCountY / 2.0), 0);
	//offset towards center of Chunk area.
	NewChunkPosition.X += GameLevel->ChunkRootCM / 2.0;
	NewChunkPosition.Y += GameLevel->ChunkRootCM / 2.0;

	int CurrentRow = 0;
	int CurrentColumn = 0;
	bool bGridFilled = false;
	bool bForwards = true;
	bool bFirstIteration = true;
	FGridIterator* Iter = new FGridIterator();
	FGridChunk* LastMadeChunk = nullptr;
	while (!bGridFilled)
	{
		//exits loop
		if (CurrentRow == CubeCountY)
		{
			bGridFilled = true;
			continue;
		}

		FGridChunk* NewChunk = new FGridChunk();
		NewChunk->Position = NewChunkPosition;
		NewChunk->Previous = nullptr;
		NewChunk->bVisited = false;
		NewChunk->bSpawned = false;
		{
			NewChunk->Edges = TArray<FGridChunkEdge*>();

			//right
			FGridChunkEdge* NewEdge = new FGridChunkEdge();
			NewEdge->Target = nullptr;
			NewEdge->Normal = FVector(1, 0, 0);
			NewChunk->Edges.Add(NewEdge);

			//left
			NewEdge = new FGridChunkEdge();
			NewEdge->Target = nullptr;
			NewEdge->Normal = FVector(-1, 0, 0);
			NewChunk->Edges.Add(NewEdge);

			//forwards
			NewEdge = new FGridChunkEdge();
			NewEdge->Target = nullptr;
			NewEdge->Normal = FVector(0, 1, 0);
			NewChunk->Edges.Add(NewEdge);

			//backwards
			NewEdge = new FGridChunkEdge();
			NewEdge->Target = nullptr;
			NewEdge->Normal = FVector(0, -1, 0);
			NewChunk->Edges.Add(NewEdge);
		}

		//set start chunk if this is the first iteration
		if (bFirstIteration)
		{
			StartChunk = NewChunk;
			bFirstIteration = !bFirstIteration;
		}

		// connects last made chunk
		if (LastMadeChunk) 
			ConnectAdjacentChunks(LastMadeChunk, NewChunk);

		// connects lower adjacent chunks
		if (CurrentRow > 0 && CurrentColumn != 0 && CurrentColumn != CubeCountX - 1)
		{
			Iter->Target = LastMadeChunk;
			//iterate in approximate normal direction
			Iter->Iterate((NewChunk->Position - LastMadeChunk->Position).GetUnsafeNormal());

			ConnectAdjacentChunks(Iter->Target, NewChunk);
		}
		else
		{
			Iter->Target = LastMadeChunk;
			//iterate in approximate normal direction
			Iter->Iterate(FVector(0, 1, 0));

			ConnectAdjacentChunks(Iter->Target, NewChunk);
		}

		Chunks.Add(NewChunk);

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

FGridIterator* AGrid::GetIterator()
{
	FGridIterator* NewIter = new FGridIterator();

	NewIter->Target = StartChunk;

	return NewIter;
}

FGridChunk* AGrid::GetChunkNearest(FVector position)
{
	FGridIterator* NewIter = new FGridIterator();

	NewIter->Target = StartChunk;

	bool NearPosition = false;
	FGridChunk* Chunk = StartChunk;
	while (!NearPosition)
	{
		FVector VectorTo = position - NewIter->Target->Position;
		FVector ChunkBorderPosTowardsPos = VectorTo.GetUnsafeNormal() * GameLevel->ChunkRootCM;
		if (VectorTo.Size() < ChunkBorderPosTowardsPos.Size())
		{
			NewIter->Iterate(NextDirectionTowards(position));
			Chunk = NewIter->Target;
			NearPosition = true;
		}
	}
	return Chunk;
}

FVector AGrid::NextDirectionTowards(FVector direction)
{
	if (FMath::Abs(direction.X) > FMath::Abs(direction.Y))
	{
		if (direction.X > 0)
			return IterDirections.FindRef(Directions::Right);
		else
			return IterDirections.FindRef(Directions::Left);
	}
	else
	{
		if (direction.Y > 0)
			return IterDirections.FindRef(Directions::Forwards);
		else
			return IterDirections.FindRef(Directions::Backwards);
	}
}

FGridChunkEdge* AGrid::GetRandomValidEdge()
{
	int RandNum;
	bool bValid = false;

	FGridIterator* NewIter = new FGridIterator();
	FGridChunkEdge* Edge = nullptr;
	int EdgeCount = NewIter->Target->Edges.Num();
	int TryCount = 0;
	while(!bValid)
	{
		if (TryCount >= EdgeCount)
			return nullptr;

		RandNum = FMath::RandRange(0, NewIter->Target->Edges.Num() - 1);
		Edge = NewIter->Target->Edges[RandNum];

		if (Edge && Edge->Target)
			bValid = true;

		TryCount++;
	}
	return Edge;
}

//only works if axis aligned with the unreal world.
void AGrid::ConnectAdjacentChunks(FGridChunk* Chunk1, FGridChunk* Chunk2)
{
	FVector C1ToC2 = Chunk2->Position - Chunk1->Position;
	C1ToC2 = C1ToC2.GetUnsafeNormal();

	auto C1 = Chunk1->Edges.begin();
	auto C1End = Chunk2->Edges.end();
	auto C2 = Chunk1->Edges.begin();
	auto C2End = Chunk2->Edges.end();

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
	for (auto i = Chunk1->Edges.begin(); i != Chunk1->Edges.end(); ++i)
	{
		if (DirectionTo.Dot((*i)->Normal) > 0.9f)
			return (*i)->Target == Chunk2 ? true : false;
	}
	return false;
}

bool AGrid::IsChunkWithinRoomBounds(ARoom* Room, FGridChunk* Chunk)
{
	FVector RoomWorldPosition;
	FVector RoomExtent;
	GetActorBounds(false, RoomWorldPosition, RoomExtent, true);
	bool bChunkIsWithinXAxis = Chunk->Position.X < RoomWorldPosition.X + RoomExtent.X && Chunk->Position.X > RoomWorldPosition.X - RoomExtent.X ? true : false;
	bool bChunkIsWithinYAxis = Chunk->Position.Y < RoomWorldPosition.Y + RoomExtent.Y && Chunk->Position.Y > RoomWorldPosition.Y - RoomExtent.Y ? true : false;
	return bChunkIsWithinXAxis && bChunkIsWithinYAxis;
}

bool AGrid::ShouldBeReserved(FGridChunk* Chunk, ARoom* Room)
{
	return !(Chunk->bVisited) && Chunk != nullptr && IsChunkWithinRoomBounds(Room, Chunk);
}

void AGrid::AddRoom(ARoom* room)
{
	FGridIterator* NewIter = new FGridIterator();
	FGridChunk* LastChunk = nullptr;
	int RoomChunksCount = room->BoundsArea / GameLevel->ChunkRootCM;
	for (float ChunksVisited = 0; ChunksVisited >= RoomChunksCount; ChunksVisited)
	{
		if (ChunksVisited == 0)
			NewIter->Target = GetChunkNearest(room->GetActorLocation());
		else
		{
			int DirectionsToTryCount = 4;
			bool bDirectionChoiceResolved = false;

			while (!bDirectionChoiceResolved)
			{
				int TryStage = 1;
				switch (TryStage)
				{
				case 1:
					NewIter->Iterate(IterDirections.FindRef(Directions::Right));
					TryStage++;
					break;
				case 2:
					NewIter->Iterate(IterDirections.FindRef(Directions::Forwards));
					TryStage++;
					break;
				case 3:
					NewIter->Iterate(IterDirections.FindRef(Directions::Backwards));
					TryStage++;
					break;
				case 4:
					NewIter->Iterate(IterDirections.FindRef(Directions::Left));
					TryStage++;
					break;
				default:
					if (LastChunk->Previous)
					{
						NewIter->Target = LastChunk->Previous;
						break;
					}
					if (LastChunk != GetChunkNearest(room->GetActorLocation()))
					{
						NewIter->Target = GetChunkNearest(room->GetActorLocation());
						break;
					}
					else
						//failed to find all chunks in room
						__debugbreak();
				}
			}

			if (!ShouldBeReserved(NewIter->Target, room))
				continue;

			bDirectionChoiceResolved = true;
		}
		NewIter->Target->bVisited = true;
		//room should already be spawned/generated
		NewIter->Target->bSpawned = true;

		Chunks.Add(NewIter->Target);
		ChunksVisited++;

		NewIter->Target->Previous = LastChunk; /*nullptr if first time*/
		LastChunk = NewIter->Target;
	}
}

void AGrid::CarvePassageways(float MaxArea)
{
	if (!StartChunk) return;
		
	FGridIterator* NewIter = new FGridIterator();

	NewIter->Target = StartChunk;
	//finds a unvisited chunk
	while (NewIter->Target->bVisited)
		NewIter->Iterate(NextDirectionTowards(GameLevel->GetActorLocation()));

	float TotalVisitedArea = 0;
	float SqArea = GameLevel->ChunkRootCM * GameLevel->ChunkRootCM;
	TArray<FGridChunkEdge*> AttemptedEdges = TArray<FGridChunkEdge*>();
	while (TotalVisitedArea < MaxArea)
	{
		if (NewIter->Target->Edges.Num() == 0 || AttemptedEdges.Num() == NewIter->Target->Edges.Num())
			NewIter->Target = NewIter->Target->Previous;

		FGridChunkEdge* Edge = GetRandomValidEdge();
		if (!Edge) continue;
		FGridChunkEdge EdgePast;
		Edge->Target->GetEdge(EdgePast, Edge->Normal);
		if (Edge->Target && EdgePast.Target && !Edge->Target->bVisited && !EdgePast.Target->bVisited)
		{
			Edge->Target->bVisited = true;
			TotalVisitedArea += SqArea;

			NewIter->Target = Edge->Target;

			if (!AttemptedEdges.IsEmpty())
				AttemptedEdges.Empty();
		}
		else
			if (!Edge->Target)
			{
				NewIter->Target->Edges.Remove(Edge);
				delete Edge;
			}
			else
				AttemptedEdges.Add(Edge);
	}

}

void AGrid::ConnectDoorways()
{
	if (GameLevel->RoomInstances.IsEmpty())
		return;

	FGridIterator* NewIter = new FGridIterator();

	for (auto i = GameLevel->RoomInstances.begin(); i != GameLevel->RoomInstances.end(); ++i)
	{
		ARoom* CurrentRoom = (*i);
		for (auto j = CurrentRoom->DoorLocations.begin(); j != CurrentRoom->DoorLocations.end(); ++j)
		{
			NewIter->Target = GetChunkNearest(*j);

			for (auto l = NewIter->Target->Edges.begin(); l != NewIter->Target->Edges.end(); ++l)
			{
				FGridChunkEdge PassagewayEdge;
				(*l)->Target->GetEdge(PassagewayEdge, (*l)->Normal);
				bool bPasswayDetected = PassagewayEdge.Target->bVisited;
				if (!IsChunkWithinRoomBounds(CurrentRoom, (*l)->Target) && bPasswayDetected)
				{
					(*l)->Target->bVisited = true;
					NewIter->Target->bVisited = true;
				}
			}
		}
	}
}
//cleans up the grid structure before spawning assets
void AGrid::Cleanup()
{
	TArray<FGridChunkEdge*> SafeEdges = TArray<FGridChunkEdge*>();
	//does not use FGridIterator since it iterates through navigable paths
	for (int i = 0; i < Chunks.Num(); i++)
	{
		for (FGridChunkEdge* Edge : Chunks[i]->Edges)
			if (Edge->Target && Edge->Target->bVisited)
			{
				SafeEdges.Add(Edge);
			}
		Chunks[i]->Edges.Empty();
		Chunks[i]->Edges.Append(SafeEdges);
	}
}
//call cleanup first
//chooses randomly between passageway templates.
void AGrid::SpawnAssets()
{
	Cleanup();
	//for (FGridChunk* Chunk : Chunks)
	//{
	//	int RandNum = FMath::RandRange(0, GameLevel->PassagewayTemplates.Num() - 1);
	//	UPassagewayDataAsset* Template = GameLevel->PassagewayTemplates[RandNum];

	//	UWorld* World = GetWorld();
	//	UClass* ActorSubclass = Template->FloorAsset;
	//	AActor* Floor = World->SpawnActor<AActor>(ActorSubclass, Chunk->Position, FRotator::ZeroRotator);
	//	ActorSubclass = Template->CeilingAsset;
	//	if (GameLevel->bGenerateCeilingsForRoomsAndPassageways && ActorSubclass != nullptr)
	//		World->SpawnActor<AActor>(ActorSubclass, Chunk->Position, FRotator::ZeroRotator);
	//	FVector Direction = FVector::Zero();
	//	for (int i = 0; i < 4; i++)
	//	{
	//		Direction = FVector::Zero();
	//		switch (i)
	//		{
	//		case 0:
	//			Direction = IterDirections.FindRef(Directions::Forwards);
	//			break;
	//		case 1:
	//			Direction = IterDirections.FindRef(Directions::Backwards);
	//			break;
	//		case 2:
	//			Direction = IterDirections.FindRef(Directions::Left);
	//			break;
	//		case 3:
	//			Direction = IterDirections.FindRef(Directions::Right);
	//			break;
	//		}

	//		FGridChunkEdge Edge;
	//		Chunk->GetEdge(Edge, Direction);

	//		//if we can not navigate in this direction, place an asset
	//		if (Edge.Target == nullptr)
	//		{
	//			FActorSpawnParameters Params = FActorSpawnParameters();
	//			Params.Owner = Floor;

	//			FVector NewLocation = Chunk->Position + (Direction * (GameLevel->ChunkRootCM / 2));
	//			FRotator NewRotation = FRotationMatrix::MakeFromX((Chunk->Position - NewLocation).GetUnsafeNormal()).Rotator();

	//			ActorSubclass = Template->WallAsset;
	//			World->SpawnActor<AActor>(ActorSubclass, NewLocation, NewRotation, Params);
	//		}
	/*	}
	}*/
}

FVector AGrid::FindChunkCountExtentFromOrigin(FVector OriginPosition)
{
	FGridChunk* OriginChunk = GetChunkNearest(OriginPosition);
	
	FGridIterator* NewIter = new FGridIterator();

	NewIter->Target = OriginChunk;
	// 1 for origin
	int ChunkCountX = 1;
	int ChunkCountY = 1;

	int AxisChunksCount = 0;
	int DirectionCount = 0;

	//count xNeg
	NewIter->Target = OriginChunk;
	DirectionCount = ChunkCountDirection(Directions::Left);
	AxisChunksCount += DirectionCount;
	
	//count xPos
	NewIter->Target = OriginChunk;
	DirectionCount = ChunkCountDirection(Directions::Right);
	//take smaller count
	AxisChunksCount = AxisChunksCount > DirectionCount ? DirectionCount : AxisChunksCount;

	ChunkCountX += AxisChunksCount;

	AxisChunksCount = 0;
	DirectionCount = 0;

	//count yNeg
	NewIter->Target = OriginChunk;
	DirectionCount = ChunkCountDirection(Directions::Forwards);
	AxisChunksCount += DirectionCount;

	//count yPos
	NewIter->Target = OriginChunk;
	DirectionCount = ChunkCountDirection(Directions::Backwards);
	//take smaller count
	AxisChunksCount = AxisChunksCount > DirectionCount ? DirectionCount : AxisChunksCount;

	ChunkCountY += AxisChunksCount;



	return FVector(ChunkCountX, ChunkCountY, 0);
}


int AGrid::ChunkCountDirection(Directions IterDirection)
{
	int Count = 0;
	FGridChunkEdge Edge;
	FGridIterator* NewIter = new FGridIterator();
	//checks if the edge found is valid
	while (NewIter->Iterate(IterDirections.FindRef(IterDirection)) && !Edge.Target->bVisited)
		//checks for target and iterates if found
	{
		//sets edge for next loop check
		NewIter->Target->GetEdge(Edge, IterDirections.FindRef(IterDirection));
		//iteration was valid, iterate.
		Count++;
	}
	return Count;
}


