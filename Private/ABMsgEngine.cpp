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

	// ���ù��� ���� ��� ��ο��� �޼����� ������.
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
	// ���ù��� ���� ��� �ش� ���ù����׸� �޼����� ������.
	// ��� 1 : ��� �Ŵ����� ���鼭 ���ù��� �ִ��� �˻��ϰ� ������ �޼����� ������ ����
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
	// ��� 2 : ��� �Ŵ����� ��� �̺�Ʈ �ڵ鷯�� ���鼭 ���ù��� �ִ��� �˻��ϰ� ������ �޼����� ������.
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
