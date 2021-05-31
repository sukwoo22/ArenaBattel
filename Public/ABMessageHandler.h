// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "UObject/Interface.h"
#include "ABMessage.h"
//#include "ABMessageHandler.generated.h"

DECLARE_DELEGATE_OneParam(FABSinglecastMessageHandlerDelegate, FABMessage&);
DECLARE_MULTICAST_DELEGATE_OneParam(FABMulticastMsgHandlerDelegate, FABMessage&);

//UINTERFACE(MinimalAPI)
//class UABMessageHandler : public UInterface
//{
//	GENERATED_BODY()
//};

class ARENABATTLE_API IABMessageHandler
{
public:
	virtual ~IABMessageHandler() = default;
	virtual void HandleMessage(FABMessage& Message) = 0;
	virtual void BindMsgHandlerDelegates() = 0;
};

class ARENABATTLE_API FABSiglecastMessageHandler : public IABMessageHandler
{
public:
	virtual ~FABSiglecastMessageHandler() = default;
	virtual void HandleMessage(FABMessage& Message);
	FABSinglecastMessageHandlerDelegate& SinglecastMessageHandlerDelegate(EMessageID MessageID);

protected:
	TMap<EMessageID, FABSinglecastMessageHandlerDelegate>& GetSinglecastMessageHandlerDelegateMap();

private:
	TMap<EMessageID, FABSinglecastMessageHandlerDelegate> SinglecastMessageHandlerDelegateMap;
};

class ARENABATTLE_API FABMulticastMessageHandler : public FABSiglecastMessageHandler
{
	
public:
	virtual ~FABMulticastMessageHandler() = default;
	virtual void HandleMessage(FABMessage& Message);
	FABMulticastMsgHandlerDelegate& MulticastMessageHandlerDelegate(EMessageID MessageID);

private:
	TMap<EMessageID, FABMulticastMsgHandlerDelegate> MulticastMessageHandlerDelegateMap;
};