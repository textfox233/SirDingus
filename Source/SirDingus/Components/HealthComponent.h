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

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void TakeDamage(float Damage);

protected:
	
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	// Health Values
	UPROPERTY(EditAnywhere, Category = "Health Component")
	float MaxHealth = 100;
	UPROPERTY(ReplicatedUsing = OnRep_Health, VisibleAnywhere, Category = "Health Component")
	float Health;

	// -- Debug booleans
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Debug|HealthComponent", meta = (AllowPrivateAccess = "true"))
		bool bDebugMessages;
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Debug|HealthComponent", meta = (AllowPrivateAccess = "true"))
		bool bDebugLog;

	UFUNCTION()
	void OnRep_Health();

	void UpdateHUDHealth();

	// References
	APawn* OwnerPawn;
	class ASirDingusGameMode* CurrentGameMode;
	class ASirDingusPlayerController* PlayerController;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
