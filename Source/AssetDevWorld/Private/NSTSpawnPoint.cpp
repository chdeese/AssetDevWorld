// Fill out your copyright notice in the Description page of Project Settings.


#include "NSTSpawnPoint.h"
#include "NSTActorSpawnGroup.h"

// Sets default values
ANSTSpawnPoint::ANSTSpawnPoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ANSTSpawnPoint::BeginPlay()
{
	Super::BeginPlay();
	
	SpawnActor();
}

bool ANSTSpawnPoint::SpawnActor()
{
	if (!ActorBlueprints)
		return false;

	if (ActorBlueprints->Blueprints.Num() < 1)
		return false;

	// -1 on awake
	if(bRepeatSelect || ActorSpawnIndex < 0)
		ActorSpawnIndex = FMath::RandRange(0, ActorBlueprints->Blueprints.Num() - 1);

	UClass* BP = ActorBlueprints->Blueprints[ActorSpawnIndex].Get();

	AActor* SpawnedActor;
	
	if (IsChildActor())
	{
		FVector ParentPosition = GetParentActor()->GetActorLocation();
		SpawnedActor = GetWorld()->SpawnActor<AActor>(BP, ParentPosition + GetActorLocation(), GetActorRotation(), FActorSpawnParameters());
	}
	else
		SpawnedActor = GetWorld()->SpawnActor<AActor>(BP, GetActorLocation(), GetActorRotation(), FActorSpawnParameters());

	if(!SpawnedActor)
		//return false if failed.
		return false;
	
	return true;
}

// Called every frame
void ANSTSpawnPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (ElapsedSinceSpawn > RepeatCooldown 
		&& bRepeatSpawns == true)
	{
		if(SpawnActor())
			ElapsedSinceSpawn = 0;
	}
	else
		ElapsedSinceSpawn += DeltaTime;
}

