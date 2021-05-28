// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "UObject/Interface.h"
#include "ABMessage.h"
#include "ABMessageHandler.generated.h"

DECLARE_DELEGATE_OneParam(FABMessageHandlerDelegate, FABMessage&);

UINTERFACE(MinimalAPI)
class UABMessageHandler : public UInterface
{
	GENERATED_BODY()
};

class ARENABATTLE_API IABMessageHandler
{
	GENERATED_BODY()
public:
	/*static IABTelegramUser* Get()
	{
		static IABTelegramUser* TelegramUser = new IABTelegramUser();
		return TelegramUser;
	}*/

	virtual void HandleMessage(FABMessage& Message)
	{
		auto MessageHandlerDelegate = MessageHandlerDelegateMap.Find(Message.ID);

		if (nullptr != MessageHandlerDelegate)
		{
			MessageHandlerDelegate->Execute(Message);
		}
	}

	virtual void BindMsgHandlerDelegates() = 0;

	FABMessageHandlerDelegate& operator[](EMessageID MessageID)
	{
		return MessageHandlerDelegateMap.FindOrAdd(MessageID);
	}

	FABMessageHandlerDelegate& MessageHandlerDelegate(EMessageID MessageID)
	{
		return MessageHandlerDelegateMap.FindOrAdd(MessageID);
	}

	bool IsBindingCmdZero()
	{
		return MessageHandlerDelegateMap.Num() ? false : true;
	}

private:
	/*ABTelegramUser() = default;
	ABTelegramUser(const ABTelegramUser&) = delete;
	ABTelegramUser(ABTelegramUser&&) = delete;
	ABTelegramUser& operator=(const ABTelegramUser&) = delete;
	ABTelegramUser& operator=(ABTelegramUser&&) = delete;*/

private:
	TMap<EMessageID, FABMessageHandlerDelegate> MessageHandlerDelegateMap;
};

