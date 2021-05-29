// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "Blueprint/UserWidget.h"
#include "ABMessageHandler.h"
#include "ABHUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UABHUDWidget : public UUserWidget, public IABMessageHandler
{
	GENERATED_BODY()
	
public:
	virtual void BindMsgHandlerDelegates() override;

	void BindCharacterStat(class UABCharacterStatComponent* CharacterStat);
	void BindPlayerState(class AABPlayerState* PlayerState);

protected:
	virtual void NativeConstruct() override;

private:
	TWeakObjectPtr<class UABCharacterStatComponent> CurrentCharacterStat;
	TWeakObjectPtr<class AABPlayerState> CurrentPlayerState;

	UPROPERTY()
	class UProgressBar* HPBar;
	
	UPROPERTY()
	class UProgressBar* ExpBar;

	UPROPERTY()
	class UTextBlock* PlayerName;

	UPROPERTY()
	class UTextBlock* PlayerLevel;

	UPROPERTY()
	class UTextBlock* CurrentScore;
	
	UPROPERTY()
	class UTextBlock* HighScore;
};
