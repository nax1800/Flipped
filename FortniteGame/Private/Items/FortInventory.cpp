#include "pch.h"
#include "Items/FortInventory.h"

#include "Logging/LogMacros.h"

#include "Kismet/FlippedKismetLibrary.h"

DEFINE_LOG_CATEGORY(LogInventory, ELogVerbosity::All);

bool FortInventory::IsFull(AFortInventory* Context)
{
	TArray<FFortItemEntry>& ReplicatedEntries = Context->Inventory.ReplicatedEntries;
	int Count = 0;
	for (int i = 0; i < ReplicatedEntries.Num(); i++)
	{
		if (ReplicatedEntries.IsValidIndex(i) == false) continue;

		if (UFlippedKismetLibrary::GetItemQuickbar(ReplicatedEntries[i].ItemDefinition) == EFortQuickBars::Primary)
		{
			Count += 1;
		}

		if (Count >= 5)
			break;
	}

	return Count >= 5;
}

FFortItemEntry* FortInventory::FindItem(AFortInventory* Context, UFortItemDefinition* ItemDefinition)
{
	TArray<FFortItemEntry>& ReplicatedEntries = Context->Inventory.ReplicatedEntries;
	FFortItemEntry* ItemEntry{};

	for (int i = 0; i < ReplicatedEntries.Num(); i++)
	{
		if (ReplicatedEntries.IsValidIndex(i) == false) continue;

		if (ReplicatedEntries[i].ItemDefinition == ItemDefinition)
		{
			ItemEntry = &ReplicatedEntries[i];
			break;
		}
	}

	return ItemEntry;
}

FFortItemEntry* FortInventory::FindItem(AFortInventory* Context, FGuid ItemGuid)
{
	TArray<FFortItemEntry>& ReplicatedEntries = Context->Inventory.ReplicatedEntries;
	FFortItemEntry* ItemEntry{};

	for (int i = 0; i < ReplicatedEntries.Num(); i++)
	{
		if (ReplicatedEntries.IsValidIndex(i) == false) continue;

		if (UKismetGuidLibrary::EqualEqual_GuidGuid(ReplicatedEntries[i].ItemGuid, ItemGuid) == true)
		{
			ItemEntry = &ReplicatedEntries[i];
			break;
		}
	}

	return ItemEntry;
}

