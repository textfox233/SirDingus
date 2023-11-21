// Copyright Epic Games, Inc. All Rights Reserved.

#include "SirDingusCharacter.h"

#include "SirDingus/Components/HealthComponent.h"
#include "SirDingus/Components/MeleeComponent.h"
#include "SirDingus/Interface/ActionStateInterface.h"
#include "SirDingus/Controllers/SirDingusAIController.h"
#include "SirDingus/Macros/Debug Macros.h"
#include "SirDingus/Modes & States/SirDingusGameMode.h"
#include "SirDingus/Weapons/Weapon.h"

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

void ASirDingusCharacter::TestMontageAnimation(UAnimMontage* Montage, FName Section)
{
	// play animation
	if (bAlive && Montage)
	{
		PlayAnimMontageServer(Montage, Section);
	}
}

void ASirDingusCharacter::TestPrivateFunction()
{
	//// Test flinch animations
	//TestMontageAnimation(FlinchMontage, "FromBack");

	//// Test death animations
	//TestMontageAnimation(DeathMontage, "Death 02");

	//Die();

	//UPrimitiveComponent* Capsule = Cast<UPrimitiveComponent>(GetCapsuleComponent());
	//if (Capsule)
	//{
	//	if (bDebugLogs)
	//		UE_LOG(LogTemp, Display, TEXT("Disabling collision capsule on %s"), *GetName());
	//	DisableCapsuleCollisionServer(Capsule);
	//}
	//else
	//{
	//	if (bDebugLogs)
	//		UE_LOG(LogTemp, Error, TEXT("Failed to retrieve collision capsule on %s"), *GetName());
	//}
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
}

void ASirDingusCharacter::GetHit(const FVector& ImpactPoint)
{
	// if it's on the server or an autonomous proxy (chance this may activate twice)
	ENetRole NetRole{ GetLocalRole() };
	if (NetRole == ENetRole::ROLE_Authority || NetRole == ENetRole::ROLE_AutonomousProxy)
	{
		if (bDebugMessages && GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				2.f,
				FColor::Green,
				FString(TEXT("ASirDingusCharacter::GetHit()"))
			);
		}
		if (bDebugLogs)
		{
			UE_LOG(LogTemp, Warning, TEXT("ASirDingusCharacter::GetHit()->%s"), *GetNetRole());
		}

		// take the damage
		bool bSurvived = HealthComponent->TakeDamage(20.f);

		// HealthComponent says live
		if (bSurvived)
			ReactToHit(ImpactPoint);

		// HealthComponent says die
		else
			Die();

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
	}
}

void ASirDingusCharacter::ReactToHit(const FVector& ImpactPoint)
{
	const FVector Forward = GetActorForwardVector();
	const FVector ImpactLowered(ImpactPoint.X, ImpactPoint.Y, GetActorLocation().Z);
	const FVector ToHit = (ImpactLowered - GetActorLocation()).GetSafeNormal();

	// 1. Determine Angle
	// A * B = |A||B| * cos(theta)
	// |A| = 1, |B| = 1, so A * B = cos(theta)
	const double CosTheta = FVector::DotProduct(Forward, ToHit);
	// Take the inverse cosine of cos(theta) to get theta
	double Theta = FMath::Acos(CosTheta);
	// convert from radians to degrees
	Theta = FMath::RadiansToDegrees(Theta);

	// 2. Determine Positive / Negative
	const FVector CrossProduct = FVector::CrossProduct(Forward, ToHit);
	// if CrossProduct's pointing down, make Theta negative
	if (CrossProduct.Z < 0) 
		Theta *= -1.f;

	// 3. Determine Correct animation
	FName Section = "Rooted";

	if (Theta > 45.f && Theta < 135.f)			Section = "FromRight";	// Right
	else if (Theta > -45.f && Theta < 45.f)		Section = "FromFront";	// Forward
	else if (Theta > -135.f && Theta < -45.f)	Section = "FromLeft";	// Left
	else if (Theta > 135.f || Theta < -135.f)	Section = "FromBack";	// Back

	// play hit react animation
	if (bAlive && FlinchMontage)
		PlayAnimMontageServer(FlinchMontage, Section);

	if (bDebugMessages && GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			3.f,
			FColor::Blue,
			FString::Printf(TEXT("ASirDingusCharacter::TakeDamage -> Angle is %f"), Theta)
		);
	}
	if (bDrawDebug)
	{
		DRAW_SPHERE(ImpactPoint);
		UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + Forward * 60.f, 5.f, FColor::Red, 5.f);
		UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + ToHit * 60.f, 5.f, FColor::Green, 5.f);
		UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + CrossProduct * 100.f, 5.f, FColor::Blue, 5.f);
	}
}

