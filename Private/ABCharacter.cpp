// Fill out your copyright notice in the Description page of Project Settings.


#include "ABCharacter.h"
#include "ABAnimInstance.h"
#include "ABWeapon.h"
#include "ABCharacterStatComponent.h"
#include "DrawDebugHelpers.h"
#include "Components/WidgetComponent.h"
#include "ABCharacterWidget.h"
#include "ABAIController.h"
#include "ABCharacterSetting.h"
#include "ABGameInstance.h"
#include "ABPlayerController.h"
#include "ABPlayerState.h"
#include "ABHUDWidget.h"
#include "ABGameMode.h"
#include "ABMsgEngine.h"

// Sets default values
AABCharacter::AABCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SPRINGARM"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CAMERA"));
	CharacterStat = CreateDefaultSubobject<UABCharacterStatComponent>(TEXT("CHARACTERSTAT"));
	HPBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HPBARWIDGET"));

	SpringArm->SetupAttachment(GetCapsuleComponent());
	Camera->SetupAttachment(SpringArm);
	HPBarWidget->SetupAttachment(GetMesh());

	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -88.0f),
		FRotator(0.0f, -90.0f, 0.0f));
	SpringArm->TargetArmLength = 400.0f;
	SpringArm->SetRelativeRotation(FRotator(-15.0f, 0.0f, 0.0f));

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_CARDBOARD(TEXT("/Game/InfinityBladeWarriors/Character/CompleteCharacters/SK_CharM_Cardboard.SK_CharM_Cardboard"));

	if (SK_CARDBOARD.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SK_CARDBOARD.Object);
	}

	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);

	static ConstructorHelpers::FClassFinder<UAnimInstance> WARRIOR_ANIM(TEXT("/Game/Book/Animations/WarriorAnimBlueprint.WarriorAnimBlueprint_C"));
	if (WARRIOR_ANIM.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(WARRIOR_ANIM.Class);
	}

	CurrentControlMode = EControlMode::GTA;

	DirectionToMove = FVector::ZeroVector;
	ArmLengthTo = 0.0f;
	ArmRotationTo = FRotator::ZeroRotator;
	ArmLengthSpeed = 3.0f;
	ArmRotationSpeed = 10.0f;

	GetCharacterMovement()->JumpZVelocity = 800.0f;
	IsAttacking = false;
	MaxCombo = 4;
	ATTACK_END_COMBO_STATE AECSMessage;
	HandleMessage(AECSMessage);

	GetCapsuleComponent()->SetCollisionProfileName(TEXT("ABCharacter"));
	AttackRange = 80.0f;
	AttackRadius = 50.0f;

	HPBarWidget->SetRelativeLocation(FVector(0.0f, 0.0f, 180.0f));
	HPBarWidget->SetWidgetSpace(EWidgetSpace::Screen);
	static ConstructorHelpers::FClassFinder<UUserWidget> UI_HUD(TEXT("/Game/Book/UI/UI_HPBar.UI_HPBar_C"));
	if (UI_HUD.Succeeded())
	{
		HPBarWidget->SetWidgetClass(UI_HUD.Class);
		HPBarWidget->SetDrawSize(FVector2D(150.0f, 50.0f));
	}
	AIControllerClass = AABAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	CharacterAssetToLoad = FSoftObjectPath(nullptr);

	AssetIndex = 4;

	SetActorHiddenInGame(true);
	HPBarWidget->SetHiddenInGame(true);
	SetCanBeDamaged(false);

	DeadTimer = 5.0f;
	DeadTimerHandle = {};

	CurrentWeapon = nullptr;

	BindMsgHandlerDelegates();
	FABMsgEngine::AddMsgHandlerInManager(EManagerID::CHARACTER_MANAGER, GetUniqueID(), this);
}

