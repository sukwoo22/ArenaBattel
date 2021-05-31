// Fill out your copyright notice in the Description page of Project Settings.


#include "ABPlayerState.h"
#include "ABGameInstance.h"
#include "ABSaveGame.h"
#include "ABMsgEngine.h"

AABPlayerState::AABPlayerState()
{
	CharacterLevel = 1;
	GameScore = 0;
	GameHighScore = 0;
	Exp = 0;
	SaveSlotName = TEXT("Player1");
	CharacterIndex = 0;

	BindMsgHandlerDelegates();
	FABMsgEngine::AddMsgHandlerInManager(EManagerID::PLAYER_STATE_MANAGER, GetUniqueID(), this);
}

void AABPlayerState::BindMsgHandlerDelegates()
{
	MH_DEFI(GET_PLAYER_STATE)
	{
		MH_INIT(GET_PLAYER_STATE);
		Message.GameScore = GameScore;
		Message.GameHightScore = GameHighScore;
		Message.CharacterLevel = CharacterLevel;
		Message.CharacterIndex = CharacterIndex;
		Message.PlayerName = GetPlayerName();
		if (CurrentStatData->NextExp <= KINDA_SMALL_NUMBER)
		{
			Message.ExpRatio = 0.0f;
		}
		else
		{
			float Result = (float)Exp / (float)CurrentStatData->NextExp;
			Message.ExpRatio = Result;
		}
	}MH_DEFI_END;


	MH_DEFI(GET_GAME_SCORE)
	{
		MH_INIT(GET_GAME_SCORE);
		Message.GameHighScore = GameHighScore;
	}MH_DEFI_END;

	MH_DEFI(GET_CHARACTER_LEVEL)
	{
		MH_INIT(GET_CHARACTER_LEVEL);
		Message.CharacterLevel = CharacterLevel;
	}MH_DEFI_END;

	MH_DEFI(GET_CHARACTER_INDEX)
	{
		MH_INIT(GET_CHARACTER_INDEX);
		Message.CharacterIndex = CharacterIndex;
	}MH_DEFI_END;

	MH_DEFI(GET_EXP_RATIO)
	{
		MH_INIT(GET_EXP_RATIO);
		if (CurrentStatData->NextExp <= KINDA_SMALL_NUMBER)
		{
			Message.ExpRatio = 0.0f;
		}
		else
		{
			float Result = (float)Exp / (float)CurrentStatData->NextExp;
			Message.ExpRatio = Result;
		}
	}MH_DEFI_END;
	
	MH_DEFI(GET_SAVE_SLOT_NAME)
	{
		MH_INIT(GET_SAVE_SLOT_NAME);
		Message.SaveSlotName = &SaveSlotName;
	}MH_DEFI_END;

	MH_DEFI(ADD_EXP)
	{
		MH_INIT(ADD_EXP);
		if (CurrentStatData->NextExp == -1)
		{
			Message.IsSuccess = false;
			return;
		}

		bool DidLevelUp = false;
		Exp = Exp + Message.Exp;
		if (Exp >= CurrentStatData->NextExp)
		{
			Exp -= CurrentStatData->NextExp;
			SetCharacterLevel(CharacterLevel + 1);
			DidLevelUp = true;
		}

		ON_PLAYER_STATE_CHANGED OPSCMessage;
		HandleMessage(OPSCMessage);
		SAVE_PLAYER_DATA SPDMessage;
		HandleMessage(SPDMessage);
		Message.IsSuccess = DidLevelUp;
	}MH_DEFI_END;

	MH_DEFI(ADD_GAME_SCORE)
	{
		MH_INIT(ADD_GAME_SCORE);
		++GameScore;
		if (GameScore >= GameHighScore)
		{
			GameHighScore = GameScore;
		}
		ON_PLAYER_STATE_CHANGED OPSCMessage;
		HandleMessage(OPSCMessage);
		SAVE_PLAYER_DATA SPDMessage;
		HandleMessage(SPDMessage);
	}MH_DEFI_END;

	MH_DEFI(INIT_PLAYER_DATA)
	{
		MH_INIT(INIT_PLAYER_DATA);
		auto ABSaveGame = Cast<UABSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0));
		if (nullptr == ABSaveGame)
		{
			ABSaveGame = GetMutableDefault<UABSaveGame>();
		}

		SetPlayerName(ABSaveGame->PlayerName);
		SetCharacterLevel(ABSaveGame->Level);
		GameScore = 0;
		GameHighScore = ABSaveGame->HighScore;
		Exp = ABSaveGame->Exp;
		CharacterIndex = ABSaveGame->CharacterIndex;
		SAVE_PLAYER_DATA SPDMessage;
		HandleMessage(SPDMessage);
	}MH_DEFI_END;

	MH_DEFI(SAVE_PLAYER_DATA)
	{
		MH_INIT(SAVE_PLAYER_DATA);
		UABSaveGame* NewPlayerData = NewObject<UABSaveGame>();
		NewPlayerData->PlayerName = GetPlayerName();
		NewPlayerData->Level = CharacterLevel;
		NewPlayerData->Exp = Exp;
		NewPlayerData->HighScore = GameHighScore;
		NewPlayerData->CharacterIndex = CharacterIndex;

		if (!UGameplayStatics::SaveGameToSlot(NewPlayerData, SaveSlotName, 0))
		{
			ABLOG_MSG(Error, Message.ID, TEXT("SaveGame Error!"));
		}
	}MH_DEFI_END;
}

void AABPlayerState::SetCharacterLevel(int32 NewCharacterLevel)
{
	auto ABGameInstance = Cast<UABGameInstance>(GetGameInstance());
	ABCHECK(nullptr != ABGameInstance);

	CurrentStatData = ABGameInstance->GetABCharacterData(NewCharacterLevel);
	ABCHECK(nullptr != CurrentStatData);

	CharacterLevel = NewCharacterLevel;
}
