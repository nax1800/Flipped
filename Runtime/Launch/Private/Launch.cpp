#include "pch.h"

#include "Logging/LogMacros.h"

#include "Athena/FortGameModeAthena.h"

#include "Engine/World.h"
#include "Engine/NetDriver.h"

#include "McpProfileSys/McpProfileGroup.h"

DEFINE_LOG_CATEGORY(LogLaunch, ELogVerbosity::All);

#ifdef DEV
    inline void (*oProcessEvent)(UObject*, UFunction*, void*);
    inline void ProcessEvent(UObject* Context, UFunction* Function, void* Parameters)
    {
        if (Context == nullptr || Function == nullptr)
            return oProcessEvent(Context, Function, Parameters);

        string FunctionName = Function->GetName();
        /*
            if (FunctionName == "")
            {
                * Log here, call something, etc....
            }
        */

        return oProcessEvent(Context, Function, Parameters);
    }
#endif // DEV

DWORD thMain(LPVOID lpVoid)
{
#ifdef DEV
    AllocConsole();
    FILE* File;
    freopen_s(&File, "CONOUT$", "w+", stdout);
    SetConsoleTitle(L"Flipped Dev Console || 19.10");
#endif

    MH_STATUS InitialiseStatus = MH_Initialize();
    if (InitialiseStatus != MH_OK)
    {
        UE_LOG(LogLaunch, ELogVerbosity::Fatal, "MinHook failed to initialise, aborting.... (returned %s)", MH_StatusToString(InitialiseStatus));
        return 0;
    }

    Sleep(3000);

    UWorld* World = UWorld::GetWorld();

    *reinterpret_cast<bool*>(InSDKUtils::GetImageBase() + 0xB30CF9F) = false; // GIsClient

    World->OwningGameInstance->LocalPlayers.Remove(0);
    UKismetSystemLibrary::ExecuteConsoleCommand(World, FString(L"open Artemis_Terrain"), nullptr);

    bool bAreHooksOK = FortGameModeAthena::Patch() && World::Patch() && NetDriver::Patch() && McpProfileGroup::Patch();
    if (bAreHooksOK == false)
    {
        UE_LOG(LogLaunch, ELogVerbosity::Fatal, "Hooks weren't successful, aborting....");
        return 0;
    }
    else
        UE_LOG(LogLaunch, ELogVerbosity::Display, "Hooks successful.");

#ifdef DEV
    if (CreateHook(Offsets::ProcessEvent, ProcessEvent, (void**)&oProcessEvent) == true)
        UE_LOG(LogLaunch, ELogVerbosity::Display, "ProcessEvent has been hooked.");
    else
        UE_LOG(LogLaunch, ELogVerbosity::Warning, "ProcessEvent has failed to hook.");
#endif // DEV

#ifdef NOMCP
    if (CreateHook(0x106A3BC, IsRunningNoMCP) == true)
        UE_LOG(LogLaunch, ELogVerbosity::Display, "IsRunningNoMCP has been hooked.");
#endif //NOMCP

    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ulReason, LPVOID lpReserved)
{
    if (ulReason == DLL_PROCESS_ATTACH)
        CreateThread(0, 0, thMain, 0, 0, 0);

    return TRUE;
}

