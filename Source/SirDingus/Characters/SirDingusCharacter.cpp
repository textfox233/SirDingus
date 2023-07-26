// Copyright Epic Games, Inc. All Rights Reserved.

#include "SirDingusCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "SirDingus/Weapons/Weapon.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/SceneComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Engine/EngineTypes.h"

#include "SirDingus/Components/HealthComponent.h"
#include "SirDingus/Components/MeleeComponent.h"

#include "SirDingus/Modes & States/SirDingusGameMode.h"
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
	/// Debug Msg
	//if (GEngine)
	//{
	//	GEngine->AddOnScreenDebugMessage(
	//		-1,
	//		2.f,
	//		FColor::Green,
	//		FString(TEXT("ASirDingusCharacter::TakeDamage()"))
	//	);
	//}

	// play flinch animation
	if (bAlive && FlinchMontage)
	{
		PlayAnimMontageServer(FlinchMontage);
	}

	/// Debug Message
	//if (GEngine)
	//{
	//	GEngine->AddOnScreenDebugMessage(
	//		-1,
	//		3.f,
	//		FColor::Yellow,
	//		FString::Printf(TEXT("ASirDingusCharacter::TakeDamage -> bAlive: %s"), bAlive ? TEXT("true") : TEXT("false"))
	//	);
	//
	//	GEngine->AddOnScreenDebugMessage(
	//		-1,
	//		3.f,
	//		FColor::Yellow,
	//		FString::Printf(TEXT("ASirDingusCharacter::TakeDamage -> Role is %s"), *GetNetRole())
	//	);
	//}

	return superResult;
}

/// * Refactored blueprint function
// Play a given montage over the network
void ASirDingusCharacter::PlayAnimMontageServer_Implementation(UAnimMontage* AnimMontage)
{
	PlayAnimMontageMulticast(AnimMontage);
}

/// * Refactored blueprint function
// Play a given montage on each client
void ASirDingusCharacter::PlayAnimMontageMulticast_Implementation(UAnimMontage* AnimMontage)
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
	PlayAnimMontage(AnimMontage);
}

void ASirDingusCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	// bind DamageTaken callback to OnTakeAnyDamage delegate - this line crashes editor in multiplayer, unsure why
	//GetOwner()->OnTakeAnyDamage.AddDynamic(this, &ASirDingusCharacter::DamageTaken);

	//Spawn Weapon
	if (EquippedWeaponClass)
	{
		// spawn weapon
		EquippedWeapon = GetWorld()->SpawnActor<AWeapon>(EquippedWeaponClass);
		// attach to socket (right hand)
		EquippedWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("weaponSocket_r"));
		// set owner (for later)
		EquippedWeapon->SetOwner(this);
	}
	else
	{
		//DEBUG MESSAGE
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.f,
				FColor::Red,
				FString(TEXT("No Weapon Equipped"))
			);
		}
	}

	// get current gamemode
	CurrentGameMode = Cast<ASirDingusGameMode>(UGameplayStatics::GetGameMode(this));

	//DEBUG MESSAGE
	//if (GEngine)
	//{
	//	GEngine->AddOnScreenDebugMessage(
	//		-1,
	//		15.f,
	//		FColor::Yellow,
	//		FString(TEXT("ASirDingusCharacter::BeginPlay()"))
	//	);
	//}
}

void ASirDingusCharacter::CharacterDeath()
{
	// mark as dead
	bAlive = false;

	// get capsule component
	UPrimitiveComponent* Capsule = Cast<UPrimitiveComponent>(GetCapsuleComponent());

	// set to no collision
	//Capsule->SetCollisionProfileName(TEXT("NoCollision"));
	//if (Capsule) { DisableCapsuleCollisionServer(Capsule); }
	if (Capsule) { DisableCapsuleCollisionMulticast(Capsule); }

	/// Debug Message
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			3.f,
			FColor::Yellow,
			//FString::Printf(TEXT("ASirDingusCharacter::CharacterDeath -> bAlive: %s"), bAlive ? TEXT("true") : TEXT("false"))
			FString::Printf(TEXT("ASirDingusCharacter::CharacterDeath -> Capsule is %s"), Capsule ? TEXT("valid") : TEXT("invalid"))
		);
	}

	// play death animation
	if (DeathMontage)
	{
		PlayAnimMontageServer(DeathMontage);
	}
}

