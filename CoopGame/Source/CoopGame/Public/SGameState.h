// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "SGameState.generated.h"

/**
 * 
 */
UCLASS()
class COOPGAME_API ASGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:

	ASGameState();
	
public:

	virtual void Tick(float DeltaSeconds) override;

	void GameOver(int32 VictorID);
	
	UPROPERTY(Replicated, BlueprintReadWrite)
	bool IsGameOver;

	UPROPERTY(Replicated, BlueprintReadWrite)
	int32 _VictorID;
	
};