void AABCharacter::BindMsgHandlerDelegates()
{

	//매크로를 이용한 커맨드 정의는 아래와 같은 의미이다.
	/*MessageHandler[EMessageID::GET_EXP].BindLambda([this](FABMessage& InMessage) {
		auto& Message = static_cast<GET_EXP&>(InMessage);
		Message.Exp = CharacterStat->GetDropExp();
	});*/

	MH_DEFI(SET_CHARACTER_STATE)
	{
		MH_INIT(SET_CHARACTER_STATE);
		CurrentState = Message.CharacterState;
		switch (CurrentState)
		{
			case ECharacterState::LOADING:
			{
				if (bIsPlayer)
				{
					DisableInput(ABPlayerController);
					
					GET_HUD_WIDGET GHWMessage;
					GHWMessage.ReceiverID = ABPlayerController->GetUniqueID();
					FABMsgEngine::SendMessage(GHWMessage);
					GHWMessage.HUDWidget->BindCharacterStat(CharacterStat);

					auto ABPlayerState = Cast<AABPlayerState>(GetPlayerState());
					ABCHECK(nullptr != ABPlayerState);
					
					GET_CHARACTER_LEVEL GCLMessage;
					GCLMessage.ReceiverID = ABPlayerState->GetUniqueID();
					FABMsgEngine::SendMessage(GCLMessage);
					
					SET_NEW_LEVEL SNLMessage;
					SNLMessage.ReceiverID = CharacterStat->GetUniqueID();
					SNLMessage.NewLevel = GCLMessage.CharacterLevel;
					FABMsgEngine::SendMessage(SNLMessage);
				}
				else
				{
					auto ABGameMode = Cast<AABGameMode>(GetWorld()->GetAuthGameMode());
					ABCHECK(nullptr != ABGameMode);
					int32 TargetLevel = FMath::CeilToInt(((float)ABGameMode->GetScore() * 0.8f));
					int32 FinalLevel = FMath::Clamp<int32>(TargetLevel, 1, 20);
					ABLOG_MSG(Warning, Message.ID, TEXT("New NPC Level : %d "), FinalLevel);
					
					SET_NEW_LEVEL SNLMessage;
					SNLMessage.ReceiverID = CharacterStat->GetUniqueID();
					SNLMessage.NewLevel = FinalLevel;
					FABMsgEngine::SendMessage(SNLMessage);
				}
				SetActorHiddenInGame(true);
				HPBarWidget->SetHiddenInGame(true);
				SetCanBeDamaged(false);
				break;
			}
			case ECharacterState::READY:
			{
				SetActorHiddenInGame(false);
				HPBarWidget->SetHiddenInGame(false);
				SetCanBeDamaged(true);
				
				CharacterStat->SinglecastMessageHandlerDelegate(EMessageID::ON_HP_IS_ZERO).BindLambda([this](FABMessage& InMessage)->void {
					SET_CHARACTER_STATE SCSMessage;
					SCSMessage.CharacterState = ECharacterState::DEAD;
					HandleMessage(SCSMessage);
				});

				auto CharacterWidget = Cast<UABCharacterWidget>(HPBarWidget->GetUserWidgetObject());
				ABCHECK(nullptr != CharacterWidget);
				CharacterWidget->BindCharacterStat(CharacterStat);

				if (bIsPlayer)
				{
					SET_CONTROL_MODE SCMMessage;
					SCMMessage.ControlMode = EControlMode::DIABLO;
					HandleMessage(SCMMessage);
					GetCharacterMovement()->MaxWalkSpeed = 600.0f;
					EnableInput(ABPlayerController);
				}
				else
				{
					SET_CONTROL_MODE SCMMessage;
					SCMMessage.ControlMode = EControlMode::ENEMY;
					HandleMessage(SCMMessage);
					GetCharacterMovement()->MaxWalkSpeed = 400.0f;
					ABAIController->RunAI();
				}
				break;
			}
			case ECharacterState::DEAD:
			{
				SetActorEnableCollision(false);
				GetMesh()->SetHiddenInGame(false);
				HPBarWidget->SetHiddenInGame(true);
				ABAnim->SetDeadAnim();
				SetCanBeDamaged(false);

				if (bIsPlayer)
				{
					DisableInput(ABPlayerController);
				}
				else
				{
					ABAIController->StopAI();
				}

				GetWorld()->GetTimerManager().SetTimer(DeadTimerHandle, FTimerDelegate::CreateLambda([this]()->void {
					if (bIsPlayer)
					{
						SHOW_RESULT_UI SRUMessage;
						SRUMessage.ReceiverID = ABPlayerController->GetUniqueID();
						FABMsgEngine::SendMessage(SRUMessage);
					}
					else
					{
						Destroy();
					}
				}), DeadTimer, false);

				break;
			}
			default:
				ABLOG_MSG(Warning, Message.ID, TEXT("Invalid Mode"));
				break;
		}
	}MH_DEFI_END;

	MH_DEFI(GET_EXP)
	{
		MH_INIT(GET_EXP);
		GET_DROP_EXP GDEMessage;
		GDEMessage.ReceiverID = CharacterStat->GetUniqueID();
		FABMsgEngine::SendMessage(GDEMessage);
		Message.Exp = GDEMessage.DropExp;
		//Message.Exp = CharacterStat->GetDropExp();
	}MH_DEFI_END;

	MH_DEFI(GET_FINAL_ATTACK_RANGE)
	{
		MH_INIT(GET_FINAL_ATTACK_RANGE);
		Message.FinalAttackRange = (nullptr != CurrentWeapon) ? CurrentWeapon->GetAttackRange() : AttackRange;
	}MH_DEFI_END;

	MH_DEFI(GET_FINAL_ATTACK_DAMAGE)
	{
		MH_INIT(GET_FINAL_ATTACK_DAMAGE);
		GET_ATTACK GAMessage;
		GAMessage.ReceiverID = CharacterStat->GetUniqueID();
		FABMsgEngine::SendMessage(GAMessage);
		float AttackDamage = nullptr != CurrentWeapon ? GAMessage.Attack + CurrentWeapon->GetAttackDamage() : GAMessage.Attack;
		float AttackModifier = nullptr != CurrentWeapon ? CurrentWeapon->GetAttackModifier() : 1.0f;
		Message.FinalDamage = AttackDamage * AttackModifier;
	}MH_DEFI_END;

	MH_DEFI(SET_CONTROL_MODE)
	{
		MH_INIT(SET_CONTROL_MODE);
		CurrentControlMode = Message.ControlMode;
		switch (Message.ControlMode)
		{
			case EControlMode::GTA:
				ArmLengthTo = 450.0f;
				ArmRotationTo = FRotator::ZeroRotator;
				SpringArm->bUsePawnControlRotation = true;
				SpringArm->bInheritPitch = true;
				SpringArm->bInheritRoll = true;
				SpringArm->bInheritYaw = true;
				SpringArm->bDoCollisionTest = true;
				bUseControllerRotationYaw = false;
				GetCharacterMovement()->bOrientRotationToMovement = true;
				GetCharacterMovement()->bUseControllerDesiredRotation = false;
				GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
				break;

			case EControlMode::DIABLO:
				ArmLengthTo = 800.0f;
				ArmRotationTo = FRotator(-45.0f, 0.0f, 0.0f);
				SpringArm->bUsePawnControlRotation = false;
				SpringArm->bInheritPitch = false;
				SpringArm->bInheritRoll = false;
				SpringArm->bInheritYaw = false;
				SpringArm->bDoCollisionTest = false;
				bUseControllerRotationYaw = false;
				GetCharacterMovement()->bOrientRotationToMovement = false;
				GetCharacterMovement()->bUseControllerDesiredRotation = true;
				GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
				break;

			case EControlMode::ENEMY:
				bUseControllerRotationYaw = false;
				GetCharacterMovement()->bUseControllerDesiredRotation = false;
				GetCharacterMovement()->bOrientRotationToMovement = true;
				GetCharacterMovement()->RotationRate = FRotator(0.0f, 480.0f, 0.0f);
				break;

			default:
				ABLOG_MSG(Warning, Message.ID, TEXT("Invalid control mode"));
				break;
		}
	}MH_DEFI_END;

	MH_DEFI(SET_WEAPON)
	{
		MH_INIT(SET_WEAPON);
		ABCHECK(nullptr != Message.Weapon);
		if (nullptr != CurrentWeapon)
		{
			CurrentWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			CurrentWeapon->Destroy();
			CurrentWeapon = nullptr;
		}

		FName WeaponSocket(TEXT("hand_rSocket"));
		if (nullptr != Message.Weapon)
		{
			Message.Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocket);
			Message.Weapon->SetOwner(this);
			CurrentWeapon = Message.Weapon;
		}
	}MH_DEFI_END;

	MH_DEFI(ATTACK)
	{
		MH_INIT(ATTACK);
		if (IsAttacking)
		{
			ABCHECK(FMath::IsWithinInclusive<int32>(CurrentCombo, 1, MaxCombo));
			if (CanNextCombo)
			{
				IsComboInputOn = true;
			}
		}
		else
		{
			ABCHECK(CurrentCombo == 0);
			ATTACK_START_COMBO_STATE ASCMessage;
			HandleMessage(ASCMessage);
			ABAnim->PlayAttackMonTage();
			ABAnim->JumpToAttackMontageSection(CurrentCombo);
			IsAttacking = true;
		}
	}MH_DEFI_END;

	MH_DEFI(ATTACK_START_COMBO_STATE)
	{
		MH_INIT(ATTACK_START_COMBO_STATE);
		CanNextCombo = true;
		IsComboInputOn = false;
		ABCHECK(FMath::IsWithinInclusive<int32>(CurrentCombo, 0, MaxCombo - 1));
		CurrentCombo = FMath::Clamp<int32>(CurrentCombo + 1, 1, MaxCombo);
	}MH_DEFI_END;

	MH_DEFI(ATTACK_END_COMBO_STATE)
	{
		MH_INIT(ATTACK_END_COMBO_STATE);
		IsComboInputOn = false;
		CanNextCombo = false;
		CurrentCombo = 0;
	}MH_DEFI_END;

	MH_DEFI(ATTACK_CHECK)
	{
		MH_INIT(ATTACK_CHECK);
		GET_FINAL_ATTACK_RANGE GFARMessage;
		HandleMessage(GFARMessage);
		float FinalAttackRange = GFARMessage.FinalAttackRange;

		FHitResult HitResult;
		FCollisionQueryParams Params(NAME_None, false, this);
		bool bResult = GetWorld()->SweepSingleByChannel(
			HitResult,
			GetActorLocation(),
			GetActorLocation() + GetActorForwardVector() * FinalAttackRange,
			FQuat::Identity,
			ECollisionChannel::ECC_GameTraceChannel2,
			FCollisionShape::MakeSphere(AttackRadius),
			Params);

#if ENABLE_DRAW_DEBUG

		FVector TraceVec = GetActorForwardVector() * FinalAttackRange;
		FVector Center = GetActorLocation() + TraceVec * 0.5f;
		float HalfHeight = FinalAttackRange * 0.5f + AttackRadius;
		FQuat CapsuleRot = FRotationMatrix::MakeFromZ(TraceVec).ToQuat();
		FColor DrawColor = bResult ? FColor::Green : FColor::Red;
		float DebugLifeTime = 5.0f;

		DrawDebugCapsule(GetWorld(),
			Center,
			HalfHeight,
			AttackRadius,
			CapsuleRot,
			DrawColor,
			false,
			DebugLifeTime);

#endif

		if (bResult)
		{
			if (HitResult.Actor.IsValid())
			{
				FDamageEvent DamageEvent;
				GET_FINAL_ATTACK_DAMAGE GFADMessage;
				HandleMessage(GFADMessage);
				HitResult.Actor->TakeDamage(GFADMessage.FinalDamage, DamageEvent, GetController(), this);
			}
		}

	}MH_DEFI_END;
}