/** Refactored Blueprint Functionality -- Melee Swing Line Traces **/
/// -- Set linetraces to occur until TriggerMeleeEnd()
// Start traces
void ASirDingusCharacter::MeleeTraceStart()
{
	// start timer
	GetWorld()->GetTimerManager().SetTimer(MeleeTraceHandle, this, &ASirDingusCharacter::MeleeTraceInProgress, 0.01f, true, 0.05f);
}
// Perform single trace
void ASirDingusCharacter::MeleeTraceInProgress()
{
	//UE_LOG(LogTemp, Warning, TEXT("timer active"));

	// perform line trace (debug? / logs?)
	AActor* hit = DrawWeaponArc();

	// process the hit
	if (ProcessMeleeHit(hit))
	// if hit was valid
	{
		// stop line tracing - should stop multiple hits per swing
		GetWorld()->GetTimerManager().ClearTimer(MeleeTraceHandle);
	}

	//if (hit != nullptr)
	//// anything hit?
	//{
	//	// process hit
	//	if (ProcessMeleeHit(hit))
	//	// if hit was valid
	//	{
	//
	//	}
	//}
}
// End traces
void ASirDingusCharacter::MeleeTraceEnd()
{
	// clear timer
	GetWorld()->GetTimerManager().ClearTimer(MeleeTraceHandle);
}

// -- Process melee hits (TRUE means damage was dealt, FALSE means the hit was invalid)
bool ASirDingusCharacter::ProcessMeleeHit(AActor* hitActor, bool bDebugLog )
{
	// Was there no hit?
	if (hitActor == nullptr)
	{
		// FALSE: target invalid
		return false;
	}
	
	if (bDebugLog) {
		UE_LOG(LogTemp, Log, TEXT("ASirDingusCharacter::ProcessMeleeHit"));
		UE_LOG(LogTemp, Warning, TEXT("%s Hit"), *hitActor->GetName());
	}

	// Was the hit actor a dead character?
	if (ASirDingusCharacter* Character = Cast<ASirDingusCharacter>(hitActor))
	{
		// if character is not alive
		if (!Character->bAlive)
		{
			/// Debug
			if (bDebugLog && GEngine)
			{
				GEngine->AddOnScreenDebugMessage(
					-1,
					2.f,
					FColor::Yellow,
					FString(TEXT("Target is already dead"))
				);
			}
			// FALSE: target invalid
			return false;
		}
	}

	// Did a player just hit another player?
	if(this->ActorHasTag("Player"))
	{
		// check tags to see what is being damaged
		if (hitActor->ActorHasTag("Player"))
		{
			/// Debug
			if (bDebugLog) 
			{
				UE_LOG(LogTemp, Log, TEXT("Target is a player"));
				
				if (GEngine)
				{
					GEngine->AddOnScreenDebugMessage(
						-1,
						15.f,
						FColor::Yellow,
						FString(TEXT("Hit target is player"))
					);
				}
			}

			// FALSE: target invalid
			return false;
		}
	}

	if (bDebugLog) { UE_LOG(LogTemp, Log, TEXT("Target not a player")); }

	// deal damage to hit actors
	UClass* DamageTypeClass = UDamageType::StaticClass();
	float dmgDealt = UGameplayStatics::ApplyDamage(
		hitActor,		// DamagedActor - Actor that will be damaged.
		50,				// BaseDamage - The base damage to apply.
		Controller,		// EventInstigator - Controller that was responsible for causing this damage (e.g. player who swung the weapon)
		this,			// DamageCauser - Actor that actually caused the damage (e.g. the grenade that exploded)
		DamageTypeClass	// DamageTypeClass - Class that describes the damage that was done.
	);
	if (bDebugLog) { UE_LOG(LogTemp, Log, TEXT("damage dealt: %f"), dmgDealt); }
	if (bDebugLog) { 
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				3.f,
				FColor::Yellow,
				FString::Printf(TEXT("ASirDingusCharacter::ProcessMeleeHit -> damage dealt: %f"), dmgDealt)
			);
		};
	}

	// TRUE: damage was dealt
	return true;

	///if (GEngine)
	//{
	//	GEngine->AddOnScreenDebugMessage(
	//		-1,
	//		15.f,
	//		FColor::Yellow,
	//		FString(TEXT("Valid Target"))
	//	);
	//}
}

