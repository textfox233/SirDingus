// Copyright Epic Games, Inc. All Rights Reserved.

#include "SirDingusCharacter.h"

#include "SirDingus/Weapons/Weapon.h"
#include "SirDingus/Components/HealthComponent.h"
#include "SirDingus/Components/MeleeComponent.h"
#include "SirDingus/Modes & States/SirDingusGameMode.h"
#include "SirDingus/Macros/Debug Macros.h"

#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/SceneComponent.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

#include "Engine/SkeletalMeshSocket.h"
#include "Engine/EngineTypes.h"

#include "Camera/CameraComponent.h"
#include "Net/UnrealNetwork.h"

//////////////////////////////////////////////////////////////////////////
/// ASirDingusCharacter

void ASirDingusCharacter::DisableCapsuleCollisionServer_Implementation(UPrimitiveComponent* Capsule)
{
	DisableCapsuleCollisionMulticast(Capsule);
	//Capsule->SetCollisionProfileName(TEXT("NoCollision"));
}

void ASirDingusCharacter::DisableCapsuleCollisionMulticast_Implementation(UPrimitiveComponent* Capsule)
{
	// set to no collision
	Capsule->SetCollisionProfileName(TEXT("Spectator"));
}

void ASirDingusCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASirDingusCharacter, bAlive);
}

ASirDingusCharacter::ASirDingusCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	/** Components **/
	// -- Health
	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
	// -- Melee
	MeleeComponent = CreateDefaultSubobject<UMeleeComponent>(TEXT("MeleeComponent"));

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	//// Load Animation Montage
	//static ConstructorHelpers::FObjectFinder<UAnimMontage>BasicAttackMontageObject(TEXT("/Game/Blueprints/Characters/Animation/Great_Sword_Slash_Vertical_Montage.Great_Sword_Slash_Vertical_Montage"));
	//if (BasicAttackMontageObject.Succeeded())
	//// if Successful THEN assign object to variable
	//{
	//	BasicAttackMontage = BasicAttackMontageObject.Object;
	//}
}

float ASirDingusCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float superResult = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (bDebugMessages && GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			2.f,
			FColor::Green,
			FString(TEXT("ASirDingusCharacter::TakeDamage()"))
		);
	}

	// play flinch animation
	if (bAlive && FlinchMontage)
	{
		PlayAnimMontageServer(FlinchMontage);
	}

	if (bDebugMessages && GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			3.f,
			FColor::Yellow,
			FString::Printf(TEXT("ASirDingusCharacter::TakeDamage -> bAlive: %s"), bAlive ? TEXT("true") : TEXT("false"))
		);
	
		GEngine->AddOnScreenDebugMessage(
			-1,
			3.f,
			FColor::Yellow,
			FString::Printf(TEXT("ASirDingusCharacter::TakeDamage -> Role is %s"), *GetNetRole())
		);
	}

	return superResult;
}

void ASirDingusCharacter::GetHit(const FVector& ImpactPoint)
{
	DRAW_SPHERE(ImpactPoint);
}

void ASirDingusCharacter::ResetActionState()
{
	ActionState = EActionState::EAS_Unoccupied;
}
void ASirDingusCharacter::Interrupted()
{
	ActionState = EActionState::EAS_Interrupted;
}

/// * Refactored blueprint function
// Play a given montage over the network
void ASirDingusCharacter::PlayAnimMontageServer_Implementation(UAnimMontage* AnimMontage, FName StartSectionName)
{
	PlayAnimMontageMulticast(AnimMontage, StartSectionName);
}

/// * Refactored blueprint function
// Play a given montage on each client
void ASirDingusCharacter::PlayAnimMontageMulticast_Implementation(UAnimMontage* AnimMontage, FName StartSectionName)
{
	//float animTime = 0.1f;		//Default value in case the animation can't be played.
	//
	//Log(ELogLevel::INFO, __FUNCTION__);
	//
	//// try and play a firing animation if specified
	//if (AnimMontage != NULL && playerViewMesh != NULL)
	//{
	//	// Get the animation object for the arms mesh
	//	UAnimInstance* AnimInstance = playerViewMesh->GetAnimInstance();
	//	if (AnimInstance != NULL)
	//	{
	//		animTime = AnimInstance->Montage_Play(FireAnimation, 1.f);
	//		if (animTime < 0.1f)
	//		{
	//			//The function returns 0 if the animation can't be played.
	//			animTime = 0.1f;
	//		}
	//	}
	//}

	// just play the montage
	PlayAnimMontage(AnimMontage, 1.0f, StartSectionName);
}

void ASirDingusCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	// -- Initialisers -- //
	
	// get current gamemode
	CurrentGameMode = Cast<ASirDingusGameMode>(UGameplayStatics::GetGameMode(this));

	ActionState = EActionState::EAS_Unoccupied;

	if (bDebugMessages && GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			15.f,
			FColor::Yellow,
			FString(TEXT("ASirDingusCharacter::BeginPlay()"))
		);
	}
}

void ASirDingusCharacter::CharacterDeath()
{
	// mark as dead
	bAlive = false;

	// get capsule component
	UPrimitiveComponent* Capsule = Cast<UPrimitiveComponent>(GetCapsuleComponent());

	// set to no collision
	if (Capsule) { DisableCapsuleCollisionMulticast(Capsule); }

	if (bDebugMessages && GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			3.f,
			FColor::Yellow,
			FString::Printf(TEXT("ASirDingusCharacter::CharacterDeath -> Capsule is %s"), Capsule ? TEXT("valid") : TEXT("invalid"))
		);
	}

	// play death animation
	if (DeathMontage) { PlayAnimMontageServer(DeathMontage); }
}

void ASirDingusCharacter::Dodge()
{
	//if (bDebugMessages && GEngine)
	//{
	//	GEngine->AddOnScreenDebugMessage(
	//		-1,
	//		15.f,
	//		FColor::Yellow,
	//		FString::Printf(TEXT("Dodging = %s"), (bIsDodging ? TEXT("true") : TEXT("false")))
	//	);
	//}
	if (bAlive)
	{
		PlayAnimMontageServer(DodgeMontage);
	}
}

///void ASirDingusCharacter::StopDodging()
//{
//	//DEBUG MESSAGE
//	//if ( GEngine)
//	//{
//	//	GEngine->AddOnScreenDebugMessage(
//	//		-1,
//	//		15.f,
//	//		FColor::Yellow,
//	//		FString::Printf(TEXT("Stop Dodging"))
//	//	);
//	//}
//}

void ASirDingusCharacter::Attack()
{
	///DEBUG MESSAGE - Errors in this debug message, ignore for now
	if (bDebugMessages && GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
		1,
		15.f,
		FColor::Green,
		FString::Printf(TEXT("ASirDingusCharacter::Attack()"))
		);
	}

	if (bAlive)
	{
		if (ActionState == EActionState::EAS_Unoccupied)
		{
			if (MeleeComponent)
			{
				MeleeComponent->PerformAttack();
				ActionState = EActionState::EAS_Attacking;
			}
		}
		else if (bDebugMessages && GEngine) 
		{
			FString message = TEXT("Action state is not unnoccupied");
			GEngine->AddOnScreenDebugMessage(
				2,
				15.f,
				FColor::Red,
				//FString::Printf(TEXT("ActionState = %s"), (ActionState))
				message
			);
		}
	}
}

FString ASirDingusCharacter::GetNetRole()
{
	ENetRole localRole = this->GetLocalRole();
	FString role;
	if (localRole)
	{
		switch (localRole)
		{
		case ENetRole::ROLE_Authority:
			return FString("Authority");
			break;
		case ENetRole::ROLE_AutonomousProxy:
			return FString("AutonomousProxy");
			break;
		case ENetRole::ROLE_MAX:
			return FString("MAX");
			break;
		case ENetRole::ROLE_None:
			return FString("AutNonehority");
			break;
		case ENetRole::ROLE_SimulatedProxy:
			return FString("SimulatedProxy");
			break;
		}
	}
	return FString("ERROR: local role not found");
}