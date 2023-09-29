// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CharacterTypes.h"
#include "InputActionValue.h"
#include "SirDingusCharacter.generated.h"

UCLASS(config=Game)
class ASirDingusCharacter : public ACharacter
{
	GENERATED_BODY()

	// Gamemode Pointer
	class ASirDingusGameMode* CurrentGameMode;

	/** Camera Stuff **/
	// -- Camera Boom (positions the camera behind the character)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	// -- Follow Camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	/** Components **/
	// -- Health
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Health, meta = (AllowPrivateAccess = "true"))
		class UHealthComponent* HealthComponent;
	// -- Melee
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Melee, meta = (AllowPrivateAccess = "true"))
		class UMeleeComponent* MeleeComponent;

	/** Debug Booleans **/
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = Debug, meta = (AllowPrivateAccess = "true"))
		bool bDebugMessages;
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = Debug, meta = (AllowPrivateAccess = "true"))
		bool bDebugLogs;


public:
	ASirDingusCharacter();

	/** Character Death **/
	virtual void CharacterDeath();

	// -- Disable Capsule Collision (code must be replicated)
	UFUNCTION(Server, Reliable)
	void DisableCapsuleCollisionServer(UPrimitiveComponent* Capsule);
	UFUNCTION(NetMulticast, Reliable)
	void DisableCapsuleCollisionMulticast(UPrimitiveComponent* Capsule);

	/** Replication **/
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** Taking Damage **/
	// - tried at first to bind a new function to OnTakeAnyDamage but ran into problems, decided instead to override AActor::TakeDamage()
	float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;


	//** Playing Animations **/
	UFUNCTION(Server, Reliable, BlueprintCallable)
		void PlayAnimMontageServer(UAnimMontage* AnimMontage, FName StartSectionName = NAME_None);
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
		void PlayAnimMontageMulticast(UAnimMontage* AnimMontage, FName StartSectionName = NAME_None);
	
	UFUNCTION(BlueprintCallable)
		void ResetActionState();
	UFUNCTION(BlueprintCallable)
		void Interrupted();

	/** Animation Montages **/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess))
		class UAnimMontage* BasicAttackMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess))
		class UAnimMontage* FlinchMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess))
		class UAnimMontage* DeathMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess))
		class UAnimMontage* DodgeMontage;


	/** Input Functions **/

	// -- Dodging
	void Dodge();
	//void StopDodging();

	// -- Movement
	void Move(FVector2D MoveVector);

	// -- Looking
	void Look(const FInputActionValue& Value);

	// -- Attacking
	void Attack();

	// -- Restart Game
	UFUNCTION(Server, Reliable)
		void RestartGame(const FInputActionValue& Value);

	// -- Quit Game
	void QuitGame(const FInputActionValue& Value);

	// -- Extra Test Action
	//UFUNCTION(BlueprintImplementableEvent, meta = (AllowPrivateAccess))
	void TestSomething(const FInputActionValue& Value);


protected:

	///** Input Functions **/
	//
	//// -- Dodging
	//void Dodge(const FInputActionValue& Value);
	//void StopDodging();
	//
	//// -- Movement
	//void Move(const FInputActionValue& Value);
	//
	//// -- Looking
	//void Look(const FInputActionValue& Value);
	//		
	////// -- Attacking
	////void Attack(const FInputActionValue& Value);
	//
	//// -- Restart Game
	//UFUNCTION(Server, Reliable)
	//void RestartGame(const FInputActionValue& Value);
	//
	//// -- Quit Game
	//void QuitGame(const FInputActionValue& Value);
	//
	//// -- Extra Test Action
	////UFUNCTION(BlueprintImplementableEvent, meta = (AllowPrivateAccess))
	//void TestSomething(const FInputActionValue& Value);
	//
	//
	//// APawn interface
	//virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();

	
	/** Debugging Functions **/

	// -- Return NetRole as an FString
	FString GetNetRole();

	///** Melee Animation Timer (Refactored into melee component) **/
	//
	//// -- Timer Handle
	//FTimerHandle MeleeTraceHandle;
	//
	//// -- Start
	//UFUNCTION(BlueprintCallable)
	//void MeleeTraceStart();
	//
	//// -- InProgress
	//UFUNCTION()
	//void MeleeTraceInProgress();
	//
	//// -- Stop
	//UFUNCTION(BlueprintCallable)
	//void MeleeTraceEnd();


	/** Melee Functions (Refactored into melee component) **/
	//
	//// -- Process Melee Hits
	//UFUNCTION(BlueprintCallable)
	//bool ProcessMeleeHit(AActor* hitActor, bool bDebugLog = false);
	//
	//// -- Perform Weapon Arc via Line Traces
	//UFUNCTION(BlueprintCallable)
	//AActor* DrawWeaponArc(bool bDrawDebug = false, bool bDebugLog = false);


private:
	/** Is Character Alive **/
	UPROPERTY(Replicated)
	bool bAlive = true;

	ECharacterState CharacterState = ECharacterState::ECS_Unequipped;
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EActionState ActionState = EActionState::EAS_Unoccupied;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	FORCEINLINE void SetAlive(bool isAlive) { bAlive = isAlive; }
	FORCEINLINE bool GetAlive() const { return bAlive; }

	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }
};