// -- Draw a line trace to track a weapon's movement and detect hit events
AActor* ASirDingusCharacter::DrawWeaponArc(bool bDrawDebug, bool bDebugLog)
{
	// define points for line trace
	if (EquippedWeapon)
	{
		FHitResult Hit;

		// grab skeleton
		const USkeletalMeshComponent* skComp = EquippedWeapon->FindComponentByClass<USkeletalMeshComponent>();
		USkeletalMesh* skMesh = skComp->GetSkeletalMeshAsset();
		//USkeletalMesh* skMesh = EquippedWeapon->FindComponentByClass<USkeletalMesh>();
		
		// grab sockets
		FVector traceStart = skMesh->GetSocketByIndex(0)->GetSocketTransform(skComp).GetLocation();
		FVector traceEnd = skMesh->GetSocketByIndex(1)->GetSocketTransform(skComp).GetLocation();

		// collision parameters - ignore self
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);

		// perform line trace
		GetWorld()->LineTraceSingleByChannel(Hit,traceStart, traceEnd, ECollisionChannel::ECC_Camera, QueryParams);

		// Debug
		if (bDrawDebug) { DrawDebugLine(GetWorld(), traceStart, traceEnd, Hit.bBlockingHit ? FColor::Green : FColor::Red, false, 5.0f, 0, 1.0f); }
		if (bDebugLog) { UE_LOG(LogTemp, Log, TEXT("Tracing line: %s to %s"), *traceStart.ToCompactString(), *traceEnd.ToCompactString()); }

		// if hit occurs and hit actor is valid
		if (Hit.bBlockingHit && IsValid(Hit.GetActor()))
		{
			if (bDebugLog) { UE_LOG(LogTemp, Log, TEXT("Trace hit actor: %s"), *Hit.GetActor()->GetName()); }
			// return hit actor
			return Hit.GetActor();
		}

		// nothing hit
		if (bDebugLog) { UE_LOG(LogTemp, Log, TEXT("No actors hit")); }
	}
	return nullptr;
}

void ASirDingusCharacter::Dodge()
{

	///DEBUG MESSAGE
	//if (GEngine)
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

void ASirDingusCharacter::StopDodging()
{
	///DEBUG MESSAGE
	//if (GEngine)
	//{
	//	GEngine->AddOnScreenDebugMessage(
	//		-1,
	//		15.f,
	//		FColor::Yellow,
	//		FString::Printf(TEXT("Stop Dodging"))
	//	);
	//}
}

void ASirDingusCharacter::Move(FVector2D MovementVector)
{
	if (bAlive)
	{
		if (Controller != nullptr)
		{
			// find out which way is forward
			const FRotator Rotation = GetControlRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);

			// get forward vector
			const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

			// get right vector 
			const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

			// add movement 
			AddMovementInput(ForwardDirection, MovementVector.Y);
			AddMovementInput(RightDirection, MovementVector.X);

			/// DEBUG MESSAGE
			//if (GEngine)
			//{
			//	GEngine->AddOnScreenDebugMessage(
			//		-1,
			//		15.f,
			//		FColor::Yellow,
			//		FString::Printf(TEXT("Moving"))
			//	);
			//}
		}
	}
}

