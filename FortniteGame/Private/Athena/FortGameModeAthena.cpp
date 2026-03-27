#include "pch.h"
#include "Athena/FortGameModeAthena.h"

#include "Engine/NetDriver.h"
#include "Logging/LogMacros.h"
#include "UObject/UObjectGlobals.h"

#include "IpNetDriver.h"

#include "Items/FortInventory.h"

DEFINE_LOG_CATEGORY(LogFortGameMode, ELogVerbosity::All);

bool FortGameModeAthena::ReadyToStartMatch(AFortGameModeAthena* Context)
{
	AFortGameStateAthena* GameState = reinterpret_cast<AFortGameStateAthena*>(Context->GameState);
	FPlaylistPropertyArray& CurrentPlaylistInfo = GameState->CurrentPlaylistInfo;

	if (CurrentPlaylistInfo.BasePlaylist == nullptr)
	{
		UFortPlaylistAthena* Playlist = StaticFindObject<UFortPlaylistAthena>(nullptr, L"/Game/Athena/Playlists/Playlist_DefaultSolo.Playlist_DefaultSolo");
		if (Playlist == nullptr) 
		{
			UE_LOG(LogFortGameMode, ELogVerbosity::Error, "Could not find playlist.");
			return false;
		}
		
		CurrentPlaylistInfo.BasePlaylist = Playlist;
		CurrentPlaylistInfo.OverridePlaylist = Playlist;
		CurrentPlaylistInfo.ArrayReplicationKey++;
		// CurrentPlaylistInfo.MarkArrayDirty();
		GameState->OnRep_CurrentPlaylistInfo();

		GameState->CurrentPlaylistId = Playlist->PlaylistId;
		GameState->OnRep_CurrentPlaylistId();

		Context->CurrentPlaylistId = Playlist->PlaylistId;
		Context->CurrentPlaylistName = Playlist->PlaylistName;

		GameState->AirCraftBehavior = Playlist->AirCraftBehavior;
		GameState->WorldLevel = Playlist->LootLevel;
		GameState->CachedSafeZoneStartUp = Playlist->SafeZoneStartUp;

		if (Playlist->MaxTeamSize > 1 && Playlist->DBNOType == EDBNOType::On)
		{
			Context->bDBNOEnabled = true;
			GameState->bDBNODeathEnabled = true;
			GameState->SetIsDBNODeathEnabled(true);
		}

		Context->AISettings = Playlist->AISettings;
		if (Context->AISettings != nullptr)
			Context->AISettings->AIServices[1] = UAthenaAIServicePlayerBots::StaticClass();

		Context->WarmupRequiredPlayerCount = 1;

		UE_LOG(LogFortGameMode, ELogVerbosity::Display, "Playlist set to %s.", Playlist->PlaylistName.ToString().c_str());
	}

	if (GameState->MapInfo == nullptr) return false;

	if (Context->bWorldIsReady == false)
	{
		UWorld* World = UWorld::GetWorld();

		FName NAME_GameNetDriver = UKismetStringLibrary::Conv_StringToName(FString(L"GameNetDriver"));

		UIpNetDriver* NetDriver = reinterpret_cast<UIpNetDriver*>(nullptr);
		if (NetDriver == nullptr)
		{
			UE_LOG(LogFortGameMode, ELogVerbosity::Error, "Could not create NetDriver.");
			return false;
		}

		NetDriver->World = World;
		NetDriver->NetDriverName = FName(282);

		World->NetDriver = NetDriver;
	

		FString Error;
		FURL URL{};
		URL.Port = 7777;

		if (IpNetDriver::InitListen(NetDriver, World, URL, false, Error) == true)
		{
			NetDriver::SetWorld(NetDriver, World);

			for (FLevelCollection& LevelCollection : World->LevelCollections) { LevelCollection.NetDriver = NetDriver; }
		}
		else
		{
			UE_LOG(LogFortGameMode, ELogVerbosity::Error, "Could not create NetDriver.");
			return false;
		}

		Context->bWorldIsReady = true;
	}

	return Context->NumPlayers > Context->WarmupRequiredPlayerCount;
}

APawn* FortGameModeAthena::SpawnDefaultPawnFor(AFortGameModeAthena* Context, APlayerController* NewPlayer, AActor* StartSpot)
{
	return Context->SpawnDefaultPawnAtTransform(NewPlayer, StartSpot->GetTransform());
}

void FortGameModeAthena::HandleStartingNewPlayer(AFortGameModeAthena* Context, AFortPlayerControllerAthena* NewPlayer)
{
	/*
	* TODO LIST:
		* Grant Abilities.
		* MAYBE: Setup creative portal.
		* MAYBE: Give LateGame Items. (If I do LateGame, probs not tbh)
	*/

	AFortPlayerStateAthena* PlayerState = reinterpret_cast<AFortPlayerStateAthena*>(NewPlayer->PlayerState);

	for (FItemAndCount StartingItem : Context->StartingItems)
	{
		FortInventory::AddItem(NewPlayer->WorldInventory, StartingItem.Item, StartingItem.Count);
	}
	FortInventory::AddItem(NewPlayer->WorldInventory, NewPlayer->CosmeticLoadoutPC.Pickaxe->WeaponDefinition);

	PlayerState->WorldPlayerId = PlayerState->PlayerId;

	return oHandleStartingNewPlayer(Context, NewPlayer);
}

bool FortGameModeAthena::Patch()
{
	return CreateHook(0x10ADCBC, ReadyToStartMatch) == true && CreateHook(0x5F85608, SpawnDefaultPawnFor) == true && CreateHook(0x63D4170, HandleStartingNewPlayer, (void**)&oHandleStartingNewPlayer) == true;
}
