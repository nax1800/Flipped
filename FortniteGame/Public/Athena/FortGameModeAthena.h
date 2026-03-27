#pragma once

class FortGameModeAthena
{ 
private:
	static inline void (*oHandleStartingNewPlayer)(AFortGameModeAthena*, AFortPlayerControllerAthena*);

public:
	static bool ReadyToStartMatch(AFortGameModeAthena* Context);
	static APawn* SpawnDefaultPawnFor(AFortGameModeAthena* Context, APlayerController* NewPlayer, AActor* StartSpot);
	static void HandleStartingNewPlayer(AFortGameModeAthena* Context, AFortPlayerControllerAthena* NewPlayer);

	static bool Patch();
};

