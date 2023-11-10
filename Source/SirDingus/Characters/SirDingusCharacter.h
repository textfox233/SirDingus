// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SirDingus/Interface/HitInterface.h"
//#include "CharacterTypes.h"
#include "InputActionValue.h"
#include "SirDingusCharacter.generated.h"

UCLASS(config=Game)
class ASirDingusCharacter : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

	// Gamemode Pointer
	class ASirDingusGameMode* CurrentGameMode;

	/** Camera Stuff **/
	// -- Camera Boom (positions the camera behind the character)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	// -- Follow Camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	/** Components **/
	// -- Health
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health", meta = (AllowPrivateAccess = "true"))
		class UHealthComponent* HealthComponent;
	// -- Melee
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Melee", meta = (AllowPrivateAccess = "true"))
		class UMeleeComponent* MeleeComponent;

	/** Debug Area **/
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Debug|SirDingusCharacter", meta = (AllowPrivateAccess = "true"))
		bool bDrawDebug = false;
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Debug|SirDingusCharacter", meta = (AllowPrivateAccess = "true"))
		bool bDebugMessages = false;
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Debug|SirDingusCharacter", meta = (AllowPrivateAccess = "true"))
		bool bDebugLogs = false;
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Debug|SirDingusCharacter", meta = (AllowPrivateAccess = "true"))
		bool bDebugStates = false;

	//UFUNCTION(BlueprintCallable)
	//	void PrintActionState();
		//void TestFlinchAnimation(FName Section);
		void TestMontageAnimation(UAnimMontage* Montage, FName Section = NAME_None);

public:
	ASirDingusCharacter();

	// used when the player controller wants to test something that requires private access
	void TestPrivateFunction();

	/** Character Death **/
	virtual void CharacterDeath();

	// -- Disable Capsule Collision (code must be replicated)
	UFUNCTION(Server, Reliable)
	void DisableCapsuleCollisionServer(UPrimitiveComponent* Capsule);
	UFUNCTION(NetMulticast, Reliable)
	void DisableCapsuleCollisionMulticast(UPrimitiveComponent* Capsule);

	/** Replication **/
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** Getting hit by an Attack or Damaging Effect **/
	virtual void GetHit(const FVector& ImpactPoint) override;

	/** Playing Animations **/
	UFUNCTION(Server, Reliable, BlueprintCallable)
		void PlayAnimMontageServer(UAnimMontage* AnimMontage, const FName& StartSectionName = NAME_None);
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
		void PlayAnimMontageMulticast(UAnimMontage* AnimMontage, const FName& StartSectionName = NAME_None);
	
	/** Animation Montages **/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess))
		class UAnimMontage* FlinchMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess))
		class UAnimMontage* DeathMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess))
		class UAnimMontage* DodgeMontage;


	/** Input Functions **/
	// -- Dodging
	void Dodge();

	// -- Attacking
	void Attack();

protected:
	
	// To add mapping context
	virtual void BeginPlay();

	
	/** Debugging Functions **/
	// -- Return NetRole as an FString
	FString GetNetRole();

private:
	/** Is Character Alive **/
	UPROPERTY(Replicated)
	bool bAlive = true;

	//ECharacterState CharacterState = ECharacterState::ECS_Unequipped;
	//UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	//EActionState ActionState = EActionState::EAS_Unoccupied;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	FORCEINLINE void SetAlive(bool isAlive) { bAlive = isAlive; }
	FORCEINLINE bool GetAlive() const { return bAlive; }

	//FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }
	//FORCEINLINE EActionState GetActionState() const { return ActionState; }

	//FORCEINLINE bool CanMove() const { return ActionState == EActionState::EAS_Unoccupied; }
};

