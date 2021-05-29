// Fill out your copyright notice in the Description page of Project Settings.


#include "ABHUDWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "ABCharacterStatComponent.h"
#include "ABPlayerState.h"
#include "ABMsgEngine.h"


void UABHUDWidget::BindMsgHandlerDelegates()
{
	MH_DEFI(BIND_CHARACTER_STAT)
	{
		MH_INIT(BIND_CHARACTER_STAT);
		////*ABCHECK*/(nullptr != CharacterStat);
		////CurrentCharacterStat = CharacterStat;
		//CharacterStat->MessageHandlerMulticastDelegate(EMessageID::ON_HP_CHANGED).AddLambda([this](FABMessage& InMessage)->void {
		//	ABCHECK(CurrentCharacterStat.IsValid());
		//	GET_HP_RATIO GHRMessage;
		//	GHRMessage.ReceiverID = CurrentCharacterStat->GetUniqueID();
		//	ABMsgEngine::SendMessage(GHRMessage);

		//	HPBar->SetPercent(GHRMessage.HPRatio);
		//});
	}MH_DEFI_END;

	MH_DEFI(BIND_PLAYER_STATE)
	{
		MH_INIT(BIND_PLAYER_STATE);

	}MH_DEFI_END;
}

void UABHUDWidget::BindCharacterStat(class UABCharacterStatComponent* CharacterStat)
{
	ABCHECK(nullptr != CharacterStat);
	CurrentCharacterStat = CharacterStat;
	CharacterStat->MessageHandlerMulticastDelegate(EMessageID::ON_HP_CHANGED).AddLambda([this](FABMessage& InMessage)->void {
		ABCHECK(CurrentCharacterStat.IsValid());
		GET_HP_RATIO GHRMessage;
		GHRMessage.ReceiverID = CurrentCharacterStat->GetUniqueID();
		ABMsgEngine::SendMessage(GHRMessage);

		HPBar->SetPercent(GHRMessage.HPRatio);
	});
}

void UABHUDWidget::BindPlayerState(class AABPlayerState* PlayerState)
{
	ABCHECK(nullptr != PlayerState);
	CurrentPlayerState = PlayerState;

	PlayerState->MessageHandlerMulticastDelegate(EMessageID::ON_PLAYER_STATE_CHANGED).AddLambda([this](FABMessage& InMessage)->void {
		ABCHECK(CurrentPlayerState.IsValid());

		GET_PLAYER_STATE GPSMessage;
		GPSMessage.ReceiverID = CurrentPlayerState->GetUniqueID();
		ABMsgEngine::SendMessage(GPSMessage);

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

