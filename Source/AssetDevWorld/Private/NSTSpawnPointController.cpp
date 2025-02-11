// Fill out your copyright notice in the Description page of Project Settings.


#include "NSTSpawnPointController.h"
#include "NSTSpawnPoint.h"
#include "NSTActorSpawnGroup.h"

void ANSTSpawnPointController::CombineAllGroups()
{
	if (SpawnPoints.Num() < 1 || SpawnGroups.Num() < 1)
		return;

	TArray<TSubclassOf<AActor>> CombinedGroup = TArray<TSubclassOf<AActor>>();

	for (UNSTActorSpawnGroup* Group : SpawnGroups)
		CombinedGroup.Append(Group->Blueprints);

	for (ANSTSpawnPoint* Point : SpawnPoints)
		Point->ActorBlueprints->Blueprints = CombinedGroup;
}

void ANSTSpawnPointController::ForceSetAllCooldowns()
{
	for (ANSTSpawnPoint* Point : SpawnPoints)
		Point->RepeatCooldown = ForcedCooldownLength;
}

void ANSTSpawnPointController::StopAllRepeating()
{
	for (ANSTSpawnPoint* Point : SpawnPoints)
		Point->bRepeatSpawns = false;
}

void ANSTSpawnPointController::StopAllRepeatSelection()
{
	for (ANSTSpawnPoint* Point : SpawnPoints)
		Point->bRepeatSelect = false;
}

void ANSTSpawnPointController::ForceAllRepeat()
{
	for (ANSTSpawnPoint* Point : SpawnPoints)
		Point->bRepeatSpawns = true;
}

void ANSTSpawnPointController::ForceAllRepeatSelection()
{
	for (ANSTSpawnPoint* Point : SpawnPoints)
		Point->bRepeatSelect = true;
}