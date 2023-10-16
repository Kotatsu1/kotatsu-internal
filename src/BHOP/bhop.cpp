#include "bhop.h"
#include "../offsets.h"
#include "../globals.h"
#include <windows.h>


void BHOP::BHop()
{
	const auto client = reinterpret_cast<uintptr_t>(GetModuleHandle("client.dll"));
    const auto localPlayerPawn = *reinterpret_cast<uintptr_t*>(client + Offsets::dwLocalPlayerPawn);
    const auto flags = *reinterpret_cast<uint32_t*>(localPlayerPawn + Offsets::m_fFlags);

    if (GetAsyncKeyState(VK_SPACE) && flags & (1 << 0))
    {
        *reinterpret_cast<int*>(client + Offsets::dwForceJump) = 65537;
    }
    else
    {
        *reinterpret_cast<int*>(client + Offsets::dwForceJump) = 256;
    }
}

