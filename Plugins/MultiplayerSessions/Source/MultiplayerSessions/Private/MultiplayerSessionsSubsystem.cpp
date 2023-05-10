// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"

UMultiplayerSessionsSubsystem::UMultiplayerSessionsSubsystem():
	CreateSessionCompleteDelegate	(FOnCreateSessionCompleteDelegate::CreateUObject	(this, &ThisClass::OnCreateSessionComplete)),
	FindSessionsCompleteDelegate	(FOnFindSessionsCompleteDelegate::CreateUObject		(this, &ThisClass::OnFindSessionsComplete)),
	JoinSessionCompleteDelegate		(FOnJoinSessionCompleteDelegate::CreateUObject		(this, &ThisClass::OnJoinSessionComplete)),
	DestroySessionCompleteDelegate	(FOnDestroySessionCompleteDelegate::CreateUObject	(this, &ThisClass::OnDestroySessionComplete)),
	StartSessionCompleteDelegate	(FOnStartSessionCompleteDelegate::CreateUObject		(this, &ThisClass::OnStartSessionComplete))
{
	// access the subsystem to get the interface from it
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();

	if (Subsystem) // if valid
	{
		// get the interface via the subsystem
		SessionInterface = Subsystem->GetSessionInterface();
	}
}

void UMultiplayerSessionsSubsystem::CreateSession(int32 NumPublicConnections, FString MatchType)
{
	if (!SessionInterface)
	{
		return; // early return if invalid session interface
	}

	// check if existing session exists -> destroy if true
	auto ExistingSession = SessionInterface->GetNamedSession(NAME_GameSession);
	if (ExistingSession != nullptr)
	{
		// also store info for new session to be created
		bCreateSessionOnDestroy = true;
		LastNumPublicConnections = NumPublicConnections;
		LastMatchType = MatchType;

		DestroySession();
	}

	// store the delegate in an FDelegateHandle so we can later remove it from the delegate list
	CreateSessionCompleteDelegateHandle = SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegate);

	// configure session settings
	LastSessionSettings = MakeShareable(new FOnlineSessionSettings());
	LastSessionSettings->bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL" ? true : false; // can be either NULL or Steam (LAN or Online)
	LastSessionSettings->NumPublicConnections = NumPublicConnections;
	LastSessionSettings->bAllowJoinInProgress = true;
	LastSessionSettings->bAllowJoinViaPresence = true;
	LastSessionSettings->bShouldAdvertise = true;
	LastSessionSettings->bUsesPresence	= true;

	LastSessionSettings->bUseLobbiesIfAvailable = true;
	LastSessionSettings->BuildUniqueId = 1;

	LastSessionSettings->Set(FName("MatchType"), MatchType, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (!SessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *LastSessionSettings))
	{
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);

		// Broadcast custom delegate
		MultiplayerOnCreateSessionComplete.Broadcast(false);
	}

}

void UMultiplayerSessionsSubsystem::FindSessions(int32 MaxSearchResults)
{
	if (!SessionInterface.IsValid())
	{
		return;
	}

	// add delegate to online session interface delegate list
	FindSessionsCompleteDelegateHandle = SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegate);

	LastSessionSearch = MakeShareable(new FOnlineSessionSearch());
	LastSessionSearch->MaxSearchResults = MaxSearchResults;
	LastSessionSearch->bIsLanQuery = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL" ? true : false; // can be either NULL or Steam (LAN or Online)
	LastSessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (!SessionInterface->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(), LastSessionSearch.ToSharedRef()))
	// IF search failed
	{
		// clear delegate
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegateHandle);

		// broadcast menu delegate - pass false
		MultiplayerOnFindSessionsComplete.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
	}
	// ELSE OnFindSessionsComplete will fire successfully
}

void UMultiplayerSessionsSubsystem::JoinSession(const FOnlineSessionSearchResult& SessionResult)
{
	if(!SessionInterface.IsValid())
	// if invalid
	{
		// return and broadcast failure to the menu
		MultiplayerOnJoinSessionComplete.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
		return;
	}

	// add to delegate list
	JoinSessionCompleteDelegateHandle = SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegate);

	// join session
	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (!SessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, SessionResult))
	// IF join failed
	{
		// clear delegate
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);

		// broadcast menu delegate - pass UnknownError
		MultiplayerOnJoinSessionComplete.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
	}
}

void UMultiplayerSessionsSubsystem::DestroySession()
{
	if (!SessionInterface.IsValid())
	// IF destroy fails
	{
		// broadcast menu delegate - pass false
		MultiplayerOnDestroySessionComplete.Broadcast(false);
		return;
	}
	
	// add delegate to session list
	SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegate);

	if (!SessionInterface->DestroySession(NAME_GameSession))
	// IF destroy fails
	{
		// Clear delegate
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegateHandle);
	
		// broadcast menu delegate - pass false
		MultiplayerOnDestroySessionComplete.Broadcast(false);
	}
}

void UMultiplayerSessionsSubsystem::StartSession()
{
}

void UMultiplayerSessionsSubsystem::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (SessionInterface)
	{
		// remove delegate from delegate list
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);
	}

	// Broadcast custom delegate
	MultiplayerOnCreateSessionComplete.Broadcast(bWasSuccessful);
}

void UMultiplayerSessionsSubsystem::OnFindSessionsComplete(bool bWasSuccessful)
{
	if (SessionInterface)
	{
		// clear delegate
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegateHandle);
	}

	// in case results array is empty
	if (LastSessionSearch->SearchResults.Num() <= 0)
	{
		// broadcast menu delegate - pass false
		MultiplayerOnFindSessionsComplete.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
		return;
	}

	// broadcast menu delegate - pass results
	MultiplayerOnFindSessionsComplete.Broadcast(LastSessionSearch->SearchResults, bWasSuccessful);
}

void UMultiplayerSessionsSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (SessionInterface)
	{
		// clear delegate
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);
	}

	// broadcast menu delegate - pass result
	MultiplayerOnJoinSessionComplete.Broadcast(Result);
}

void UMultiplayerSessionsSubsystem::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (SessionInterface)
	{
		// Clear delegate
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegateHandle);
	}
	if (bWasSuccessful && bCreateSessionOnDestroy)
	// IF want to make a new session
	{
		// switch boolean
		bCreateSessionOnDestroy = false;

		// create sesion
		CreateSession(LastNumPublicConnections, LastMatchType);
	}

	// broadcast menu delegate - successful
	MultiplayerOnDestroySessionComplete.Broadcast(bWasSuccessful);
}

void UMultiplayerSessionsSubsystem::OnStartSessionComplete(FName SessionName, bool bWasSuccessful)
{
}