bool FortInventory::AddItem(AFortInventory* Context, UFortItemDefinition* ItemDefinition, int Count, int LoadedAmmo, bool bForceNewItem)
{
	if (Context == nullptr || ItemDefinition == nullptr) return false;
	if (Count <= 0)
	{
		UE_LOG(LogInventory, ELogVerbosity::Display, "Count <= 0 (%i), if trying to remove items use RemoveItem()", Count);
		return false;
	}

	FFortItemEntry ItemEntry = UFlippedKismetLibrary::CreateItemEntry(ItemDefinition, Count, 1, LoadedAmmo);
	int MaxStackSize = ItemDefinition->MaxStackSize.Value;
	bool bAllowMultipleStacks = ItemDefinition->bAllowMultipleStacks;
	int OverStack = 0;

	if (bForceNewItem && IsFull(Context) == false)
	{
		if (bAllowMultipleStacks == false && FindItem(Context, ItemDefinition) != nullptr)
		{
			UE_LOG(LogInventory, ELogVerbosity::Warning, "Cannot have multiple stacks of item.");
			return false;
		}
		else
		{
			if (MaxStackSize < Count)
			{
				AFortPlayerControllerAthena* Owner = reinterpret_cast<AFortPlayerControllerAthena*>(Context->Owner);
				Owner->ServerAttemptInventoryDrop(ItemEntry.ItemGuid, Count - MaxStackSize, false);
				ItemEntry.Count = MaxStackSize;
			}

			if (ItemDefinition->IsA(UFortAmmoItemDefinition::StaticClass()) == true || ItemDefinition->IsA(UFortResourceItemDefinition::StaticClass()) == true)
			{
				FFortItemEntryStateValue Value{};
				Value.IntValue = 1;
				Value.StateType = EFortItemEntryState::ShouldShowItemToast;
				ItemEntry.StateValues.Add(Value);
			}

			Context->Inventory.ReplicatedEntries.Add(ItemEntry);
			Context->HandleInventoryLocalUpdate();
			Context->Inventory.MarkArrayDirty();
			Context->Inventory.MarkItemDirty(ItemEntry);
		}
	}
	else if (bForceNewItem && IsFull(Context) == true)
	{
		UE_LOG(LogInventory, ELogVerbosity::Warning, "Inventory is full, cannot create new item.");
		return false;
	}

	if (MaxStackSize > 1)
	{
		for (int i = 0; i < Context->Inventory.ReplicatedEntries.Num(); i++)
		{
			if (Context->Inventory.ReplicatedEntries.IsValidIndex(i) == false) continue;

			FFortItemEntry& CurrentEntry = Context->Inventory.ReplicatedEntries[i];

			if (CurrentEntry.ItemDefinition == ItemDefinition)
			{
				if (CurrentEntry.Count < MaxStackSize || bAllowMultipleStacks == false)
				{
					UFortWorldItem* StackingItemInstance = reinterpret_cast<UFortWorldItem*>(CurrentEntry.ItemDefinition);

					OverStack = CurrentEntry.Count + Count - MaxStackSize;

					if (bAllowMultipleStacks == false && CurrentEntry.Count > MaxStackSize)
						break;

					int AmountToStack = OverStack > 0 ? Count - OverStack : Count;

					FFortItemEntry* ReplicatedEntry = FindItem(Context, CurrentEntry.ItemGuid);

					CurrentEntry.Count += AmountToStack;
					ReplicatedEntry->Count += AmountToStack;

					if (ItemDefinition->IsA(UFortAmmoItemDefinition::StaticClass()) == true || ItemDefinition->IsA(UFortResourceItemDefinition::StaticClass()) == true)
					{
						FFortItemEntryStateValue Value{};
						Value.IntValue = 1;
						Value.StateType = EFortItemEntryState::ShouldShowItemToast;
						ReplicatedEntry->StateValues.Add(Value);
					}

					Context->Inventory.MarkItemDirty(CurrentEntry);
					Context->Inventory.MarkItemDirty(*ReplicatedEntry);

					if (OverStack <= 0)
						return true;
				}
			}
		}
	}



	Count = OverStack > 0 ? OverStack : Count;

	if (OverStack > 0 && bAllowMultipleStacks == false || OverStack > 0 && IsFull(Context) == true)
	{
		AFortPlayerControllerAthena* Owner = reinterpret_cast<AFortPlayerControllerAthena*>(Context->Owner);
		APlayerPawn_Athena_C* AcknowledgedPawn = static_cast<APlayerPawn_Athena_C*>(Owner->AcknowledgedPawn);

		if (AcknowledgedPawn == nullptr)
			return false;

		FFortItemEntry itemEntry{};
		itemEntry.Count = Count;
		itemEntry.ItemDefinition = ItemDefinition;

		//GameHelper::SpawnPickup(&itemEntry, Pawn->K2_GetActorLocation(), false, Pawn);
		return false;
	}

	if (MaxStackSize < Count)
	{
		AFortPlayerControllerAthena* Owner = reinterpret_cast<AFortPlayerControllerAthena*>(Context->Owner);
		Owner->ServerAttemptInventoryDrop(ItemEntry.ItemGuid, Count - MaxStackSize, false);
		ItemEntry.Count = MaxStackSize;
	}

	if (ItemDefinition->IsA(UFortAmmoItemDefinition::StaticClass()) || ItemDefinition->IsA(UFortResourceItemDefinition::StaticClass()))
	{
		FFortItemEntryStateValue Value{};
		Value.IntValue = 1;
		Value.StateType = EFortItemEntryState::ShouldShowItemToast;
		ItemEntry.StateValues.Add(Value);
	}

	Context->Inventory.ReplicatedEntries.Add(ItemEntry);
	Context->HandleInventoryLocalUpdate();
	Context->Inventory.MarkArrayDirty();
	Context->Inventory.MarkItemDirty(ItemEntry);

	return true;
}

bool FortInventory::AddItem(AFortInventory* Context, FFortItemEntry ItemEntry, bool bForceNewItem)
{
	return AddItem(Context, ItemEntry.ItemDefinition, ItemEntry.Count, ItemEntry.LoadedAmmo, bForceNewItem);
}
