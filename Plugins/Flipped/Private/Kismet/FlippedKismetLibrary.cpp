#include "pch.h"
#include "Kismet/FlippedKismetLibrary.h"

#include "Logging/LogMacros.h"

#include "Items/FortWeaponItemDefinition.h"

DEFINE_LOG_CATEGORY(LogFlippedKismet, ELogVerbosity::All);

UFortItem* UFlippedKismetLibrary::CreateItem(UFortItemDefinition* ItemDefintion, int32 Count, int32 Level)
{
	UFortItem* CreatedItem = ItemDefintion->CreateTemporaryItemInstanceBP(Count, Level);
	return CreatedItem;
}

FFortItemEntry UFlippedKismetLibrary::CreateItemEntry(UFortItemDefinition* ItemDefintion, int32 Count, int32 Level, int32 LoadedAmmo)
{
	FFortItemEntry NewEntry{};
	if (ItemDefintion == nullptr)
	{
		UE_LOG(LogFlippedKismet, ELogVerbosity::Warning, "ItemDefinition is null in CreateItemEntry(), returning empty.");
		return NewEntry;
	}

	if (Count < 1 || Level < 1)
	{
		UE_LOG(LogFlippedKismet, ELogVerbosity::Warning, "Count/Level < 1 in CreateItemEntry(), returning empty.");
		return NewEntry;
	}

	UFortItem* WorldItem = CreateItem(ItemDefintion, Count, Level);
	
	int ClipSize = ItemDefintion->IsA(UFortWeaponItemDefinition::StaticClass()) ? FortWeaponItemDefinition::GetClipSize(reinterpret_cast<UFortWeaponItemDefinition*>(ItemDefintion)) : LoadedAmmo;

	NewEntry.ItemDefinition = ItemDefintion;
	NewEntry.ItemGuid = WorldItem->GetItemGuid();
	NewEntry.Count = Count;
	NewEntry.Level = Level;
	NewEntry.LoadedAmmo = LoadedAmmo == -1 ? ClipSize : LoadedAmmo;

	return NewEntry;
}

EFortQuickBars UFlippedKismetLibrary::GetItemQuickbar(UFortItemDefinition* ItemDefinition)
{
	bool bIsSecondary = ItemDefinition->IsA(UFortWeaponMeleeItemDefinition::StaticClass()) == true || ItemDefinition->IsA(UFortEditToolItemDefinition::StaticClass()) == true ||
		ItemDefinition->IsA(UFortBuildingItemDefinition::StaticClass()) == true || ItemDefinition->IsA(UFortAmmoItemDefinition::StaticClass()) == true ||
		ItemDefinition->IsA(UFortResourceItemDefinition::StaticClass()) == true || ItemDefinition->IsA(UFortTrapItemDefinition::StaticClass()) == true;

	return bIsSecondary ? EFortQuickBars::Secondary : EFortQuickBars::Primary;
}
