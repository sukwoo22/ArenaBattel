// Fill out your copyright notice in the Description page of Project Settings.


#include "ABMessageHandler.h"

void FABSiglecastMessageHandler::HandleMessage(FABMessage& Message)
{
	auto MsgHandlerDelegate = SinglecastMessageHandlerDelegateMap.Find(Message.ID);
	if (nullptr != MsgHandlerDelegate)
	{
		MsgHandlerDelegate->Execute(Message);
	}
}

FABSinglecastMessageHandlerDelegate& FABSiglecastMessageHandler::SinglecastMessageHandlerDelegate(EMessageID MessageID)
{
	return SinglecastMessageHandlerDelegateMap.FindOrAdd(MessageID);
}

TMap<EMessageID, FABSinglecastMessageHandlerDelegate>& FABSiglecastMessageHandler::GetSinglecastMessageHandlerDelegateMap()
{
	return SinglecastMessageHandlerDelegateMap;
}

void FABMulticastMessageHandler::HandleMessage(FABMessage& Message)
{
	auto MsgHandlerDelegate = GetSinglecastMessageHandlerDelegateMap().Find(Message.ID);
	if (nullptr != MsgHandlerDelegate)
	{
		MsgHandlerDelegate->Execute(Message);
	}
	else
	{
		auto MulticastMsgHandlerDelegate = MulticastMessageHandlerDelegateMap.Find(Message.ID);
		if (nullptr != MulticastMsgHandlerDelegate)
		{
			MulticastMsgHandlerDelegate->Broadcast(Message);
		}
	}
}

FABMulticastMsgHandlerDelegate& FABMulticastMessageHandler::MulticastMessageHandlerDelegate(EMessageID MessageID)
{
	return MulticastMessageHandlerDelegateMap.FindOrAdd(MessageID);
}