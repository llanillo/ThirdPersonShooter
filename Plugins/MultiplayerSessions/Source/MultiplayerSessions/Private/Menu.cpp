#include "Menu.h"
#include "Components/Button.h"
#include "OnlineSessionSettings.h"
#include "MultiplayerSessionsSubsystem.h"
#include "Kismet/KismetSystemLibrary.h"

bool UMenu::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}

	if (HostButton)
	{
		HostButton->OnPressed.AddDynamic(this, &ThisClass::OnHostButtonPressed);
	}

	if (JoinButton)
	{
		JoinButton->OnPressed.AddDynamic(this, &ThisClass::OnJoinButtonPressed);
	}

	if(QuitButton)
	{
		QuitButton->OnPressed.AddDynamic(this, &ThisClass::OnQuitButtonPressed);
	}

	return true;
}

void UMenu::OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld)
{
	Super::OnLevelRemovedFromWorld(InLevel, InWorld);
	MenuTearDown();
}

void UMenu::MenuSetup(int32 NumberOfPublicConnections, FString TypeOfMatch, FString LobbyPath)
{
	GameSessionData.NumPublicConnections = NumberOfPublicConnections;
	PathToLobbyMap = FString::Printf(TEXT("%s?listen"), *LobbyPath);
	GameSessionData.MatchType = TypeOfMatch;
	bIsFocusable = true;

	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);

	if (const UWorld* const World = GetWorld())
	{
		if (APlayerController* const PlayerController = World->GetFirstPlayerController())
		{
			FInputModeUIOnly InputUIModeData;
			InputUIModeData.SetWidgetToFocus(TakeWidget());
			// Can move mouse cursor off the screen
			InputUIModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			PlayerController->SetInputMode(InputUIModeData);
			PlayerController->SetShowMouseCursor(true);
		}
	}


	if (const UGameInstance* const GameInstance = GetGameInstance())
	{
		MultiplayerSessionsSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
	}

	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->MultiplayerOnCreateSessionComplete.AddDynamic(
			this, &ThisClass::UMenu::OnCreateSession);
		MultiplayerSessionsSubsystem->MultiplayerOnFindSessionComplete.AddUObject(this, &ThisClass::OnFindSessions);
		MultiplayerSessionsSubsystem->MultiplayerOnJoinSessionComplete.AddUObject(this, &ThisClass::OnJoinSession);
		MultiplayerSessionsSubsystem->MultiplayerOnDestroySessionComplete.
		                              AddDynamic(this, &ThisClass::OnDestroySession);
		MultiplayerSessionsSubsystem->MultiplayerOnStartSessionComplete.AddDynamic(this, &ThisClass::OnStartSession);
	}
}

void UMenu::OnHostButtonPressed()
{
	HostButton->SetIsEnabled(false);

	if (!MultiplayerSessionsSubsystem)
	{
		return;
	}

	MultiplayerSessionsSubsystem->CreateSession(GameSessionData);
}

void UMenu::OnJoinButtonPressed()
{
	JoinButton->SetIsEnabled(false);

	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->FindSessions(1000);
	}
}

void UMenu::OnQuitButtonPressed()
{
	if (const UWorld* const World = GetWorld())
	{
		World->GetFirstPlayerController()->ConsoleCommand("quit");
	}
}

void UMenu::MenuTearDown()
{
	RemoveFromParent();
	if (const UWorld* const World = GetWorld())
	{
		if (APlayerController* const PlayerController = World->GetFirstPlayerController())
		{
			const FInputModeGameOnly InputModeGameData;
			PlayerController->SetInputMode(InputModeGameData);
			PlayerController->SetShowMouseCursor(false);
		}
	}
}

void UMenu::OnCreateSession(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		if (UWorld* const World = GetWorld())
		{
			World->ServerTravel(PathToLobbyMap);
		}
	}

	HostButton->SetIsEnabled(true);
}

void UMenu::OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful)
{
	if (!MultiplayerSessionsSubsystem)
	{
		return;
	}

	for (const FOnlineSessionSearchResult& Result : SessionResults)
	{
		FString SettingsValue;
		Result.Session.SessionSettings.Get(FName("MatchType"), SettingsValue);

		if (SettingsValue == GameSessionData.MatchType)
		{
			MultiplayerSessionsSubsystem->JoinSession(Result);
		}
	}

	if(!bWasSuccessful || SessionResults.Num() == 0)
	{
		JoinButton->SetIsEnabled(true);
	}
}

void UMenu::OnJoinSession(EOnJoinSessionCompleteResult::Type Result)
{
	if (!MultiplayerSessionsSubsystem)
	{
		return;
	}

	if (const IOnlineSessionPtr SessionInterface = MultiplayerSessionsSubsystem->GetOnlineSession())
	{
		FString Address;
		SessionInterface->GetResolvedConnectString(NAME_GameSession, Address);

		if (APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController())
		{
			PlayerController->ClientTravel(Address, TRAVEL_Absolute);
		}
	}

	if(Result != EOnJoinSessionCompleteResult::UnknownError)
	{
		JoinButton->SetIsEnabled(true);
	}
}

void UMenu::OnDestroySession(bool bWasSuccessful)
{
}

void UMenu::OnStartSession(bool bWasSuccessful)
{
}
