// Fill out your copyright notice in the Description page of Project Settings.


#include "ABCharacterStatComponent.h"
#include "ABGameInstance.h"
#include "ABMsgEngine.h"

// Sets default values for this component's properties
UABCharacterStatComponent::UABCharacterStatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;

	Level = 1;
	BindMsgHandlerDelegates();
	FABMsgEngine::AddMsgHandlerInManager(EManagerID::CHARACTER_COMPONENT_MANAGER, GetUniqueID(), this);
}


void UABCharacterStatComponent::BindMsgHandlerDelegates()
{
	MH_DEFI(SET_NEW_LEVEL)
	{
		MH_INIT(SET_NEW_LEVEL);
		auto ABGameInstance = Cast<UABGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

		ABCHECK(ABGameInstance != nullptr);
		CurrentStatData = ABGameInstance->GetABCharacterData(Message.NewLevel);
		if (CurrentStatData != nullptr)
		{
			Level = Message.NewLevel;
			SET_HP SHMessage;
			SHMessage.NewHP = CurrentStatData->MaxHP;
			HandleMessage(SHMessage);
		}
		else
		{
			ABLOG(Error, TEXT("Level (%d) data doesn't exist"), Message.NewLevel);
		}
	}MH_DEFI_END;
	
	MH_DEFI(SET_DAMAGE)
	{
		MH_INIT(SET_DAMAGE);
		ABCHECK(nullptr != CurrentStatData);
		SET_HP SHMessage;
		SHMessage.NewHP = FMath::Clamp<float>(CurrentHP - Message.NewDamage, 0.0f, CurrentStatData->MaxHP);
		HandleMessage(SHMessage);
		
		//ABLOG(Warning, TEXT(" HPRatio : %f, CurrentHP : %f"), CurrentHP / CurrentStatData->MaxHP, CurrentHP);
		ABLOG_MSG(Warning, Message.ID,TEXT(" HPRatio : %f, CurrentHP : %f"), CurrentHP / CurrentStatData->MaxHP, CurrentHP);
	}MH_DEFI_END;


	MH_DEFI(SET_HP)
	{
		MH_INIT(SET_HP);
		
		CurrentHP = Message.NewHP;
		ON_HP_CHANGED OHCMessage;
		HandleMessage(OHCMessage);

		if (CurrentHP < KINDA_SMALL_NUMBER)
		{
			CurrentHP = 0.0f;
			ON_HP_IS_ZERO OHIZMessage;
			HandleMessage(OHIZMessage);
		}
	}MH_DEFI_END;
	
	MH_DEFI(GET_ATTACK)
	{
		MH_INIT(GET_ATTACK);
		ABCHECK(nullptr != CurrentStatData);
		Message.Attack = CurrentStatData->Attack;
	}MH_DEFI_END;

	MH_DEFI(GET_HP_RATIO)
	{
		MH_INIT(GET_HP_RATIO);
		ABCHECK(nullptr != CurrentStatData);
		Message.HPRatio = CurrentStatData->MaxHP < KINDA_SMALL_NUMBER ? 0.0f : CurrentHP / CurrentStatData->MaxHP;
	}MH_DEFI_END;

	MH_DEFI(GET_DROP_EXP)
	{
		MH_INIT(GET_DROP_EXP);
		Message.DropExp = CurrentStatData->DropExp;
	}MH_DEFI_END;
	
}

// Called when the game starts
void UABCharacterStatComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UABCharacterStatComponent::InitializeComponent()
{
	Super::InitializeComponent();
	SET_NEW_LEVEL Message;
	Message.NewLevel = Level;
	HandleMessage(Message);
}

