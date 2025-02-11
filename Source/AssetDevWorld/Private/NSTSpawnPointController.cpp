// Fill out your copyright notice in the Description page of Project Settings.


#include "NSTSpawnPointController.h"
#include "Net/UnrealNetwork.h"
#include "NSTSpawnPoint.h"
#include "NSTActorSpawnGroup.h"

void ANSTSpawnPointController::GetLifetimeReplicatedProps(TArray <FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    //Replicate current health.
    DOREPLIFETIME(ANSTSpawnPointController, bSingleAppendedGroup);
}

// Sets default values
ANSTSpawnPointController::ANSTSpawnPointController()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ANSTSpawnPointController::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ANSTSpawnPointController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ANSTSpawnPointController::OnRep_AppendGroups()
{
	if (!bTryAppendGroups)
		return;

	AppendGroups();
	bTryAppendGroups = false;
}		

bool ANSTSpawnPointController::AppendGroups()
{
	if (SpawnPoints.Num() < 1 || SpawnGroups.Num() < 1)
		return false;

	UNSTActorSpawnGroup AppendedGroup = UNSTActorSpawnGroup();

	for (int i = 0; i < SpawnGroups.Num(); i++)
	{
		AppendedGroup.Blueprints.Append(SpawnGroups[i]->Blueprints);
	}
	for (int i = 0; i < SpawnPoints.Num(); i++)
	{
		SpawnPoints[i]->ActorBlueprints->Blueprints = AppendedGroup.Blueprints;
	}

	return true;
}
