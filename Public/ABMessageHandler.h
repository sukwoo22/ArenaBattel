// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "UObject/Interface.h"
#include "ABMessage.h"
#include "ABMessageHandler.generated.h"

DECLARE_DELEGATE_OneParam(FABMessageHandlerDelegate, FABMessage&);
DECLARE_MULTICAST_DELEGATE_OneParam(FABMessageHandlerMulticastDelegate, FABMessage&);

UINTERFACE(MinimalAPI)
class UABMessageHandler : public UInterface
{
	GENERATED_BODY()
};

class ARENABATTLE_API IABMessageHandler
{
	GENERATED_BODY()
public:

	virtual void HandleMessage(FABMessage& Message)
	{
		if (MessageHandlerDelegateMap.Contains(Message.ID))
		{
			MessageHandlerDelegateMap[Message.ID].Execute(Message);
		}
	}

	virtual void BindMsgHandlerDelegates() = 0;

	FABMessageHandlerDelegate& MessageHandlerDelegate(EMessageID MessageID)
	{
		return MessageHandlerDelegateMap.FindOrAdd(MessageID);
	}

	bool IsBindingCmdZero()
	{
		return MessageHandlerDelegateMap.Num() ? false : true;
	}

protected:
	TMap<EMessageID, FABMessageHandlerDelegate>& GetMessageHandlerDelegateMap()
	{
		return MessageHandlerDelegateMap;
	}

private:
	TMap<EMessageID, FABMessageHandlerDelegate> MessageHandlerDelegateMap;
};

class ARENABATTLE_API ABMulticastMessageHandler : public IABMessageHandler
{
	
public:

	virtual void HandleMessage(FABMessage& Message)
	{
		auto& MsgHandlerDelegateMap = GetMessageHandlerDelegateMap();

		if (MsgHandlerDelegateMap.Contains(Message.ID))
		{
			MsgHandlerDelegateMap[Message.ID].Execute(Message);
		}
		else if (MessageHandlerMulticastDelegateMap.Contains(Message.ID))
		{
			MessageHandlerMulticastDelegateMap[Message.ID].Broadcast(Message);
		}
	}

	virtual void BindMsgHandlerDelegates() = 0;

	FABMessageHandlerMulticastDelegate& MessageHandlerMulticastDelegate(EMessageID MessageID)
	{
		return MessageHandlerMulticastDelegateMap.FindOrAdd(MessageID);
	}

private:
	TMap<EMessageID, FABMessageHandlerMulticastDelegate> MessageHandlerMulticastDelegateMap;
};