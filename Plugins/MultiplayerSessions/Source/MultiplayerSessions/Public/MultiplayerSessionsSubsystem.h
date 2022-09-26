#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "MultiplayerSessionsSubsystem.generated.h"

/*
 * Custom delegates to bind callbacks to
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMultiplayerOnCreateSessionComplete, bool, bWasSuccessful);
DECLARE_MULTICAST_DELEGATE_TwoParams(FMultiplayerOnFindSessionComplete, const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful);
DECLARE_MULTICAST_DELEGATE_OneParam(FMultiplayerOnJoinSessionComplete, EOnJoinSessionCompleteResult::Type Result);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMultiplayerOnDestroySessionComplete, bool, bWasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMultiplayerOnStartSessionComplete, bool, bWasSuccessful);

USTRUCT()
struct FSessionData
{
	GENERATED_BODY()

	int32 NumPublicConnections;

	FString MatchType;
};

/**
 * Players must have Steam running in background and have the same Steam region in settings
 */
UCLASS()
class MULTIPLAYERSESSIONS_API UMultiplayerSessionsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	const FName LanMatchName = "NULL";
	
	IOnlineSessionPtr SessionInterface;

	TSharedPtr<FOnlineSessionSettings> LastSessionSettings;

	TSharedPtr<FOnlineSessionSearch> LastSessionSearch;
	
	/*
	 * To add the Online Session Interface delegate list.
	 * We'll bind our MultiplayerSessionSubsystem internal callbacks to these.
	 */
	FOnCreateSessionCompleteDelegate CreateSessionCompleteDelegate;

	FOnFindSessionsCompleteDelegate FindSessionsCompleteDelegate;

	FOnJoinSessionCompleteDelegate JoinSessionCompleteDelegate;

	FOnDestroySessionCompleteDelegate DestroySessionCompleteDelegate;
	
	FOnStartSessionCompleteDelegate StartSessionCompleteDelegate;
	
	FDelegateHandle CreateSessionCompleteDelegateHandle;

	FDelegateHandle FindSessionsCompleteDelegateHandle;
	
	FDelegateHandle JoinSessionCompleteDelegateHandle;

	FDelegateHandle DestroySessionCompleteDelegateHandle;

	FDelegateHandle StartSessionCompleteDelegateHandle;

	bool bCreateSessionOnDestroy {false};
	
protected:
public:

	/*
	 * Custom delegate to bind callbacks to
	 */
	FMultiplayerOnCreateSessionComplete MultiplayerOnCreateSessionComplete;

	FMultiplayerOnFindSessionComplete MultiplayerOnFindSessionComplete;

	FMultiplayerOnJoinSessionComplete MultiplayerOnJoinSessionComplete;

	FMultiplayerOnDestroySessionComplete MultiplayerOnDestroySessionComplete;

	FMultiplayerOnStartSessionComplete MultiplayerOnStartSessionComplete;

	FSessionData LastSessionData;
	
protected:

	/*
	 * Internal callbacks for the delegates we'll add to the Online Session Interface delegate list.
	 * These don't need to be call outside of this class.
	 */
	void OnCreateSessionComplete(FName Session, bool bWasSuccessful);

	void OnFindSessionComplete(bool bWasSuccessful);

	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);

	void OnStartSessionComplete(FName SessionName, bool bWasSuccessful);
	
public:
	
	UMultiplayerSessionsSubsystem();

	/*
	 * To handle session functionality. The menu class will call these
	 */
	void CreateSession(const FSessionData& NewSessionData);

	void FindSessions(int32 MaxSearchResults);

	void JoinSession(const FOnlineSessionSearchResult& SessionResult);

	void DestroySession();

	void StartSession();

	FORCEINLINE const IOnlineSessionPtr& GetOnlineSession() const { return SessionInterface; }
};