// Play a given montage over the network
void ASirDingusCharacter::PlayAnimMontageServer_Implementation(UAnimMontage* AnimMontage, 
	const FName& StartSectionName)
{
	PlayAnimMontageMulticast(AnimMontage, StartSectionName);
}

// Play a given montage on each client
void ASirDingusCharacter::PlayAnimMontageMulticast_Implementation(UAnimMontage* AnimMontage, 
	const FName& StartSectionName)
{
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
	if (bDebugLogs)
	{
		if (CurrentGameMode)
		{
			UE_LOG(LogTemp, Display, TEXT("CurrentGameMode successfully populated in %s BeginPLay()"), *GetName());
			UE_LOG(LogTemp, Display, TEXT("NetRole is %s"), *GetNetRole());
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("CurrentGameMode failed to populate in %s BeginPLay()"), *GetName());
			UE_LOG(LogTemp, Error, TEXT("NetRole is %s"), *GetNetRole());
		}
	}

	//if (bDebugMessages && GEngine)
	//{
	//	GEngine->AddOnScreenDebugMessage(
	//		-1,
	//		15.f,
	//		FColor::Yellow,
	//		FString(TEXT("ASirDingusCharacter::BeginPlay()"))
	//	);
	//}
}

void ASirDingusCharacter::Die()
{
	if (!HasAuthority())
		return;

	// mark as dead
	bAlive = false;

	// if ai controlled, run this too to inform the blackboard - probably not the best solution, but leaving it for now
	if (AController* rawController = Controller)
	{
		if (ASirDingusAIController* AIController = Cast<ASirDingusAIController>(rawController))
		//if (AAIController* AIController = Cast<AAIController>(rawController))
		{
			// update blackboard value
			AIController->SetIsAlive(false);

			//// clear focus to stop rotation
			//AIController->ClearFocus(EAIFocusPriority::Gameplay);
			//// inform the blackboard of the AI's death
			//AIController->GetBlackboardComponent()->SetValueAsBool(TEXT("IsAlive"), bValue);
		}
	}

	ReportDeath();

	// get capsule component - error here
	UPrimitiveComponent* Capsule = Cast<UPrimitiveComponent>(GetCapsuleComponent());
	// set to no collision
	if (Capsule) { DisableCapsuleCollisionServer(Capsule); }

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
	if (DeathMontage)
		PlayAnimMontageServer(DeathMontage, "Death 02");
}

void ASirDingusCharacter::ReportDeath_Implementation()
{
	// if current gamemode is nullptr get it, otherwise set to current self / do nothing
	CurrentGameMode = CurrentGameMode == nullptr ? Cast<ASirDingusGameMode>(UGameplayStatics::GetGameMode(this)) : CurrentGameMode;

	// inform gamemode character has died
	if (CurrentGameMode)
		CurrentGameMode->AddToDeathToll(Controller);
	else if (bDebugLogs)
	{
		UE_LOG(LogTemp, Error, TEXT("CurrentGameMode is nullptr in %s"), *GetName());
		UE_LOG(LogTemp, Error, TEXT("NetMode :: %s"), *GetNetRole());
	}
}

void ASirDingusCharacter::Dodge()
{
	if (bAlive)
	{
		PlayAnimMontageServer(DodgeMontage);
	}
}

void ASirDingusCharacter::Attack()
{
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
		// check interface works
		if (bool bControllerUsesInterface = GetController()->Implements<UActionStateInterface>()) 
		{
			IActionStateInterface* ActionStateObject = Cast<IActionStateInterface>(GetController());
			if (ActionStateObject->GetActionState() == EActionState::EAS_Unoccupied)
			{
				if (MeleeComponent)
					MeleeComponent->PerformAttack();
				else if (bDebugMessages && GEngine)
				{
					FString message = TEXT("Missing melee component");
					GEngine->AddOnScreenDebugMessage(
						2,
						15.f,
						FColor::Red,
						message
					);
				}
			}
			else if (bDebugMessages && GEngine)
			{
				FString message = TEXT("Action state is not unnoccupied");
				GEngine->AddOnScreenDebugMessage(
					2,
					15.f,
					FColor::Red,
					message
				);
			}
		}
		else if (bDebugMessages && GEngine)
		{
			FString message = TEXT("Controller does not implement UActionStateInterface");
			GEngine->AddOnScreenDebugMessage(
				2,
				15.f,
				FColor::Red,
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