// Fill out your copyright notice in the Description page of Project Settings.

#include "SGameOverUserWidget.h"
#include "TextBlock.h"



bool USGameOverUserWidget::Initialize()
{
	Super::Initialize();
	VictorName = Cast<UTextBlock>(GetWidgetFromName("VictoryName"));

	return true;
}