// Fill out your copyright notice in the Description page of Project Settings.


#include "ABCharacterWidget.h"
#include "ABCharacterStatComponent.h"
#include "Components/ProgressBar.h"
#include "ABMsgEngine.h"

void UABCharacterWidget::BindCharacterStat(class UABCharacterStatComponent* NewCharacterStat)
{
	ABCHECK(nullptr != NewCharacterStat);
	CurrentCharacterStat = NewCharacterStat;
	CurrentCharacterStat->MulticastMessageHandlerDelegate(EMessageID::ON_HP_CHANGED).AddLambda([this](FABMessage& InMessage)->void {
		ABCHECK(nullptr != HPProgressBar);

		GET_HP_RATIO GHRMessage;
		GHRMessage.ReceiverID = CurrentCharacterStat->GetUniqueID();
		FABMsgEngine::SendMessage(GHRMessage);

		HPProgressBar->SetPercent(GHRMessage.HPRatio);
	});
}

void UABCharacterWidget::NativeConstruct()
{
	Super::NativeConstruct();
	HPProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("PB_HPBar")));

	ABCHECK(nullptr != HPProgressBar);

	if (CurrentCharacterStat.IsValid())
	{
		if (nullptr != HPProgressBar)
		{
			GET_HP_RATIO GHRMessage;
			GHRMessage.ReceiverID = CurrentCharacterStat->GetUniqueID();
			FABMsgEngine::SendMessage(GHRMessage);

			HPProgressBar->SetPercent(GHRMessage.HPRatio);
		}
	}
}

