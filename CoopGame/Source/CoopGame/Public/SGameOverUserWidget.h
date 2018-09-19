// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SGameOverUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class COOPGAME_API USGameOverUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
	
public:

	virtual bool Initialize() override;

	class UTextBlock* VictorName;
	
};
