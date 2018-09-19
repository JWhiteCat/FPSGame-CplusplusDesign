// Fill out your copyright notice in the Description page of Project Settings.

#include "SGameState.h"
#include "SPlayerState.h"
#include "Net/UnrealNetwork.h"


ASGameState::ASGameState()
{
	PrimaryActorTick.bCanEverTick = true;
	SetReplicates(true);
	
	_VictorID = 0;
	IsGameOver = false;
}

void ASGameState::Tick(float DeltaSeconds)
{
	//¼ì²éµÃ·Ö
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{

		APlayerController* PC = It->Get();
		if (PC && PC->PlayerState)
		{
			if (PC->PlayerState->Score >= 60.0f)
			{
				_VictorID = PC->PlayerState->PlayerId;
				IsGameOver = true;
				PC->PlayerState->Score = 0.0f;
			}

		}
	}
}

void ASGameState::GameOver(int32 VictorID)
{

}

void ASGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASGameState, _VictorID);
	DOREPLIFETIME(ASGameState, IsGameOver);
}