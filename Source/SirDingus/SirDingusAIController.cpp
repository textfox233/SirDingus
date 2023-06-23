// Fill out your copyright notice in the Description page of Project Settings.


#include "SirDingusAIController.h"

#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"

void ASirDingusAIController::BeginPlay()
{
	Super::BeginPlay();
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	if (AIBehavior)
	{
		RunBehaviorTree(AIBehavior);
		
		// initialise blackboard values				// key					// value
		GetBlackboardComponent()->SetValueAsVector	(TEXT("StartLocation"),	GetPawn()->GetActorLocation());
		GetBlackboardComponent()->SetValueAsBool	(TEXT("IsAlive"),		true);
	}

	// Debug Msg
	else if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			15.f,
			FColor::Yellow,
			FString(TEXT("Behavior Tree Invalid"))
		);
	}
}

void ASirDingusAIController::SetIsAlive(bool bValue)
{
	/// Debug Msg
	//if (GEngine)
	//{
	//	GEngine->AddOnScreenDebugMessage(
	//		-1,
	//		2.f,
	//		FColor::Yellow,
	//		FString::Printf(TEXT("ASirDingusAIController::SetIsAlive(%s)"), (bValue ? TEXT("true") : TEXT("false")))
	//	);
	//}

	// clear focus to stop rotation
	ClearFocus(EAIFocusPriority::Gameplay);
	// inform the blackboard of the AI's death
	GetBlackboardComponent()->SetValueAsBool(TEXT("IsAlive"), bValue);
}

void ASirDingusAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	//APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	// get target player
	UObject* TargetPlayer = GetBlackboardComponent()->GetValueAsObject(TEXT("TargetPlayer"));

	// if target player is not set
	if (!TargetPlayer)
	{
		// Check if you can see ANY player
		if (CanSeeAnyPlayer())
		{
			/// Select a new target
			if (APawn* TargetPawn = ChooseTarget())
			{
				// set focus
				// set blackboard value
				GetBlackboardComponent()->SetValueAsObject(TEXT("TargetPlayer"), TargetPawn);
			}
		}
		else
		{
			// cannot see player
			GetBlackboardComponent()->ClearValue(TEXT("PlayerLocation"));
		}
	}
	
	
	if (TargetPlayer != nullptr)
	{
		if (APawn* PlayerPawn = Cast<APawn>(TargetPlayer))
		{
			// if target player is set AND within line of sight
			// Observe Player Position
			if (LineOfSightTo(PlayerPawn))
			{
				// set blackboard values
				GetBlackboardComponent()->SetValueAsVector(TEXT("PlayerLocation"), PlayerPawn->GetActorLocation());
				GetBlackboardComponent()->SetValueAsVector(TEXT("LastKnownPlayerLocation"), PlayerPawn->GetActorLocation());
			}
			else
			{
				LostSightOfTarget();
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("ASirDingusAIController::Tick() | TargetPlayer cannot cast to APawn*"));
		}
	}
	else
	{
		//UE_LOG(LogTemp, Warning, TEXT("ASirDingusAIController::Tick() | TargetPlayer is nullptr"));
	}

}


APawn* ASirDingusAIController::ChooseTarget()
{
	// update number of players
	_numPlayers = GetWorld()->GetNumPlayerControllers();

	if (_numPlayers)
	{
		//APawn* TargetPawn;

		// more than 1 player
		if (_numPlayers > 1)
		{
			// Random Player
			//TargetPawn = GetRandomPlayerPawn();

			// Closest Player
			return GetClosestPlayerPawn();
			
			//TargetPawn = GetClosestPlayerPawn();
			//
			//if (TargetPawn)
			//{
			//	// set target in blackboard
			//	GetBlackboardComponent()->SetValueAsObject(TEXT("TargetPlayer"), TargetPawn);
			//
			//	// focus on them
			//	SetFocus(TargetPawn);
			//	return true;
			//}
			//else
			//{
			//	UE_LOG(LogTemp, Warning, TEXT("TargetPawn is nullptr"));
			//}
		}

		// only 1 player, pick that one
		else
		{
			return GetWorld()->GetFirstPlayerController()->GetPawn();
			
			//TargetPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
			//
			//// set target in blackboard
			//GetBlackboardComponent()->SetValueAsObject(TEXT("TargetPlayer"), TargetPawn);
			//
			//// focus on them
			//SetFocus(TargetPawn);
			//return true;
		}

		//UE_LOG(LogTemp, Warning, TEXT("set %s focus to %s"), *OwnerComp.GetAIOwner()->GetName(), *TargetPawn->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UBTTask_ChooseTarget::ExecuteTask() | ERROR | GetWorld()->GetNumPlayerControllers() returns False"));
	}
	return nullptr;
}

bool ASirDingusAIController::CanSeeAnyPlayer()
{
	APawn* PlayerPawn;
	_numPlayers = GetWorld()->GetNumPlayerControllers();
	// for every player
	for (FConstPlayerControllerIterator i = GetWorld()->GetPlayerControllerIterator(); i.GetIndex() < _numPlayers; i++)
	{
		/// Debug Logs
		//UE_LOG(LogTemp, Warning, TEXT("Index is [%d]"), i.GetIndex());
		//UE_LOG(LogTemp, Warning, TEXT("Player [%d]: %s"), i.GetIndex(), *i->Get()->GetName());

		/// Get Pawn
		PlayerPawn = i->Get()->GetPawn();
		if (PlayerPawn)
		{
			// Can see this player?
			if (LineOfSightTo(PlayerPawn))
			{
				UE_LOG(LogTemp, Warning, TEXT("%s is visible"), *PlayerPawn->GetName());
				return true;
			}
		}
		else
		{
			//UE_LOG(LogTemp, Warning, TEXT("ASirDingusAIController::CanSeeAnyPlayer() | PlayerPawn is nullptr"));
		}
	}
	
	//UE_LOG(LogTemp, Warning, TEXT("No players visible"));

	// Cannot see any player
	return false;
}


