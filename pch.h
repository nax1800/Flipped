#ifndef PCH_H
#define PCH_H

#include <Windows.h>
#include <iostream>
#include <format>
#include <vector>
#include <string>

#define DEV

#include "MinHook/include/MinHook.h"
#include "CppSDK/SDK.hpp"

using namespace std;
using namespace SDK;

inline bool CreateHook(uintptr_t Offset, LPVOID Detour, LPVOID* Original = nullptr)
{
	void* Address = reinterpret_cast<void*>(InSDKUtils::GetImageBase() + Offset);
	MH_CreateHook(Address, Detour, Original);
	return MH_EnableHook(Address) == MH_OK;
}

template<typename T = UObject>
inline bool VirtualHook(uintptr_t Index, LPVOID Detour, LPVOID* Original = nullptr)
{
	UObject* DefaultObj = T::GetDefaultObj();
	if (DefaultObj == nullptr || DefaultObj->VTable == nullptr)
		return false;

	if (Original != nullptr)
		*Original = DefaultObj->VTable[Index];

	DWORD OldProtection;
	VirtualProtect(&DefaultObj->VTable[Index], sizeof(void*), PAGE_EXECUTE_READWRITE, &OldProtection);
	DefaultObj->VTable[Index] = Detour;
	return VirtualProtect(&DefaultObj->VTable[Index], 8, OldProtection, NULL);
}

static void* NullOriginal = nullptr;

template <typename T = void*>
inline void ExecHook(UFunction* Function, void* Detour, T& Original = NullOriginal)
{
	if (Function == nullptr) 
		return;

	if (is_same_v<T, void*> == false)
		Original = reinterpret_cast<T>(Function->ExecFunction);

	Function->ExecFunction = reinterpret_cast<UFunction::FNativeFuncPtr>(Detour);
}

#endif //PCH_H
