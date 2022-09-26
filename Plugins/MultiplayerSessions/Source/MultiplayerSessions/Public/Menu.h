#pragma once

#include "CoreMinimal.h"
#include "MultiplayerSessionsSubsystem.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Menu.generated.h"

class UButton;
class UMultiplayerSessionsSubsystem;

/**
 * 
 */
UCLASS()
class MULTIPLAYERSESSIONS_API UMenu : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	UButton* HostButton;

	UPROPERTY(meta = (BindWidget))
	UButton* JoinButton;

	UPROPERTY(meta = (BindWidget))
	UButton* QuitButton;
	
protected:
public:
private:
	UFUNCTION()
	void OnHostButtonPressed();

	UFUNCTION()
	void OnJoinButtonPressed();

	UFUNCTION()
	void OnQuitButtonPressed();
	
	UFUNCTION()
	void MenuTearDown();

protected:
	/*
	 * Callbacks for the custom delegate on the MultiplayerSubsystem
	 */
	UFUNCTION()
	virtual void OnCreateSession(bool bWasSuccessful);

	virtual void OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful);

	virtual void OnJoinSession(EOnJoinSessionCompleteResult::Type Result);

	UFUNCTION()
	virtual void OnDestroySession(bool bWasSuccessful);

	UFUNCTION()
	virtual void OnStartSession(bool bWasSuccessful);

public:
	UFUNCTION(BlueprintCallable)
	void MenuSetup(int32 NumberOfPublicConnections = 4, FString TypeOfMatch = "FreeForAll", FString LobbyPath = "/Game/Maps/Start/Lobby");

private:
	// The subsystem designed to handle all online session functionality
	UMultiplayerSessionsSubsystem* MultiplayerSessionsSubsystem;

	FSessionData GameSessionData {4, "FreeForAll"};

	FString PathToLobbyMap {""};

protected:
	virtual bool Initialize() override;

	virtual void OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld);
};
