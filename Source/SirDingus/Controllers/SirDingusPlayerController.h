// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SirDingus/Interface/ActionStateInterface.h"
#include "GameFramework/PlayerController.h"
#include "InputAction.h"
#include "SirDingusPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class SIRDINGUS_API ASirDingusPlayerController : public APlayerController, public IActionStateInterface
{
	GENERATED_BODY()


public:
	UFUNCTION(Client, Reliable)
	void SetHUDHealth(float Health, float MaxHealth);

	virtual void UpdateActionState(const EActionState State) override;

protected:

	/** Input Functions **/

	/// -- Dodging
	void Dodge(const FInputActionValue& Value);

	/// -- Movement
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
	void TestSomething(const FInputActionValue& Value);


	// APlayerController interface
	virtual void SetupInputComponent() override;

	// To add mapping context
	virtual void BeginPlay() override;

private:
	// HUD & Gamemode Pointers
	class ASirDingusHUD* PlayerHUD;
	class ASirDingusGameMode* CurrentGameMode;

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

	/// -- Restart Game
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* RestartGameAction;

	/// -- Quit Game
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* QuitGameAction;

	/// -- Extra Test Action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* TestAction;

	/** Debug Booleans **/
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Debug|SirDingusPlayerController", meta = (AllowPrivateAccess = "true"))
	bool bDebugMessages;
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Debug|SirDingusPlayerController", meta = (AllowPrivateAccess = "true"))
	bool bDebugLogs;
};
