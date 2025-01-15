// Fill out your copyright notice in the Description page of Project Settings.

#include "SimpleLevelGenerator/Grid.h"
#include "SimpleLevelGenerator/Room.h"
#include "SimpleLevelGenerator/GameLevel.h"
#include "SimpleLevelGenerator/PassagewayDataAsset.h"



AGrid::Directions AGrid::RandomDirection(int Dimensions = 0)
{
	if (Dimensions != 2 && Dimensions != 3) 
	{
		__debugbreak();
		return Directions::Backwards;
	}
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
	Iterator = new FGridIterator();
	Chunks = TArray<FGridChunk*>();
}

AGrid::~AGrid()
{
	delete Iterator;
	//delete struct stuff.
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
			newChunk->Edges = TArray<FGridChunkEdge*>();

			//right
			FGridChunkEdge* newEdge = new FGridChunkEdge();
			newEdge->Target = nullptr;
			newEdge->Normal = FVector(1, 0, 0);
			newChunk->Edges.Add(newEdge);

			//left
			newEdge = new FGridChunkEdge();
			newEdge->Target = nullptr;
			newEdge->Normal = FVector(-1, 0, 0);
			newChunk->Edges.Add(newEdge);

			//forwards
			newEdge = new FGridChunkEdge();
			newEdge->Target = nullptr;
			newEdge->Normal = FVector(0, 1, 0);
			newChunk->Edges.Add(newEdge);

			//backwards
			newEdge = new FGridChunkEdge();
			newEdge->Target = nullptr;
			newEdge->Normal = FVector(0, -1, 0);
			newChunk->Edges.Add(newEdge);
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

FGridIterator* AGrid::GetIterator()
{
	if (!Iterator->Target)
		Iterator->Target = StartChunk;
	return Iterator;
}

FGridChunk* AGrid::GetChunkNearest(FVector position)
{
	Iterator->Target = StartChunk;

	bool NearPosition = false;
	FGridChunk* Chunk = StartChunk;
	while (!NearPosition)
	{
		FVector VectorTo = position - Iterator->Target->Position;
		FVector ChunkBorderPosTowardsPos = VectorTo.GetUnsafeNormal() * ChunkRootCM;
		if (VectorTo.Size() < ChunkBorderPosTowardsPos.Size())
		{
			Iterator->Iterate(NextDirectionTowards(position));
			Chunk = Iterator->Target;
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
			return IterDirections.FindRef(Directions::Up);
		else
			return IterDirections.FindRef(Directions::Down);
	}
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
	bool bChunkIsWithinZAxis = Chunk->Position.Z < RoomWorldPosition.Z + RoomExtent.Z && Chunk->Position.Z > RoomWorldPosition.Z - RoomExtent.Z ? true : false;
	return GameLevel->b3DMode ? bChunkIsWithinXAxis && bChunkIsWithinYAxis && bChunkIsWithinZAxis : bChunkIsWithinXAxis && bChunkIsWithinYAxis;
}

bool AGrid::ShouldBeReserved(FGridChunk* Chunk, ARoom* Room)
{
	return !(Chunk->bVisited) && Chunk != nullptr && IsChunkWithinRoomBounds(Room, Chunk);
}

void AGrid::ReserveChunksInRoom(ARoom* room)
{
	FGridChunk* LastChunk = nullptr;
	int RoomChunksCount = room->BoundsArea / ChunkRootCM;
	for (float ChunksVisited = 0; ChunksVisited >= RoomChunksCount; ChunksVisited)
	{
		if (ChunksVisited == 0)
			Iterator->Target = GetChunkNearest(room->GetActorLocation());
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
					Iterator->Iterate(IterDirections.FindRef(Directions::Right));
					TryStage++;
					break;
				case 2:
					Iterator->Iterate(IterDirections.FindRef(Directions::Up));
					TryStage++;
					break;
				case 3:
					Iterator->Iterate(IterDirections.FindRef(Directions::Down));
					TryStage++;
					break;
				case 4:
					Iterator->Iterate(IterDirections.FindRef(Directions::Left));
					TryStage++;
					break;
				default:
					if (LastChunk->Previous)
					{
						Iterator->Target = LastChunk->Previous;
						break;
					}
					if (LastChunk != GetChunkNearest(room->GetActorLocation()))
					{
						Iterator->Target = GetChunkNearest(room->GetActorLocation());
						break;
					}
					else
						//failed to find all chunks in room
						__debugbreak();
				}
			}

			if (!ShouldBeReserved(Iterator->Target, room))
				continue;

			bDirectionChoiceResolved = true;
		}
		Iterator->Target->bVisited = true;
		//room should already be spawned/generated
		Iterator->Target->bSpawned = true;
		ChunksVisited++;

		Iterator->Target->Previous = LastChunk; /*nullptr if first time*/
		LastChunk = Iterator->Target;
	}
}

