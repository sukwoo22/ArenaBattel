// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "ABMessage.generated.h"

enum class EControlMode
{
	GTA,
	DIABLO,
	AUTO_PLAY,
	ENEMY
};

UENUM()
enum class EMessageID
{
	MESSAGE_INVALID,
	//ABCharcterMessage
	SET_CHARACTER_STATE,
	GET_EXP,
	GET_FINAL_ATTACK_RANGE,
	GET_FINAL_ATTACK_DAMAGE,
	SET_CONTROL_MODE,
	SET_WEAPON,
	ATTACK,
	ATTACK_START_COMBO_STATE,
	ATTACK_END_COMBO_STATE,
	ATTACK_CHECK,
	ON_ASSET_LOAD_COMPLETED,
	ON_ATTACK_END,
	//ABControllerMessage
	GET_HUD_WIDGET,
	GAIN_EXP,
	ADD_GAME_SCORE,
	CHANGE_INPUT_MODE,
	SHOW_RESULT_UI,
	//ABPlayerState
	GET_PLAYER_STATE,
	GET_GAME_SCORE,
	GET_CHARACTER_LEVEL,
	GET_CHARACTER_INDEX,
	GET_EXP_RATIO,
	GET_SAVE_SLOT_NAME,
	ADD_EXP,
	INIT_PLAYER_DATA,
	SAVE_PLAYER_DATA,
	ON_PLAYER_STATE_CHANGED,

	//ABCharacterStatComponent
	SET_NEW_LEVEL,
	SET_DAMAGE,
	SET_HP,
	GET_ATTACK,
	GET_HP_RATIO,
	GET_DROP_EXP,
	ON_HP_IS_ZERO,
	ON_HP_CHANGED,
	
	//ABAnimInstance
	PLAY_ATTACK_MONTAGE,
	JUMP_TO_ATTACK_MONTAGE_SECTION,
	ON_NEXT_ATTACK_CHECK,
	ON_ATTACK_HIT_CHECK,
	SET_DEAD_ANIM,
};

#pragma region ABMsgLog
class ARENABATTLE_API FABMsgLogHelp
{
public:
	static FABMsgLogHelp& Get();
	FString GetEnumToString(EMessageID EnumValue);
	int32 FindFuncSignaureIndex(const FString& str);
	FString GetFuncName(const FString& str);
};

#define ABLOG_MSGINFO(MessageID) (FABMsgLogHelp::Get().GetFuncName(FString(__FUNCTION__)) \
+ FABMsgLogHelp::Get().GetEnumToString(MessageID) + TEXT(" ( Line : ") + FString::FromInt(__LINE__) + TEXT(" )"))

