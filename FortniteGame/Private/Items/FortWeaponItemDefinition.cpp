#include "pch.h"
#include "Items/FortWeaponItemDefinition.h"

#include "Logging/LogMacros.h"

int FortWeaponItemDefinition::GetClipSize(UFortWeaponItemDefinition* Context)
{
	if (Context == nullptr)
		return -1;

	FFortRangedWeaponStats WeaponStats{};
	if (UFortKismetLibrary::GetRangedWeaponStatsRow(Context->WeaponStatHandle, &WeaponStats) == true)
	{
		return WeaponStats.ClipSize;
	}
	else
	{
		UE_LOG(LogFortItem, ELogVerbosity::Warning, "UFortKismetLibrary::GetWeaponStatsRow() failed, using manual method.");

		UDataTable* DataTable = Context->WeaponStatHandle.DataTable;
		FName RowName = Context->WeaponStatHandle.RowName;

		if (DataTable == nullptr || RowName.IsNone() == true)
			return -1;

		TMap<FName, uint8*>& RowMap = DataTable->RowMap;

		for (int i = 0; i < RowMap.Num(); ++i)
		{
			TPair<FName, uint8*>& Pair = RowMap[i];

			if (Pair.Key() == RowName)
				return reinterpret_cast<FFortRangedWeaponStats*>(Pair.Second)->ClipSize;
		}
	}

	UE_LOG(LogFortItem, ELogVerbosity::Warning, "Failed to get clip size, returning -1.");
	return -1;
}
