#include "pch.h"

#include "Logging/LogMacros.h"

#include "Athena/FortGameModeAthena.h"

#include "Engine/World.h"
#include "Engine/NetDriver.h"

#include "McpProfileSys/McpProfileGroup.h"

DEFINE_LOG_CATEGORY(LogLaunch, ELogVerbosity::All);

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

    bool* GIsClient = reinterpret_cast<bool*>(InSDKUtils::GetImageBase() + 0xB30CF9F);
    UWorld* World = UWorld::GetWorld();

    *GIsClient = false;

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

    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ulReason, LPVOID lpReserved)
{
    if (ulReason == DLL_PROCESS_ATTACH)
        CreateThread(0, 0, thMain, 0, 0, 0);

    return TRUE;
}

