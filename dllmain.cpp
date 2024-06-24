#include "hooks.h"

DWORD WINAPI Main(PVOID pParam)
{
    Util::InitConsole();
    Offsets::Init();
    Hooks::Init();
    Patch();
    UnlockConsole();

    while (true)
    {
        if (GetAsyncKeyState(VK_F3) && bIsTickHooked == false)
        {
            Hook(TickFlush, TickFlush_Hook);
            bIsTickHooked = true;
            break;
        }
    }

    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, PVOID pReserved)
{
    switch (dwReason)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(0, 0, Main, hModule, 0, 0);
        break;
    }
    return TRUE;
}