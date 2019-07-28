// Fill out your copyright notice in the Description page of Project Settings.

#include "HMultiSelectUserWidget.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"

void UHMultiSelectUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	WarriorButton = Cast<UButton>(GetWidgetFromName(TEXT("btnWarrior")));
	
	AxeButton = Cast<UButton>(GetWidgetFromName(TEXT("btnAxe")));

	BackButton = Cast<UButton>(GetWidgetFromName(TEXT("btnBack")));

	InputButton = Cast<UButton>(GetWidgetFromName(TEXT("btnInput")));

	TextBox = Cast<UEditableTextBox>(GetWidgetFromName(TEXT("IPaddrTextBox")));
	
	WarriorButton->OnClicked.AddDynamic(this, &UHMultiSelectUserWidget::OnWorriorButtonClicked);

	AxeButton->OnClicked.AddDynamic(this, &UHMultiSelectUserWidget::OnAxeButtonClicked);

	BackButton->OnClicked.AddDynamic(this, &UHMultiSelectUserWidget::OnBackButtonClicked);

	InputButton->OnClicked.AddDynamic(this, &UHMultiSelectUserWidget::OnInputButtonClicked);

	
	CurrentGameMode = EGameMode::MULTI_GAME;
}

void UHMultiSelectUserWidget::OnWorriorButtonClicked()
{
	PlayerType = EPlayerType::WARRIOR;
	
}

void UHMultiSelectUserWidget::OnAxeButtonClicked()
{
	PlayerType = EPlayerType::AXE;
}

void UHMultiSelectUserWidget::OnBackButtonClicked()
{
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("Title"));
}

void UHMultiSelectUserWidget::OnInputButtonClicked()
{
	IPaddress = TextBox->GetText().ToString();

	if (IPaddress.Len() <= 0 || IPaddress.Len() > 15) return;
	if (PlayerType == EPlayerType::NONE) return;

	UGameplayStatics::OpenLevel(GetWorld(), TEXT("Stage2"));
}
