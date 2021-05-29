// Fill out your copyright notice in the Description page of Project Settings.


#include "ABCharacterWidget.h"
#include "ABCharacterStatComponent.h"
#include "Components/ProgressBar.h"
#include "ABMsgEngine.h"

void UABCharacterWidget::BindCharacterStat(class UABCharacterStatComponent* NewCharacterStat)
{
	ABCHECK(nullptr != NewCharacterStat);

	CurrentCharacterStat = NewCharacterStat;
	NewCharacterStat->MessageHandlerMulticastDelegate(EMessageID::ON_HP_CHANGED).AddLambda([this](FABMessage& InMessage)->void {
		if (CurrentCharacterStat.IsValid())
		{
			if (nullptr != HPProgressBar)
			{
				GET_HP_RATIO GHRMessage;
				GHRMessage.ReceiverID = CurrentCharacterStat->GetUniqueID();
				ABMsgEngine::SendMessage(GHRMessage);

				HPProgressBar->SetPercent(GHRMessage.HPRatio);
				//HPProgressBar->SetPercent(CurrentCharacterStat->GetHPRatio());
			}
		}
	});
}

void UABCharacterWidget::NativeConstruct()
{
	Super::NativeConstruct();
	HPProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("PB_HPBar")));
	ABCHECK(nullptr != HPProgressBar);
	UpdateHPWidget();
}

void UABCharacterWidget::UpdateHPWidget()
{
	if (CurrentCharacterStat.IsValid())
	{
		if (nullptr != HPProgressBar)
		{
			GET_HP_RATIO GHRMessage;
			GHRMessage.ReceiverID = CurrentCharacterStat->GetUniqueID();
			ABMsgEngine::SendMessage(GHRMessage);

			HPProgressBar->SetPercent(GHRMessage.HPRatio);
			//HPProgressBar->SetPercent(CurrentCharacterStat->GetHPRatio());
		}
	}
}
