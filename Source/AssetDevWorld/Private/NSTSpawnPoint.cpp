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
	int RandomIndex = FMath::RandRange(0, ActorBlueprints->Blueprints.Num() - 1);
	UClass* BP = ActorBlueprints->Blueprints[RandomIndex].Get();

	AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(BP, GetActorLocation(), GetActorRotation(), FActorSpawnParameters());
	if(!SpawnedActor)
		//return false if failed.
		return false;
	
	return true;
}

// Called every frame
void ANSTSpawnPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

