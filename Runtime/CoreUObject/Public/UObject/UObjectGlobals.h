#pragma once

UObject* StaticFindObject(UClass* Class, UObject* InOuter, const TCHAR* Name, bool ExactClass = false);

template <typename T>
inline T* StaticFindObject(UObject* InOuter, const TCHAR* Name, bool ExactClass = false)
{
	return reinterpret_cast<T*>(StaticFindObject(T::StaticClass(), InOuter, Name, ExactClass));
}