void AGrid::CarvePassageways(float MaxArea)
{
	Iterator->Target = StartChunk;
	//finds a unvisited chunk
	while (Iterator->Target->bVisited)
		Iterator->Iterate(NextDirectionTowards(GameLevel->GetActorLocation()));

	float TotalVisitedArea = 0;
	float SqArea = ChunkRootCM * ChunkRootCM;
	TArray<FGridChunkEdge*> AttemptedEdges = TArray<FGridChunkEdge*>();
	while (TotalVisitedArea < MaxArea)
	{
		if (Iterator->Target->Edges.Num() == 0 || AttemptedEdges.Num() == Iterator->Target->Edges.Num())
			Iterator->Target = Iterator->Target->Previous;

		int RandNum = FMath::RandRange(0, Iterator->Target->Edges.Num() - 1);
		FGridChunkEdge* Edge = Iterator->Target->Edges[RandNum];
		FGridChunkEdge EdgePast;
		Edge->Target->GetEdge(EdgePast, Edge->Normal);
		if (Edge->Target && EdgePast.Target && !Edge->Target->bVisited && !EdgePast.Target->bVisited)
		{
			Edge->Target->bVisited = true;
			TotalVisitedArea += SqArea;

			Iterator->Target = Edge->Target;

			if (!AttemptedEdges.IsEmpty())
				AttemptedEdges.Empty();
		}
		else
			if (!Edge->Target)
			{
				Iterator->Target->Edges.Remove(Edge);
				delete Edge;
			}
			else
				AttemptedEdges.Add(Edge);
	}

}

void AGrid::ConnectDoorways()
{
	for (auto i = GameLevel->RoomInstances.begin(); i != GameLevel->RoomInstances.end(); ++i)
	{
		ARoom* CurrentRoom = (*i);
		for (auto j = CurrentRoom->DoorLocations.begin(); j != CurrentRoom->DoorLocations.end(); ++j)
		{
			Iterator->Target = GetChunkNearest(*j);

			for (auto l = Iterator->Target->Edges.begin(); l != Iterator->Target->Edges.end(); ++l)
			{
				FGridChunkEdge PassagewayEdge;
				(*l)->Target->GetEdge(PassagewayEdge, (*l)->Normal);
				bool bPasswayDetected = PassagewayEdge.Target->bVisited;
				if (!IsChunkWithinRoomBounds(CurrentRoom, (*l)->Target) && bPasswayDetected)
				{
					(*l)->Target->bVisited = true;
					Iterator->Target->bVisited = true;
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
	for (FGridChunk* Chunk : Chunks)
	{
		int RandNum = FMath::RandRange(0, GameLevel->PassagewayTemplates.Num() - 1);
		UPassagewayDataAsset* Template = GameLevel->PassagewayTemplates[RandNum];

		UWorld* World = GetWorld();
		UClass* ActorSubclass = Template->FloorAsset;
		AActor* Floor = World->SpawnActor<AActor>(ActorSubclass, Chunk->Position, FRotator::ZeroRotator);
		ActorSubclass = Template->CeilingAsset;
		if (GameLevel->bGenerateCeilingsForRoomsAndPassageways && ActorSubclass != nullptr)
			World->SpawnActor<AActor>(ActorSubclass, Chunk->Position, FRotator::ZeroRotator);
		FVector Direction = FVector::Zero();
		for (int i = 0; i < 4; i++)
		{
			Direction = FVector::Zero();
			switch (i)
			{
			case 0:
				Direction = IterDirections.FindRef(Directions::Forwards);
				break;
			case 1:
				Direction = IterDirections.FindRef(Directions::Backwards);
				break;
			case 2:
				Direction = IterDirections.FindRef(Directions::Left);
				break;
			case 3:
				Direction = IterDirections.FindRef(Directions::Right);
				break;
			}

			FGridChunkEdge Edge;
			Chunk->GetEdge(Edge, Direction);

			//if we can not navigate in this direction, place an asset
			if (Edge.Target == nullptr)
			{
				FActorSpawnParameters Params = FActorSpawnParameters();
				Params.Owner = Floor;

				FVector NewLocation = Chunk->Position + (Direction * (ChunkRootCM / 2));
				FRotator NewRotation = FRotationMatrix::MakeFromX((Chunk->Position - NewLocation).GetUnsafeNormal()).Rotator();

				ActorSubclass = Template->WallAsset;
				World->SpawnActor<AActor>(ActorSubclass, NewLocation, NewRotation, Params);
			}
		}
	}
}


