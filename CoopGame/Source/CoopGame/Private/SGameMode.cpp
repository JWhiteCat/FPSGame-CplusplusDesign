// Fill out your copyright notice in the Description page of Project Settings.

#include "SGameMode.h"
#include "SHealthComponent.h"
#include "TimerManager.h"
#include "SPlayerState.h"
#include "Regex.h"
#include "SGameOverUserWidget.h"
#include "UObject/ConstructorHelpers.h"

void ASGameMode::CheckAnyPlayerAlive()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();
		if (PC && PC->GetPawn())
		{
			APawn* MyPawn = PC->GetPawn();
			USHealthComponent* HealthComp = Cast<USHealthComponent>(MyPawn->GetComponentByClass(USHealthComponent::StaticClass()));
			if (ensure(HealthComp) && HealthComp->GetHealth() > 0.0f)
			{
				//
				return;
			}
		}
	}

	//No
	//GameOver();
}

void ASGameMode::GameOver()
{
	//´´½¨GameOverWidget
	if (GEngine && GEngine->GameViewport)
	{
		static ConstructorHelpers::FClassFinder<UUserWidget> GameOverUserWidgetBP(TEXT("WidgetBlueprint'/Game/UI/WBP_GameOver.WBP_GameOver_C'"));
		if (GameOverUserWidgetBP.Succeeded())
		{
			UE_LOG(LogTemp, Warning, TEXT("PVEBP Succeed!"));
			GameOverUserWidgetClass = GameOverUserWidgetBP.Class;
		}

		GameOverUserWidget = CreateWidget<USGameOverUserWidget>(GetWorld()->GetGameInstance(), GameOverUserWidgetClass);

		if (GameOverUserWidget != nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("GameOverUserWidget AddToViewPort!"));
			
			GameOverUserWidget->AddToViewport();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("GameOverUserWidget is a nullptr"));
		}
	}
}

void ASGameMode::RestartDeadPlayers()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();
		if (PC && PC->GetPawn() == nullptr)
		{
			RestartPlayer(PC);
		}
	}
}

void ASGameMode::RestartCurrentPlayer()
{
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &ASGameMode::RestartDeadPlayers, 5.0f, false);
}

int32 ASGameMode::CheckScore()
{
	int32 VictoryID = 0;
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		
		APlayerController* PC = It->Get();
		if (PC && PC->PlayerState)
		{
			if (PC->PlayerState->Score >= 60.0f)
			{
				VictoryID = PC->PlayerState->PlayerId;
				PC->PlayerState->Score = 0;
			}
		}	
	}
	return VictoryID;
}

ASGameMode::ASGameMode()
{

	PlayerStateClass = ASPlayerState::StaticClass();

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 1.0f;
}

void ASGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	CheckAnyPlayerAlive();


	/*
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();
		if (PC->GetPawn()->PlayerState->Score >= 100.0f)
		{
			Champion = PC->GetPawn()->PlayerState->GetPlayerName();
			GameOver();
		}
	}
	*/
}


bool ASGameMode::CheckStringIsValid(const FString str, const FString Reg)
{
	FRegexPattern Pattern(Reg);
	FRegexMatcher regMatcher(Pattern, str);
	regMatcher.SetLimits(0, str.Len());
	return regMatcher.FindNext();
}
