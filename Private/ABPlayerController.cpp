// Fill out your copyright notice in the Description page of Project Settings.


#include "ABPlayerController.h"
#include "ABHUDWidget.h"
#include "ABPlayerState.h"
#include "ABCharacter.h"
#include "ABGameplayWidget.h"
#include "ABGameplayResultWidget.h"
#include "ABGameState.h"
#include "ABMsgEngine.h"

AABPlayerController::AABPlayerController()
{
	static ConstructorHelpers::FClassFinder<UABHUDWidget> UI_HUD_C(TEXT("/Game/Book/UI/UI_HUD.UI_HUD_C"));
	if (UI_HUD_C.Succeeded())
	{
		HUDWidgetClass = UI_HUD_C.Class;
	}

	static ConstructorHelpers::FClassFinder<UABGameplayWidget> UI_MENU_C(TEXT("/Game/Book/UI/UI_Menu.UI_Menu_C"));
	if (UI_MENU_C.Succeeded())
	{
		MenuWidgetClass = UI_MENU_C.Class;
	}

	static ConstructorHelpers::FClassFinder<UABGameplayResultWidget> UI_RESULT_C(TEXT("/Game/Book/UI/UI_Result.UI_Result_C"));
	if (UI_RESULT_C.Succeeded())
	{
		ResultWidgetClass = UI_RESULT_C.Class;
	}

	BindMsgHandlerDelegates();
	FABMsgEngine::AddMsgHandlerInManager(EManagerID::CONTROLLER_MANAGER, GetUniqueID(), this);
}

void AABPlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	ABLOG_S(Warning);
}

void AABPlayerController::OnPossess(APawn* aPawn)
{
	ABLOG_S(Warning);
	Super::OnPossess(aPawn);
}

void AABPlayerController::BindMsgHandlerDelegates()
{
	MH_DEFI(GET_HUD_WIDGET)
	{
		MH_INIT(GET_HUD_WIDGET);
		Message.HUDWidget = HUDWidget;
	}MH_DEFI_END;

	MH_DEFI(GAIN_EXP)
	{
		MH_INIT(GAIN_EXP);
		GET_EXP GEMessage;
		GEMessage.ReceiverID = Message.KilledEnemyID;
		FABMsgEngine::SendMessage(GEMessage);

		ADD_EXP AEMessage;
		AEMessage.Exp = GEMessage.Exp;
		AEMessage.ReceiverID = ABPlayerState->GetUniqueID();
		FABMsgEngine::SendMessage(AEMessage);
	}MH_DEFI_END;

	MH_DEFI(ADD_GAME_SCORE)
	{
		MH_INIT(ADD_GAME_SCORE);
		ADD_GAME_SCORE AGSMessage;
		AGSMessage.ReceiverID = ABPlayerState->GetUniqueID();
		FABMsgEngine::SendMessage(AGSMessage);
	}MH_DEFI_END;

	MH_DEFI(CHANGE_INPUT_MODE)
	{
		MH_INIT(CHANGE_INPUT_MODE);
		if (Message.bGameMode)
		{
			SetInputMode(GameInputMode);
			bShowMouseCursor = false;
		}
		else
		{
			SetInputMode(UIInputMode);
			bShowMouseCursor = true;
		}
	}MH_DEFI_END;

	MH_DEFI(SHOW_RESULT_UI)
	{
		MH_INIT(SHOW_RESULT_UI);
		auto ABGameState = Cast<AABGameState>(UGameplayStatics::GetGameState(this));
		ABCHECK(nullptr != ABGameState);
		ResultWidget->BindGameState(ABGameState);

		ResultWidget->AddToViewport();
		CHANGE_INPUT_MODE CIMMessage;
		CIMMessage.bGameMode = false;
		HandleMessage(CIMMessage);
	}MH_DEFI_END;
}

void AABPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	InputComponent->BindAction(TEXT("GamePause"), EInputEvent::IE_Pressed, this, &AABPlayerController::OnGamePause);
}

void AABPlayerController::BeginPlay()
{
	Super::BeginPlay();
	CHANGE_INPUT_MODE CIMMessage;
	CIMMessage.bGameMode = true;
	HandleMessage(CIMMessage);

	HUDWidget = CreateWidget<UABHUDWidget>(this, HUDWidgetClass);
	ABCHECK(nullptr != HUDWidget);
	HUDWidget->AddToViewport(1);

	ResultWidget = CreateWidget<UABGameplayResultWidget>(this, ResultWidgetClass);
	ABCHECK(nullptr != ResultWidget);

	ABPlayerState = Cast<AABPlayerState>(PlayerState);
	ABCHECK(nullptr != ABPlayerState);
	HUDWidget->BindPlayerState(ABPlayerState);

	ON_PLAYER_STATE_CHANGED OPSCMessage;
	OPSCMessage.ReceiverID = ABPlayerState->GetUniqueID();
	FABMsgEngine::SendMessage(OPSCMessage);
}

void AABPlayerController::OnGamePause()
{
	MenuWidget = CreateWidget<UABGameplayWidget>(this, MenuWidgetClass);
	ABCHECK(nullptr != MenuWidget);
	MenuWidget->AddToViewport(3);

	SetPause(true);
	CHANGE_INPUT_MODE CIMMessage;
	CIMMessage.bGameMode = false;
	HandleMessage(CIMMessage);
}
