// Fill out your copyright notice in the Description page of Project Settings.


#include "ABHUDWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "ABCharacterStatComponent.h"
#include "ABPlayerState.h"
#include "ABMsgEngine.h"

void UABHUDWidget::BindCharacterStat(class UABCharacterStatComponent* CharacterStat)
{
	ABCHECK(nullptr != CharacterStat);
	CurrentCharacterStat = CharacterStat;
	CurrentCharacterStat->MulticastMessageHandlerDelegate(EMessageID::ON_HP_CHANGED).AddLambda([this](FABMessage& InMessage)->void {
		ABCHECK(nullptr != HPBar);

		GET_HP_RATIO GHRMessage;
		GHRMessage.ReceiverID = CurrentCharacterStat->GetUniqueID();
		FABMsgEngine::SendMessage(GHRMessage);

		HPBar->SetPercent(GHRMessage.HPRatio);
	});
}

void UABHUDWidget::BindPlayerState(class AABPlayerState* PlayerState)
{
	ABCHECK(nullptr != PlayerState);
	CurrentPlayerState = PlayerState;

	PlayerState->MulticastMessageHandlerDelegate(EMessageID::ON_PLAYER_STATE_CHANGED).AddLambda([this](FABMessage& InMessage)->void {
		GET_PLAYER_STATE GPSMessage;
		GPSMessage.ReceiverID = CurrentPlayerState->GetUniqueID();
		FABMsgEngine::SendMessage(GPSMessage);

		ExpBar->SetPercent(GPSMessage.ExpRatio);
		PlayerName->SetText(FText::FromString(GPSMessage.PlayerName));
		PlayerLevel->SetText(FText::FromString(FString::FromInt(GPSMessage.CharacterLevel)));
		CurrentScore->SetText(FText::FromString(FString::FromInt(GPSMessage.GameScore)));
		HighScore->SetText(FText::FromString(FString::FromInt(GPSMessage.GameHightScore)));
	});
}

void UABHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();
	HPBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("pbHP")));
	ABCHECK(nullptr != HPBar);

	ExpBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("pbExp")));
	ABCHECK(nullptr != ExpBar);

	PlayerName = Cast<UTextBlock>(GetWidgetFromName(TEXT("txtPlayerName")));
	ABCHECK(nullptr != PlayerName);

	PlayerLevel = Cast<UTextBlock>(GetWidgetFromName(TEXT("txtLevel")));
	ABCHECK(nullptr != PlayerLevel);

	CurrentScore = Cast<UTextBlock>(GetWidgetFromName(TEXT("txtCurrentScore")));
	ABCHECK(nullptr != CurrentScore);

	HighScore = Cast<UTextBlock>(GetWidgetFromName(TEXT("txtHighScore")));
	ABCHECK(nullptr != HighScore);
}