#define ABLOG_MSG(Verbosity, MessageID, Format,  ...) UE_LOG(ArenaBattle, Verbosity, TEXT("%s %s") \
	, *ABLOG_MSGINFO(MessageID), *FString::Printf(Format, ##__VA_ARGS__))  
#pragma endregion



USTRUCT()
struct FABMessage
{
	GENERATED_BODY()
	
	EMessageID ID;
	int32 ReceiverID;
};

//Message Handler Definition
#define MH_DEFI(MessageName) SinglecastMessageHandlerDelegate(EMessageID::##MessageName).BindLambda([this](FABMessage& InMessage)
#define MH_INIT(MessageName) auto& Message = static_cast<MessageName&>(InMessage) 
#define MH_DEFI_END )

// Message Declaration
#define MSG_DECL(MessageName) \
struct MessageName : public FABMessage { MessageName() { ID = EMessageID::##MessageName, ReceiverID = -1; }
#define MSG_DECL_END };

#pragma region ABCharacterMessage
MSG_DECL(SET_CHARACTER_STATE)
IN  ECharacterState CharacterState;
MSG_DECL_END

MSG_DECL(GET_EXP)
OUT int32 Exp;
MSG_DECL_END

MSG_DECL(GET_FINAL_ATTACK_RANGE)
OUT float FinalAttackRange;
MSG_DECL_END

MSG_DECL(GET_FINAL_ATTACK_DAMAGE)
OUT float FinalDamage;
MSG_DECL_END

MSG_DECL(SET_CONTROL_MODE)
IN EControlMode ControlMode;
MSG_DECL_END

MSG_DECL(SET_WEAPON)
IN class AABWeapon* Weapon;
MSG_DECL_END

MSG_DECL(ATTACK)
MSG_DECL_END

MSG_DECL(ATTACK_START_COMBO_STATE)
MSG_DECL_END

MSG_DECL(ATTACK_END_COMBO_STATE)
MSG_DECL_END

MSG_DECL(ATTACK_CHECK)
MSG_DECL_END

MSG_DECL(ON_ASSET_LOAD_COMPLETED)
MSG_DECL_END

MSG_DECL(ON_ATTACK_END)
MSG_DECL_END
#pragma endregion

#pragma region ABPlayerControllerMessage
MSG_DECL(GET_HUD_WIDGET)
OUT class UABHUDWidget* HUDWidget;
MSG_DECL_END

MSG_DECL(GAIN_EXP)
IN int32 KilledEnemyID;
MSG_DECL_END

MSG_DECL(ADD_GAME_SCORE)
MSG_DECL_END

MSG_DECL(CHANGE_INPUT_MODE)
IN bool bGameMode;
MSG_DECL_END

MSG_DECL(SHOW_RESULT_UI)
MSG_DECL_END
#pragma endregion

#pragma region ABPlayerStateMessage
MSG_DECL(GET_PLAYER_STATE)
OUT int32 GameScore;
OUT int32 GameHightScore;
OUT int32 CharacterLevel;
OUT int32 CharacterIndex;
OUT float ExpRatio;
OUT FString PlayerName;
MSG_DECL_END

MSG_DECL(GET_GAME_SCORE)
OUT int32 GameHighScore;
MSG_DECL_END

MSG_DECL(GET_CHARACTER_LEVEL)
OUT int32 CharacterLevel;
MSG_DECL_END

MSG_DECL(GET_CHARACTER_INDEX)
OUT int32 CharacterIndex;
MSG_DECL_END

MSG_DECL(GET_EXP_RATIO)
OUT float ExpRatio;
MSG_DECL_END

MSG_DECL(GET_SAVE_SLOT_NAME)
OUT FString* SaveSlotName;
MSG_DECL_END

MSG_DECL(ADD_EXP)
IN int32 Exp;
OUT bool IsSuccess;
MSG_DECL_END

MSG_DECL(INIT_PLAYER_DATA)
MSG_DECL_END

MSG_DECL(SAVE_PLAYER_DATA)
MSG_DECL_END

MSG_DECL(ON_PLAYER_STATE_CHANGED)
MSG_DECL_END
#pragma endregion

#pragma region ABCharacterStatComponenetMessage
MSG_DECL(SET_NEW_LEVEL)
IN	int32 NewLevel;
MSG_DECL_END

MSG_DECL(SET_DAMAGE)
IN float NewDamage;
MSG_DECL_END

MSG_DECL(SET_HP)
IN float NewHP;
MSG_DECL_END

MSG_DECL(GET_ATTACK)
OUT float Attack;
MSG_DECL_END

MSG_DECL(GET_HP_RATIO)
OUT float HPRatio;
MSG_DECL_END

MSG_DECL(GET_DROP_EXP)
OUT int32 DropExp;
MSG_DECL_END

MSG_DECL(ON_HP_IS_ZERO)
MSG_DECL_END

MSG_DECL(ON_HP_CHANGED)
MSG_DECL_END
#pragma endregion

#pragma region ABAnimInstanceMsg
MSG_DECL(PLAY_ATTACK_MONTAGE)
MSG_DECL_END

MSG_DECL(JUMP_TO_ATTACK_MONTAGE_SECTION)
IN int32 NewSection;
MSG_DECL_END

MSG_DECL(ON_NEXT_ATTACK_CHECK)
MSG_DECL_END

MSG_DECL(ON_ATTACK_HIT_CHECK)
MSG_DECL_END

MSG_DECL(SET_DEAD_ANIM)
MSG_DECL_END
#pragma endregion