// Called when the game starts or when spawned
void AABCharacter::BeginPlay()
{
	Super::BeginPlay();

	bIsPlayer = IsPlayerControlled();
	if (bIsPlayer)
	{
		ABPlayerController = Cast<AABPlayerController>(GetController());
		ABCHECK(nullptr != ABPlayerController);
	}
	else
	{
		ABAIController = Cast<AABAIController>(GetController());
		ABCHECK(nullptr != ABAIController);
	}

	auto DefaultSetting = GetDefault<UABCharacterSetting>();
	if (bIsPlayer)
	{
		auto ABPlayerState = Cast<AABPlayerState>(GetPlayerState());
		ABCHECK(nullptr != ABPlayerState);
		GET_CHARACTER_INDEX GCIMessage;
		GCIMessage.ReceiverID = ABPlayerState->GetUniqueID();
		FABMsgEngine::SendMessage(GCIMessage);
		AssetIndex = GCIMessage.CharacterIndex;
	}
	else
	{
		AssetIndex = FMath::RandRange(0, DefaultSetting->CharacterAssets.Num() - 1);
	}

	CharacterAssetToLoad = DefaultSetting->CharacterAssets[AssetIndex];
	auto ABGameIntance = Cast<UABGameInstance>(GetGameInstance());
	ABCHECK(nullptr != ABGameIntance);
	AssetStreamingHandle = ABGameIntance->StreamableManager.RequestAsyncLoad(CharacterAssetToLoad, FStreamableDelegate::CreateUObject(this, &AABCharacter::OnAssetLoadCompleted));
	SET_CHARACTER_STATE Message;
	Message.CharacterState = ECharacterState::LOADING;
	HandleMessage(Message);
}


