#pragma once
#include "Engine/EngineBaseTypes.h"

class World
{
public:
	static ENetMode GetNetMode(UWorld* Context);

	static bool Patch();
};

