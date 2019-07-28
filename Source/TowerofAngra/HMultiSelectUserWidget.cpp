// Fill out your copyright notice in the Description page of Project Settings.

#include "HMultiSelectUserWidget.h"
#include "Components/Button.h"

void UHMultiSelectUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	WarriorButton = Cast<UButton>(GetWidgetFromName(TEXT("btnWarrior")));
	
	AxeButton = Cast<UButton>(GetWidgetFromName(TEXT("btnAxe")));

	BackButton = Cast<UButton>(GetWidgetFromName(TEXT("btnBack")));

	WarriorButton->OnClicked.AddDynamic(this, &UHMultiSelectUserWidget::OnWorriorButtonClicked);

	AxeButton->OnClicked.AddDynamic(this, &UHMultiSelectUserWidget::OnAxeButtonClicked);

	BackButton->OnClicked.AddDynamic(this, &UHMultiSelectUserWidget::OnBackButtonClicked);

	
}

void UHMultiSelectUserWidget::OnWorriorButtonClicked()
{
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("Stage2"));

	PlayerType = EPlayerType::WARRIOR;
}

void UHMultiSelectUserWidget::OnAxeButtonClicked()
{
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("Stage2"));

	PlayerType = EPlayerType::AXE;
}

void UHMultiSelectUserWidget::OnBackButtonClicked()
{
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("Title"));
}
