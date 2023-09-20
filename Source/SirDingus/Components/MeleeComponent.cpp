// Fill out your copyright notice in the Description page of Project Settings.


#include "MeleeComponent.h"

#include "SirDingus/Characters/SirDingusCharacter.h"
#include "SirDingus/Weapons/Weapon.h"

#include "Kismet/GameplayStatics.h"

#include "Engine/SkeletalMeshSocket.h"

// Sets default values for this component's properties
UMeleeComponent::UMeleeComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UMeleeComponent::BeginPlay()
{
	Super::BeginPlay();

	// Spawn Weapon
	if (EquippedWeaponClass)
	{
		// spawn weapon
		EquippedWeapon = GetWorld()->SpawnActor<AWeapon>(EquippedWeaponClass);
		// attach to socket (right hand)
		EquippedWeapon->AttachToComponent(Cast<ACharacter>(GetOwner())->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("weaponSocket_r"));
		// set owner (for later)
		EquippedWeapon->SetOwner(GetOwner());
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
}

/** Refactored Blueprint Functionality -- Melee Swing Line Traces **/
/// -- Set linetraces to occur until TriggerMeleeEnd()
// Start traces
void UMeleeComponent::MeleeTraceStart()
{
	// start timer
	GetWorld()->GetTimerManager().SetTimer(MeleeTraceHandle, this, &UMeleeComponent::MeleeTraceInProgress, 0.01f, true, 0.05f);
}
// Perform single trace
void UMeleeComponent::MeleeTraceInProgress()
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
void UMeleeComponent::MeleeTraceEnd()
{
	// clear timer
	GetWorld()->GetTimerManager().ClearTimer(MeleeTraceHandle);
}

// -- Process melee hits (TRUE means damage was dealt, FALSE means the hit was invalid)
bool UMeleeComponent::ProcessMeleeHit(AActor* hitActor, bool bDebugLog)
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

	// Was the hit actor an already dead character's capsule?
	if (ASirDingusCharacter* Character = Cast<ASirDingusCharacter>(hitActor))
	{
		// if character is not alive
		if (!Character->GetAlive())
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
	if (GetOwner()->ActorHasTag("Player"))
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

	if (AController* OwningController = GetOwner()->GetInstigatorController())
	{
		// deal damage to hit actors
		UClass* DamageTypeClass = UDamageType::StaticClass();
		float dmgDealt = UGameplayStatics::ApplyDamage(
			hitActor,		// DamagedActor - Actor that will be damaged.
			50,				// BaseDamage - The base damage to apply.
			OwningController,		// EventInstigator - Controller that was responsible for causing this damage (e.g. player who swung the weapon)
			GetOwner(),			// DamageCauser - Actor that actually caused the damage (e.g. the grenade that exploded)
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
	}
	else
	{
		/// Debug Message
		if (bDebugLog && GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.f,
				FColor::Red,
				FString(TEXT("Owning Controller is Invalid"))
			);
		}
		return false;
	}
}

// -- Draw a line trace to track a weapon's movement and detect hit events
AActor* UMeleeComponent::DrawWeaponArc(bool bDrawDebug, bool bDebugLog)
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
		QueryParams.AddIgnoredActor(GetOwner());

		// perform line trace
		GetWorld()->LineTraceSingleByChannel(Hit, traceStart, traceEnd, ECollisionChannel::ECC_Camera, QueryParams);

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