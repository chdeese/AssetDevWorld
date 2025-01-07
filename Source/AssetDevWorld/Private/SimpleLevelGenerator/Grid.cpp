// Fill out your copyright notice in the Description page of Project Settings.


#include "SimpleLevelGenerator/Grid.h"
#include "SimpleLevelGenerator/Room.h"

// Called when the game starts or when spawned
void AGrid::BeginPlay()
{
	Super::BeginPlay();
	
}

AGrid::AGrid()
{

}

void AGrid::init(int width, int length)
{
	int cubeCountX = width / ChunkRootCM;
	int cubeCountY = length / ChunkRootCM;

	bool bIsXCountEven = cubeCountX % 2 != 0;
	bool bIsYCountEven = cubeCountY % 2 != 0;
	//forces cubeCountX to be an even number
	cubeCountX = bIsXCountEven ? cubeCountX : cubeCountX - 1;
	//forces cubeCountY to be an even number
	cubeCountY = bIsYCountEven ? cubeCountY : cubeCountY - 1;


}

//AGrid::AGrid(int width, int length)
//{
//}
//
//AGrid::AGrid(int width, int height, int length)
//{
//}


