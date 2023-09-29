// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputAction.h"
//#include "InputActionValue.h"
#include "SirDingusPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class SIRDINGUS_API ASirDingusPlayerController : public APlayerController
{
	GENERATED_BODY()

	// -- Debug booleans
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = Debug, meta = (AllowPrivateAccess = "true"))
		bool bDebugMessages;
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = Debug, meta = (AllowPrivateAccess = "true"))
		bool bDebugLogs;

public:
	UFUNCTION(Client, Reliable)
	void SetHUDHealth(float Health, float MaxHealth);

	/** Getter for Character */
	//FORCEINLINE ASirDingusCharacter* GetSDCharacter() { return Cast<ASirDingusCharacter>(GetCharacter()); }
	/** Getter for Character */
	//FORCEINLINE ACharacter* GetCharacter() const { return Character; }

	// -- Attacking (currently public so that AI can find it, may want to change this)
	//void Attack(const FInputActionValue& Value);

protected:

	/** Input Functions **/

	/// -- Dodging
	void Dodge(const FInputActionValue& Value);
	//void StopDodging();

	/// -- Movement
	//UFUNCTION()
	void Move(const FInputActionValue& Value);

	/// -- Looking
	void Look(const FInputActionValue& Value);

	/// -- Attacking
	void Attack(const FInputActionValue& Value);

	/// -- Restart Game
	UFUNCTION(Server, Reliable)
	void RestartGame(const FInputActionValue& Value);

	/// -- Quit Game
	void QuitGame(const FInputActionValue& Value);

	/// -- Extra Test Action
	////UFUNCTION(BlueprintImplementableEvent, meta = (AllowPrivateAccess))
	void TestSomething(const FInputActionValue& Value);


	// APlayerController interface
	virtual void SetupInputComponent() override;

	// To add mapping context
	virtual void BeginPlay() override;

private:
	// HUD Pointer
	class ASirDingusHUD* PlayerHUD;
	// Gamemode Pointer
	class ASirDingusGameMode* CurrentGameMode;
	// Character Pointer (specific to my class)
	//class ASirDingusCharacter* SDCharacter;

	/** MappingContext **/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputMappingContext* DefaultMappingContext;

	/** Input Actions **/
	/// -- Dodge
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* DodgeAction;

	/// -- Move
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		UInputAction* MoveAction;

	/// -- Look
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		UInputAction* LookAction;

	/// -- Attack
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* AttackAction;
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = MyAnimationInfo, meta = (AllowPrivateAccess = "true"))
	//bool bIsAttacking;

	/// -- Restart Game
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* RestartGameAction;

	/// -- Quit Game
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* QuitGameAction;

	/// -- Extra Test Action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* TestAction;
};