// Called every frame
void AABCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SpringArm->TargetArmLength = FMath::FInterpTo(SpringArm->TargetArmLength, ArmLengthTo, DeltaTime, ArmLengthSpeed);

	switch (CurrentControlMode)
	{
	case EControlMode::DIABLO:
		SpringArm->SetRelativeRotation(FMath::RInterpTo(SpringArm->GetRelativeRotation(), ArmRotationTo, DeltaTime, ArmRotationSpeed));
		if (DirectionToMove.SizeSquared() > 0.0f)
		{
			GetController()->SetControlRotation(FRotationMatrix::MakeFromX(DirectionToMove).Rotator());
			AddMovementInput(DirectionToMove);
		}
		break;
	}

}

// Called to bind functionality to input
void AABCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	PlayerInputComponent->BindAction(TEXT("ViewChange"), EInputEvent::IE_Pressed, this, &AABCharacter::ViewChange);
	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this, &AABCharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("Attack"), EInputEvent::IE_Pressed, this, &AABCharacter::Attack);
	PlayerInputComponent->BindAxis(TEXT("UpDown"), this, &AABCharacter::UpDown);
	PlayerInputComponent->BindAxis(TEXT("LeftRight"), this, &AABCharacter::LeftRight);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &AABCharacter::LookUp);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &AABCharacter::Turn);
}

void AABCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	ABAnim = Cast<UABAnimInstance>(GetMesh()->GetAnimInstance());
	ABCHECK(ABAnim != nullptr);

	ABAnim->OnMontageEnded.AddDynamic(this, &AABCharacter::OnAttackMontageEnded);
	ABAnim->OnNextAttackCheck.AddLambda([this]()->void {
		CanNextCombo = false;

		if (IsComboInputOn)
		{
			ATTACK_START_COMBO_STATE ASCSMessage;
			HandleMessage(ASCSMessage);
			ABAnim->JumpToAttackMontageSection(CurrentCombo);
		}
	});
	
	ABAnim->OnAttackHitCheck.AddUObject(this, &AABCharacter::AttackCheck);
}

float AABCharacter::TakeDamage(float DamageAmount, FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser)
{
	float FinalDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	
	SET_DAMAGE SDMessage;
	SDMessage.ReceiverID = CharacterStat->GetUniqueID();
	SDMessage.NewDamage = FinalDamage;
	FABMsgEngine::SendMessage(SDMessage);

	if (CurrentState == ECharacterState::DEAD)
	{
		if (EventInstigator->IsPlayerController())
		{
			auto PlayerController = Cast<AABPlayerController>(EventInstigator);
			ABCHECK(nullptr != PlayerController, 0.0f);
			GAIN_EXP Message;
			Message.ReceiverID = PlayerController->GetUniqueID();
			Message.KilledEnemyID = this->GetUniqueID();
			FABMsgEngine::SendMessage(Message);
		}
	}
	return FinalDamage;
}

void AABCharacter::UpDown(float NewAxisValue)
{
	switch (CurrentControlMode)
	{
	case EControlMode::GTA:
		AddMovementInput(FRotationMatrix(FRotator(0.0f, GetControlRotation().Yaw, 0.0f)).GetUnitAxis(EAxis::X), NewAxisValue);
		break;

	case EControlMode::DIABLO:
		DirectionToMove.X = NewAxisValue;
		break;
	}
}