APawn* ASirDingusAIController::GetPlayerPawnByIndex(int Index)
{
	FConstPlayerControllerIterator i = GetWorld()->GetPlayerControllerIterator();

	// loop until the iterator reaches the index - actual loop is empty except for some debug logs
	for (; i.GetIndex() < Index; i++)
	{
		// Debug Here
		//UE_LOG(LogTemp, Warning, TEXT("Index is [%d]"), i.GetIndex());
		//UE_LOG(LogTemp, Warning, TEXT("Player [%d]: %s"), i.GetIndex(), *i->Get()->GetName());
	}

	/// Debug Logs
	//UE_LOG(LogTemp, Warning, TEXT("targetIndex is [%d]"), Index);
	//UE_LOG(LogTemp, Warning, TEXT("Final Index is [%d]"), i.GetIndex());
	//UE_LOG(LogTemp, Warning, TEXT("Player [%d]: %s"), i.GetIndex(), *i->Get()->GetName());

	// return player pawn at the desired index
	return i->Get()->GetPawn();
}

APawn* ASirDingusAIController::GetClosestPlayerPawn()
{
	if (APawn* OwnerPawn = GetPawn())
	{
		float ShortestDistance = NULL;
		APawn* ClosestPlayerPawn = nullptr;

		FVector PlayerLocation;
		APawn* PlayerPawn;

		// Get owner location
		FVector OwnerLocation = OwnerPawn->GetActorLocation();

		// for every player
		for (FConstPlayerControllerIterator i = GetWorld()->GetPlayerControllerIterator(); i.GetIndex() < _numPlayers; i++)
		{
			/// Debug Logs
			//UE_LOG(LogTemp, Warning, TEXT("Index is [%d]"), i.GetIndex());
			//UE_LOG(LogTemp, Warning, TEXT("Player [%d]: %s"), i.GetIndex(), *i->Get()->GetName());

			/// Get Pawn
			PlayerPawn = i->Get()->GetPawn();
			if (PlayerPawn)
			{
				// Get Location
				PlayerLocation = PlayerPawn->GetActorLocation();

				// Get Distance (between owner and player)
				float CurrentDistance = FVector::Dist(OwnerLocation, PlayerLocation);

				/// Compare Distance to Previously Recorded
				// if shortest is NULL, recent is first recorded distance
				// otherwise if recent is smaller, it is the new shortest
				if (ShortestDistance == NULL || CurrentDistance < ShortestDistance)
				{
					// only save if AI can see them
					if (LineOfSightTo(PlayerPawn))
					{
						// save distance for comparison 
						ShortestDistance = CurrentDistance;

						// save player pawn for return value
						ClosestPlayerPawn = PlayerPawn;
					}
				}
				// if recent is larger than shortest do nothing; move on
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("UBTTask_ChooseTarget::GetClosestPlayerPawn() | PlayerPawn is nullptr"));
			}
		}
		return ClosestPlayerPawn;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UBTTask_ChooseTarget::GetClosestPlayerPawn() | OwnerPawn is nullptr"));
	}

	/// Debug Logs
	//UE_LOG(LogTemp, Warning, TEXT("targetIndex is [%d]"), Index);
	//UE_LOG(LogTemp, Warning, TEXT("Final Index is [%d]"), i.GetIndex());
	//UE_LOG(LogTemp, Warning, TEXT("Player [%d]: %s"), i.GetIndex(), *i->Get()->GetName());

	// if above fails, return nullptr
	return nullptr;
}

void ASirDingusAIController::LostSightOfTarget()
{
	// clear focus
	ClearFocus(EAIFocusPriority::Gameplay);

	// clear blackboard values
	GetBlackboardComponent()->ClearValue(TEXT("PlayerLocation"));
	//GetBlackboardComponent()->ClearValue(TEXT("TargetPlayer"));
}

APawn* ASirDingusAIController::GetRandomPlayerPawn()
{
	// select a random player
	int randomPlayerIndex = GetRandomPlayerIndex();
	UE_LOG(LogTemp, Warning, TEXT("GetRandomPlayerIndex() returned %d with %d players"), randomPlayerIndex, _numPlayers);

	// return that random player
	return GetPlayerPawnByIndex(randomPlayerIndex);

	//return nullptr;
}

int ASirDingusAIController::GetRandomPlayerIndex()
{
	// select a random player
	// get a random number between 1 and the total number of players
	int range = GetWorld()->GetNumPlayerControllers();
	int num = rand() % range;

	///Debug logs
	//UE_LOG(LogTemp, Warning, TEXT("Random number between %d and %d is %d"), 1, numPlayers, num);
	//UE_LOG(LogTemp, Warning, TEXT("Number of player controllers is %d"), GetWorld()->GetNumPlayerControllers());

	return num;
}