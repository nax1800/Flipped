#include "pch.h"
#include "UObject/UObjectGlobals.h"

UObject* StaticFindObject(UClass* Class, UObject* InOuter, const TCHAR* Name, bool ExactClass)
{
	static auto Func = reinterpret_cast<UObject * (*)(UClass*, UObject*, const TCHAR*, bool)>(InSDKUtils::GetImageBase() + 0xBC9B40);
	return Func(Class, InOuter, Name, ExactClass);
}