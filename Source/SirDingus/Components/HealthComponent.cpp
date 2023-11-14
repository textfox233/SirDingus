// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthComponent.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"

#include "SirDingus/Modes & States/SirDingusGameMode.h"
#include "SirDingus/Controllers/SirDingusPlayerController.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME(UHealthComponent, Health);
}

// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	if (bDebugMessages && GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			5.f,
			FColor::Green,
			TEXT("UHealthComponent::BeginPlay")
		);
	}

	// initialise health
	Health = MaxHealth;

	// get game mode
	CurrentGameMode = Cast<ASirDingusGameMode>(UGameplayStatics::GetGameMode(this));

	UpdateHUDHealth();
}

void UHealthComponent::OnRep_Health()
{
}

void UHealthComponent::UpdateHUDHealth()
{
	// get controller
	OwnerPawn = OwnerPawn == nullptr ? Cast<APawn>(GetOwner()) : OwnerPawn;
	if (OwnerPawn)
	{
		if (bDebugMessages && GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				5.f,
				FColor::Blue,
				FString::Printf(TEXT("OwnerPawn is %s"), *AActor::GetDebugName(OwnerPawn))
			);
		}

		PlayerController = PlayerController == nullptr ? Cast<ASirDingusPlayerController>(OwnerPawn->GetController()) : PlayerController;

		if (PlayerController)
		{
			if (bDebugMessages && GEngine)
			{
				GEngine->AddOnScreenDebugMessage(
					-1,
					5.f,
					FColor::Green,
					TEXT("SetHUDHealth")
				);
			}

			PlayerController->SetHUDHealth(Health, MaxHealth);
		}
		else if (bDebugMessages && GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				5.f,
				FColor::Red,
				TEXT("SetHUDHealth ERROR - Cannot cast OwnerPawn's Controller to Player Controller")
			);
		}
	}
	/// Error Msg
	else
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			5.f,
			FColor::Red,
			TEXT("SetHUDHealth ERROR - Cannot cast Owner to APawn")
		);
	}
}

//void UHealthComponent::TakeDamage_Implementation(float Damage)
bool UHealthComponent::TakeDamage(float Damage)
{
	if (bDebugMessages && GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			2.f,
			FColor::Red,
			FString(TEXT("UHealthComponent::TakeDamage()"))
		);
	}
	if (bDebugLog && GetOwner()) UE_LOG(LogTemp, Warning, TEXT("%s Damaged"), *GetOwner()->GetName());

	if (Damage <= 0.f) return true; // no dmg

	// Apply damage
	Health -= Damage;
	if (bDebugLog) UE_LOG(LogTemp, Warning, TEXT("Health: %f"), Health);

	// Update HUD
	UpdateHUDHealth();

	// If health goes below zero, the actor has died
	if (Health <= 0.f)
	{
		if (bDebugLog) UE_LOG(LogTemp, Warning, TEXT("%s has dropped below 0 health, informing Character"), *GetOwner()->GetName());

		return false; // inform character death is necessary
	}
	return true;
}

//// Called every frame
//void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
//{
//	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
//
//	// ...
//}

