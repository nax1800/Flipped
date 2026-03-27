#pragma once

class FortInventory
{
public:
	static bool IsFull(AFortInventory* Context);

	static FFortItemEntry* FindItem(AFortInventory* Context, UFortItemDefinition* ItemDefinition);
	static FFortItemEntry* FindItem(AFortInventory* Context, FGuid ItemGuid);

	static bool AddItem(AFortInventory* Context, UFortItemDefinition* ItemDefinition, int Count = 1, int LoadedAmmo = -1, bool bForceNewItem = false);
	static bool AddItem(AFortInventory* Context, FFortItemEntry ItemEntry, bool bForceNewItem = false);
};

