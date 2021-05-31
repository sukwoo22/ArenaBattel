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
class ARENABATTLE_API FABMsgEngine
{
public:

	static void AddMsgHandlerInManager(EManagerID ManagerID, int32 MessageHandlerID, IABMessageHandler* MessageHandler);
	static void DeleteMsgHandlerInManager(EManagerID ManagerID, int32 MessageHandlerID);
	static void SendMessage(FABMessage& Message);


private:

	static FABMsgEngine* Get()
	{
		static FABMsgEngine* MsgEngine = new FABMsgEngine();
		return MsgEngine;
	}
	static TMap<EManagerID, TMap<uint32, IABMessageHandler*>>& GetMessageHandlerManagerMap()
	{
		return Get()->MessageHandlerManagerMap;
	}

	FABMsgEngine() = default;
	FABMsgEngine(const FABMsgEngine&) = delete;
	FABMsgEngine& operator=(const FABMsgEngine&) = delete;
	FABMsgEngine(FABMsgEngine&&) = delete;
	FABMsgEngine& operator=(const FABMsgEngine&&) = delete;

private:

	TMap<EManagerID, TMap<uint32, IABMessageHandler*>> MessageHandlerManagerMap;
};

