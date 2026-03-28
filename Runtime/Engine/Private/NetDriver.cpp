#include "pch.h"
#include "Engine/NetDriver.h"
#include "Engine/ReplicationDriver.h"

void NetDriver::TickFlush(UNetDriver* Context, float DeltaSeconds)
{
	if (Context->ReplicationDriver != nullptr)
		ReplicationDriver::ServerReplicateActors(Context->ReplicationDriver, DeltaSeconds);

	return oTickFlush(Context, DeltaSeconds);
}

void NetDriver::SetWorld(UNetDriver* Context, UWorld* InWorld)
{
	static auto Func = reinterpret_cast<void(*)(UNetDriver*, UWorld*)>(InSDKUtils::GetImageBase() + 0x1597AE4);
	return Func(Context, InWorld);
}

bool NetDriver::Patch()
{
	return CreateHook(0xBC72C0, TickFlush, (void**)&oTickFlush) == true;
}

int32 ReplicationDriver::ServerReplicateActors(UReplicationDriver* Context, float DeltaSeconds)
{
	static auto Func = reinterpret_cast<int32(*)(UReplicationDriver*, float)>(Context->VTable[0x66]);
	return Func(Context, DeltaSeconds);
}
