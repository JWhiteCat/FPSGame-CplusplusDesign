// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SGameMode.generated.h"



DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnActorKilled, AActor*, VictimActor, AActor*, KillerActor, AController*, KillerController);

/**
 *
 */
UCLASS()
class COOPGAME_API ASGameMode : public AGameModeBase
{
	GENERATED_BODY()


protected:

	void CheckAnyPlayerAlive();

	UFUNCTION(BlueprintCallable, Category = "GameMode")
	void GameOver();


public:

	ASGameMode();

	//virtual void StartPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(BlueprintAssignable, Category = "GameMode")
	FOnActorKilled OnActorKilled;

	UFUNCTION(BlueprintCallable, Category = "GameMode")
	bool CheckStringIsValid(const FString str, const FString Reg);

	void RestartDeadPlayers();

	void RestartCurrentPlayer();

	UPROPERTY(BlueprintReadWrite, Category = "GameMode")
	FString Champion;
};
