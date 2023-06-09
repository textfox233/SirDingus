// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthComponent.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


float UHealthComponent::TakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	return 0.0f;
}

// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	// initialise health
	Health = MaxHealth;

	AActor* Owner = GetOwner();
	if (Owner)
	{
		// bind DamageTaken callback to OnTakeAnyDamage delegate
		GetOwner()->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::DamageTaken);
	}
}

void UHealthComponent::DamageTaken(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* Instigator, AActor* DamageCauser)
{
	UE_LOG(LogTemp, Warning, TEXT("%s Damaged"), *DamagedActor->GetName());
	if (Damage <= 0.f) return; // no dmg

	// Apply damage
	Health -= Damage;
	UE_LOG(LogTemp, Warning, TEXT("Health: %f"), Health);
	
	// If health goes below zero, the actor has died
	if (Health <= 0.f) 
	{
		//ToonTanksGameMode->ActorDied(DamagedActor);
		UE_LOG(LogTemp, Warning, TEXT("%s has Died"), *DamagedActor->GetName());
	}
	//// Debug Msg
	//if (GEngine)
	//{
	//	GEngine->AddOnScreenDebugMessage(
	//		-1,
	//		2.f,
	//		FColor::Red,
	//		FString(TEXT("UHealthComponent::DamageTaken()"))
	//	);
	//}

}

// Called every frame
void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

