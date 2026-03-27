#pragma once

class NetDriver
{
private:
	static inline void (*oTickFlush)(UNetDriver*, float);

public:
	static void TickFlush(UNetDriver* Context, float DeltaSeconds);

	static void SetWorld(UNetDriver* Context, UWorld* InWorld);

	static bool Patch();
};

