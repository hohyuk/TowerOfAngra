// Fill out your copyright notice in the Description page of Project Settings.

#include "HResultUserWidget.h"
#include "Components/TextBlock.h"

void UHResultUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	auto Result = Cast<UTextBlock>(GetWidgetFromName(TEXT("txtResult")));
	
	//Result->SetText(FText::FromString(CurrentGameState->IsGameCleared() ? TEXT("Mission Complete") : TEXT("Mission Failed")));

	auto TotalScore = Cast<UTextBlock>(GetWidgetFromName(TEXT("txtTotalScore")));
	
	//TotalScore->SetText(FText::FromString(FString::FromInt(CurrentGameState->GetTotalGameScore())));
}
