#pragma once
class UFlippedKismetLibrary
{
public:
	static UFortItem* CreateItem(UFortItemDefinition* ItemDefintion, int32 Count, int32 Level);
	static FFortItemEntry CreateItemEntry(UFortItemDefinition* ItemDefintion, int32 Count, int32 Level, int32 LoadedAmmo = -1);
	static EFortQuickBars GetItemQuickbar(UFortItemDefinition* ItemDefinition);
};

