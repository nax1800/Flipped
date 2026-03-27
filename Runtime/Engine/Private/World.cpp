#include "pch.h"
#include "Engine/World.h"

ENetMode World::GetNetMode(UWorld* Context)
{
	return ENetMode::NM_DedicatedServer;
}

bool World::Patch()
{
	return CreateHook(0xC9EEBC, GetNetMode) == true;
}
