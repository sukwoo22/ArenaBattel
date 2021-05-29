// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "GameFramework/PlayerState.h"
#include "ABMessageHandler.h"
#include "ABPlayerState.generated.h"


/**
 * 
 */
UCLASS()
class ARENABATTLE_API AABPlayerState : public APlayerState, public ABMulticastMessageHandler
{
	GENERATED_BODY()
	
public:
	AABPlayerState();

	virtual void BindMsgHandlerDelegates() override;

	FString SaveSlotName;

private:
	void SetCharacterLevel(int32 NewCharacterLevel);
	struct FABCharacterData* CurrentStatData;
	
private:
	UPROPERTY(Transient)
	int32 GameScore;

	UPROPERTY(Transient)
	int32 GameHighScore;

	UPROPERTY(Transient)
	int32 CharacterLevel;

	UPROPERTY(Transient)
	int32 Exp;

	UPROPERTY(Transient)
	int32 CharacterIndex;
};