// where camera is looking NOT model
void ASirDingusCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ASirDingusCharacter::SingleSwing()
{
	if (bAlive)
	{
		if (BasicAttackMontage)
		{
			PlayAnimMontageServer(BasicAttackMontage);
		}
	}

	///DEBUG MESSAGE
	//if (GEngine) 
	//{
	//	GEngine->AddOnScreenDebugMessage(
	//		-1,
	//		15.f,
	//		FColor::Yellow,
	//		//FString::Printf(TEXT("BasicAttackMontage->HasValidSlotSetup() = %s"), (BasicAttackMontage->HasValidSlotSetup() ? TEXT("true") : TEXT("false")))
	//		FString::Printf(TEXT("Attacking = %s"), (bIsAttacking ? TEXT("true") : TEXT("false")))
	//		//FString::Printf(TEXT("BasicAttackMontage = %s"), (*BasicAttackMontage->GetName()))
	//	);
	//}
}

void ASirDingusCharacter::RestartGame_Implementation(const FInputActionValue& Value)
{
	/// Debug Msg
	//if (GEngine)
	//{
	//	GEngine->AddOnScreenDebugMessage(
	//		-1,
	//		3.f,
	//		FColor::Yellow,
	//		TEXT("ASirDingusCharacter::RestartGame()")
	//	);
	//}
	if (CurrentGameMode)
	{
		if (CurrentGameMode->RequestRestart())
		{
			/// Debug Msg
			//if (GEngine)
			//{
			//	GEngine->AddOnScreenDebugMessage(
			//		-1,
			//		3.f,
			//		FColor::Green,
			//		TEXT("Restart Approved")
			//	);
			//}
		}
		else
		{
			/// Debug Msg
			//if (GEngine)
			//{
			//	GEngine->AddOnScreenDebugMessage(
			//		-1,
			//		3.f,
			//		FColor::Red,
			//		TEXT("Restart Declined")
			//	);
			//}
		}
	}
	else
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				3.f,
				FColor::Red,
				TEXT("Cannot Access Game Mode")
			);
		}
	}
}

void ASirDingusCharacter::QuitGame(const FInputActionValue& Value)
{
	/// Debug Msg
	//if (GEngine)
	//{
	//	GEngine->AddOnScreenDebugMessage(
	//		-1,
	//		3.f,
	//		FColor::Yellow,
	//		TEXT("ASirDingusCharacter::QuitGame()")
	//	);
	//}

	// if this is a player, exit the game
	if (APlayerController* thisPlayer = Cast<APlayerController>(GetController()))
	{
		UKismetSystemLibrary::QuitGame
		(
			this,
			thisPlayer,
			EQuitPreference::Quit,
			true
		);
	}

	else
	{
		/// Debug Msg
		//if (GEngine)
		//{
		//	GEngine->AddOnScreenDebugMessage(
		//		-1,
		//		3.f,
		//		FColor::Red,
		//		TEXT("Controller is not a PlayerController")
		//	);
		//}
	}

}

void ASirDingusCharacter::TestSomething(const FInputActionValue& Value)
{
	/// Debug Message
	//if (GEngine)
	//{
	//	GEngine->AddOnScreenDebugMessage(
	//		-1,
	//		3.f,
	//		FColor::Yellow,
	//		FString::Printf(TEXT("ASirDingusCharacter::TestSomething -> bAlive: %s"), bAlive ? TEXT("true") : TEXT("false"))
	//	);
	//}

	UClass* DamageTypeClass = UDamageType::StaticClass();
	UGameplayStatics::ApplyDamage(this, 50.f, Controller, this, DamageTypeClass);

	// Apply instant damage
	//this->TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController * EventInstigator, AActor * DamageCauser);
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