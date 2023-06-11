// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SIRDINGUS_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHealthComponent();

	// Getter - Health
	UFUNCTION(BlueprintCallable)
	float GetHealth() { return Health; };

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	// Health Values
	UPROPERTY(EditAnywhere, Category = Health)
	float MaxHealth = 100;
	float Health;

	UFUNCTION()
	void DamageTaken(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* Instigator, AActor* DamageCauser);

	class ASirDingusGameMode* SirDingusGameMode;
public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	
};
