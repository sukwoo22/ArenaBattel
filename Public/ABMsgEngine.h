// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ABMessageHandler.h"

enum class EManagerID
{
	INVALID_MANAGER,
	CHARACTER_MANAGER,
	CONTROLLER_MANAGER,
	WIDGET_MANAGER,
	PLAYER_STATE_MANAGER,
	CHARACTER_COMPONENT_MANAGER,
};

/**
 * 
 */
class ABMsgEngine
{
public:

	static void AddMsgHandlerInManager(EManagerID ManagerID, int32 MessageHandlerID, IABMessageHandler* MessageHandler);
	static void DeleteMsgHandlerInManager(EManagerID ManagerID, int32 MessageHandlerID);
	static void SendMessage(FABMessage& Message);


private:

	static ABMsgEngine* Get()
	{
		static ABMsgEngine* MsgEngine = new ABMsgEngine();
		return MsgEngine;
	}
	static TMap<EManagerID, TMap<uint32, IABMessageHandler*>>& GetMessageHandlerManagerMap()
	{
		return Get()->MessageHandlerManagerMap;
	}

	ABMsgEngine() = default;
	ABMsgEngine(const ABMsgEngine&) = delete;
	ABMsgEngine& operator=(const ABMsgEngine&) = delete;
	ABMsgEngine(ABMsgEngine&&) = delete;
	ABMsgEngine& operator=(const ABMsgEngine&&) = delete;

private:

	TMap<EManagerID, TMap<uint32, IABMessageHandler*>> MessageHandlerManagerMap;
};