void AABCharacter::LeftRight(float NewAxisValue)
{
	switch (CurrentControlMode)
	{
	case EControlMode::GTA:
		AddMovementInput(FRotationMatrix(FRotator(0.0f, GetControlRotation().Yaw, 0.0f)).GetUnitAxis(EAxis::Y), NewAxisValue);
		break;

	case EControlMode::DIABLO:
		DirectionToMove.Y = NewAxisValue;
	}
}

void AABCharacter::LookUp(float NewAxisValue)
{
	switch (CurrentControlMode)
	{
	case EControlMode::GTA:
		AddControllerPitchInput(NewAxisValue);
		break;
	}
}

void AABCharacter::Turn(float NewAxisValue)
{
	switch (CurrentControlMode)
	{
	case EControlMode::GTA:
		AddControllerYawInput(NewAxisValue);
		break;
	}
}

void AABCharacter::ViewChange()
{
	switch (CurrentControlMode)
	{
		case EControlMode::GTA:
		{
			GetController()->SetControlRotation(GetActorRotation());
			SET_CONTROL_MODE SCMMessage;
			SCMMessage.ControlMode = EControlMode::DIABLO;;
			HandleMessage(SCMMessage);
			break;
		}
		case EControlMode::DIABLO:
		{
			GetController()->SetControlRotation(SpringArm->GetRelativeRotation());
			SET_CONTROL_MODE SCMMessage;
			SCMMessage.ControlMode = EControlMode::GTA;
			HandleMessage(SCMMessage);
			break;
		}
	}
}

void AABCharacter::Attack()
{
	ATTACK ATKMessage;
	HandleMessage(ATKMessage);
}

void AABCharacter::OnAttackMontageEnded(UAnimMontage * Montage, bool bInterrupted)
{
	ABCHECK(IsAttacking);
	ABCHECK(CurrentCombo > 0);
	IsAttacking = false;
	ATTACK_END_COMBO_STATE AECSMessage;
	HandleMessage(AECSMessage);
	ON_ATTACK_END OAEMessgae;
	HandleMessage(OAEMessgae);
}

void AABCharacter::AttackCheck()
{
	GET_FINAL_ATTACK_RANGE GFARMessage;
	HandleMessage(GFARMessage);
	float FinalAttackRange = GFARMessage.FinalAttackRange;

	FHitResult HitResult;
	FCollisionQueryParams Params(NAME_None, false, this);
	bool bResult = GetWorld()->SweepSingleByChannel(
		HitResult,
		GetActorLocation(),
		GetActorLocation() + GetActorForwardVector() * FinalAttackRange,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel2,
		FCollisionShape::MakeSphere(AttackRadius),
		Params);

#if ENABLE_DRAW_DEBUG
	
	FVector TraceVec = GetActorForwardVector() * FinalAttackRange;
	FVector Center = GetActorLocation() + TraceVec * 0.5f;
	float HalfHeight = FinalAttackRange * 0.5f + AttackRadius;
	FQuat CapsuleRot = FRotationMatrix::MakeFromZ(TraceVec).ToQuat();
	FColor DrawColor = bResult ? FColor::Green : FColor::Red;
	float DebugLifeTime = 5.0f;

	DrawDebugCapsule(GetWorld(),
		Center,
		HalfHeight,
		AttackRadius,
		CapsuleRot,
		DrawColor,
		false,
		DebugLifeTime);

#endif

	if (bResult)
	{
		if (HitResult.Actor.IsValid())
		{
			FDamageEvent DamageEvent;
			GET_FINAL_ATTACK_DAMAGE GFADMessage;
			HandleMessage(GFADMessage);
			HitResult.Actor->TakeDamage(GFADMessage.FinalDamage, DamageEvent, GetController(), this);
		}
	}
}

void AABCharacter::OnAssetLoadCompleted()
{
	USkeletalMesh* AssetLoaded = Cast<USkeletalMesh>(AssetStreamingHandle->GetLoadedAsset());
	AssetStreamingHandle.Reset();
	ABCHECK(nullptr != AssetLoaded);
	GetMesh()->SetSkeletalMesh(AssetLoaded);

	SET_CHARACTER_STATE SCSMessage;
	SCSMessage.CharacterState = ECharacterState::READY;
	HandleMessage(SCSMessage);
}

