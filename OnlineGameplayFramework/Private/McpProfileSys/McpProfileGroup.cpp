#include "pch.h"
#include "McpProfileSys/McpProfileGroup.h"

inline void McpProfileGroup::SendRequestNow(UMcpProfileGroup* Context,  /* TSharedRef<IHttpRequest>* */ void* HttpRequest ,  EContextCredentials ContextCredentials)
{
#ifdef NOMCP
    return oSendRequestNow(Context, HttpRequest, ContextCredentials);
#else
    return oSendRequestNow(Context, HttpRequest , EContextCredentials::CXC_Public );
#endif //NOMCP
}

bool McpProfileGroup::Patch()
{
    return CreateHook(0x1674270, SendRequestNow, (void**)&oSendRequestNow) == true;
}