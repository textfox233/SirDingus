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
#include "Weapon.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SceneComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Engine/EngineTypes.h"
#include "HealthComponent.h"

//////////////////////////////////////////////////////////////////////////
// ASirDingusCharacter

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

//void ASirDingusCharacter::DamageTaken(AActor* DamagedActor, float Damage, const UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
//{
//	// Debug Msg
//	if (GEngine)
//	{
//		GEngine->AddOnScreenDebugMessage(
//			-1,
//			15.f,
//			FColor::Green,
//			FString(TEXT("ASirDingusCharacter::DamageTaken()"))
//		);
//	}
//}

float ASirDingusCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float result = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	//// Debug Msg
	//if (GEngine)
	//{
	//	GEngine->AddOnScreenDebugMessage(
	//		-1,
	//		2.f,
	//		FColor::Green,
	//		FString(TEXT("ASirDingusCharacter::TakeDamage()"))
	//	);
	//}

	// if character has died
	if (HasDied())
	{
		// mark as dead
		bAlive = false;

		// play death animation
		if (DeathMontage)
		{
			PlayAnimMontageServer(DeathMontage);
		}
	}
	else
	{
		// play flinch animation
		if (FlinchMontage)
		{
			PlayAnimMontageServer(FlinchMontage);
		}
	}

	return result;
}

// * Refactored blueprint function
// Play a given montage over the network
void ASirDingusCharacter::PlayAnimMontageServer_Implementation(UAnimMontage* AnimMontage)
{
	PlayAnimMontageMulticast(AnimMontage);
}

// * Refactored blueprint function
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

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

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

bool ASirDingusCharacter::HasDied()
{
	//// apply damage
	//Health -= dmg;
	//
	//// if health is less than 0, character is dead
	//if (Health <= 0)
	//{
	//	// clamp health to 0, return dead
	//	Health = 0;
	//	return true;
	//}
	 
	// check if health is above 0
	UHealthComponent* healthComp = FindComponentByClass<UHealthComponent>();

	// check for valid component
	if (!healthComp)
	// if component is invalid, log error + exit early
	{
		// Debug Msg
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.f,
				FColor::Red,
				FString(TEXT("healthComp invalid"))
			);
		}
		return false;
	}
	else
	// otherwise test the health
	{
		//// Debug Msg
		//if (GEngine)
		//{
		//	GEngine->AddOnScreenDebugMessage(
		//		-1,
		//		15.f,
		//		FColor::Green,
		//		FString(TEXT("healthComp valid"))
		//	);
		//}

		// if health is below 0 (or equal), character is dead
		return healthComp->GetHealth() <= 0;
	}
	//// just in case
	//return false;
}

/** Refactored Blueprint Functionality -- Melee Swing Line Traces **/
// -- Set linetraces to occur until TriggerMeleeEnd()
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
	AActor* hit = DrawWeaponArc(true);

	//if (hit->IsValidLowLevel())
	if (hit != nullptr)
	// anything hit?
	{
		// process hit
		ProcessMeleeHit(hit);
	}
}
// End traces
void ASirDingusCharacter::MeleeTraceEnd()
{
	// clear timer
	GetWorld()->GetTimerManager().ClearTimer(MeleeTraceHandle);
}

// -- Process melee hits (is it a player, damage dealing etc)
void ASirDingusCharacter::ProcessMeleeHit(AActor* hitActor, bool bDebugLog )
{
	if (bDebugLog) {
		UE_LOG(LogTemp, Log, TEXT("ASirDingusCharacter::ProcessMeleeHit"));
		UE_LOG(LogTemp, Warning, TEXT("%s Hit"), *hitActor->GetName());
	}

	// players shouldn't be able to damage players
	if(this->ActorHasTag("Player"))
	{
		// check tags to see what is being damaged
		if (hitActor->ActorHasTag("Player"))
		{
			if (bDebugLog) { UE_LOG(LogTemp, Log, TEXT("Target is a player")); }

			//DEBUG MESSAGE
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(
					-1,
					15.f,
					FColor::Yellow,
					FString(TEXT("Hit target is player"))
				);
			}
			return;
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

	//if (GEngine)
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

//////////////////////////////////////////////////////////////////////////
// Input

void ASirDingusCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Are these needed here? Does AI use these?

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		//Dodging
		EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Triggered, this, &ASirDingusCharacter::Dodge);
		EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Completed, this, &ASirDingusCharacter::StopDodging);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASirDingusCharacter::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASirDingusCharacter::Look);

		//Attacking
		//EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &ASirDingusCharacter::AttackEvent);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &ASirDingusCharacter::Attack);
	}

}

void ASirDingusCharacter::Dodge(const FInputActionValue& Value)
{
	// input is a bool
	bool bIsDodging = Value.Get<bool>();

	//DEBUG MESSAGE
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			15.f,
			FColor::Yellow,
			FString::Printf(TEXT("Dodging = %s"), (bIsDodging ? TEXT("true") : TEXT("false")))
		);
	}
}

void ASirDingusCharacter::StopDodging()
{
	//DEBUG MESSAGE
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			15.f,
			FColor::Yellow,
			FString::Printf(TEXT("Stop Dodging"))
		);
	}
}

void ASirDingusCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);

		////DEBUG MESSAGE
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

void ASirDingusCharacter::Attack(const FInputActionValue& Value)
{
	if (BasicAttackMontage)
	{
		PlayAnimMontageServer(BasicAttackMontage);
	}
	//bool bValid = BasicAttackMontage->HasValidSlotSetup();
	//BasicAttackMontage->HasValidSlotSetup();


	// input is a bool
	bIsAttacking = Value.Get<bool>();

	//DEBUG MESSAGE
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
