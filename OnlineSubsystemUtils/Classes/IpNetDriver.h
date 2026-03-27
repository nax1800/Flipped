#pragma once
class IpNetDriver
{
public:
	static bool InitListen(UIpNetDriver* Context, UWorld* InNotify, FURL& LocalURL, bool bReuseAddressAndPort, FString& Error);
};

