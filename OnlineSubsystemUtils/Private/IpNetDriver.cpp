#include "pch.h"
#include "IpNetDriver.h"

bool IpNetDriver::InitListen(UIpNetDriver* Context, UWorld* InNotify, FURL& LocalURL, bool bReuseAddressAndPort, FString& Error)
{
	static auto Func = reinterpret_cast<bool(*)(UIpNetDriver * Context, UWorld * InNotify, FURL & LocalURL, bool bReuseAddressAndPort, FString & Error)>(InSDKUtils::GetImageBase() + 0x515058C);
	return Func(Context, InNotify, LocalURL, bReuseAddressAndPort, Error);
}
