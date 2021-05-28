// Fill out your copyright notice in the Description page of Project Settings.


#include "ABMsgEngine.h"


void ABMsgEngine::AddMsgHandlerInManager(EManagerID ManagerID, AActor* MessageHandler)
{
	ABCHECK(nullptr != MessageHandler);
	auto& MsgHandlerManagerMap = GetMessageHandlerManagerMap();
	auto& MsgHandlerManager = MsgHandlerManagerMap.FindOrAdd(ManagerID);
	MsgHandlerManager.Add(MessageHandler->GetUniqueID(), Cast<IABMessageHandler>(MessageHandler));
}

void ABMsgEngine::DeleteMsgHandlerInManager(EManagerID ManagerID, AActor* MessageHandler)
{
	ABCHECK(nullptr != MessageHandler);
	auto& MsgHandlerManagerMap = GetMessageHandlerManagerMap();
	if (MsgHandlerManagerMap.Contains(ManagerID))
	{
		if (MsgHandlerManagerMap[ManagerID].Contains(MessageHandler->GetUniqueID()))
		{
			MsgHandlerManagerMap[ManagerID].Remove(MessageHandler->GetUniqueID());
		}
	}
}

void ABMsgEngine::SendMessage(FABMessage& Message)
{
	auto& MsgHandlerManagerMap = GetMessageHandlerManagerMap();

	// 리시버가 없을 경우 모두에게 메세지를 보낸다.
	if (-1 == Message.ReceiverID)
	{
		for (auto& Manager : MsgHandlerManagerMap)
		{
			for (auto& Handler : Manager.Value)
			{
				Handler.Value->HandleMessage(Message);
			}
		}
	}
	// 리시버가 있을 경우 해당 리시버한테만 메세지를 보낸다.
	// 방법 1 : 모든 매니저를 돌면서 리시버가 있는지 검사하고 있으면 메세지를 보내고 종료
	else
	{
		for (auto& Manager : MsgHandlerManagerMap)
		{
			auto& HandlerMap = Manager.Value;
			if (HandlerMap.Contains(Message.ReceiverID))
			{
				HandlerMap[Message.ReceiverID]->HandleMessage(Message);
				return;
			}
		}
	}
	// 방법 2 : 모든 매니저의 모든 이벤트 핸들러를 돌면서 리시버가 있는지 검사하고 있으면 메세지를 보낸다.
	/*else
	{
		for (auto& Manager : MsgHandlerManagerMap)
		{
			for (auto& Handler : Manager.Value)
			{
				if (Message.Receiver->GetUniqueID() == Handler.Key)
				{
					Handler.Value->HandleMessage(Message);
				}
			}
		}
	}*/

}
