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

	// Bind my own take damage event
	OnTakeAnyDamage.AddDynamic(this, &ASirDingusCharacter::DamageTaken);

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void ASirDingusCharacter::DamageTaken(AActor* DamagedActor, float Damage, const UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	// Debug Msg
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			15.f,
			FColor::Yellow,
			FString(TEXT("Take damage event"))
		);
	}
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

	//Initialise Health Value
	Health = MaxHealth;

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

//bool ASirDingusCharacter::IsDead()
//{
//	return Health <= 0;
//}

bool ASirDingusCharacter::IsDead(int dmg = 0)
{
	// apply damage
	Health -= dmg;

	// if health is less than 0, character is dead
	if (Health <= 0)
	{
		// clamp health to 0, return dead
		Health = 0;
		return true;
	}
	// return live
	return false;
}

// Refactored blueprint function
void ASirDingusCharacter::ProcessMeleeHit(AActor* hitActor)
{
	// check tags to see what is being damaged
	// dont damage players
	if (hitActor->ActorHasTag("Player"))
	{
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
	// deal damage to hit actors
	UClass* DamageTypeClass = UDamageType::StaticClass();
	UGameplayStatics::ApplyDamage(
		hitActor,
		50,
		Controller,
		this,
		DamageTypeClass
	);
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			15.f,
			FColor::Yellow,
			FString(TEXT("Valid Target"))
		);
	}
}

AActor* ASirDingusCharacter::DrawWeaponArc(bool bDrawDebug)
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
		//FVector traceStart = skMesh->FindSocket("Base")->GetSocketTransform(skComp).GetLocation();
		//FVector traceEnd = skMesh->FindSocket("Tip")->GetSocketTransform(skComp).GetLocation();
		FVector traceStart = skMesh->GetSocketByIndex(0)->GetSocketTransform(skComp).GetLocation();
		FVector traceEnd = skMesh->GetSocketByIndex(1)->GetSocketTransform(skComp).GetLocation();

		// collision parameters - ignore self
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);

		// perform line trace
		GetWorld()->LineTraceSingleByChannel(Hit,traceStart, traceEnd, ECollisionChannel::ECC_Camera, QueryParams);

		// Debug
		if (bDrawDebug)
		{
			DrawDebugLine(GetWorld(), traceStart, traceEnd, Hit.bBlockingHit ? FColor::Green : FColor::Red, false, 5.0f, 0, 1.0f);
			UE_LOG(LogTemp, Log, TEXT("Tracing line: %s to %s"), *traceStart.ToCompactString(), *traceEnd.ToCompactString());
		}

		// if hit occurs and hit actor is valid
		if (Hit.bBlockingHit && IsValid(Hit.GetActor()))
		{
			// return hit actor
			UE_LOG(LogTemp, Log, TEXT("Trace hit actor: %s"), *Hit.GetActor()->GetName());
			return Hit.GetActor();
		}

		// nothing hit
		UE_LOG(LogTemp, Log, TEXT("No actors hit"));
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
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &ASirDingusCharacter::AttackEvent);
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

// Currently Unused, AttackEvent is called instead
void ASirDingusCharacter::Attack(const FInputActionValue& Value)
{
	// input is a bool
	bIsAttacking = Value.Get<bool>();

	//DEBUG MESSAGE
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			15.f,
			FColor::Yellow,
			FString::Printf(TEXT("Attacking = %s"), (bIsAttacking ? TEXT("true") : TEXT("false") ))
		);
	}
}




