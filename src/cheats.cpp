#include "cheats.h"
#include "offsets.h"
#include "globals.h"
#include <windows.h>
#include <thread>
#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <vector>


uintptr_t Cheats::GetClientDll()
{
	const auto client = reinterpret_cast<uintptr_t>(GetModuleHandle("client.dll"));

	return client;
}


void Cheats::GetMyHealth()
{
	const auto client = GetClientDll();

	const auto localPlayer = *reinterpret_cast<uintptr_t*>(client + Offsets::dwLocalPlayerController);

	const auto health = *reinterpret_cast<uintptr_t*>(localPlayer + Offsets::m_iHealth);
	printf("%d\n", health);
}


void Cheats::BHopThread()
{
	const auto client = GetClientDll();

	while (Globals::bhop)
	{
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
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}


void Cheats::BHop()
{
	std::thread bhopThread(&Cheats::BHopThread, this);
	bhopThread.detach();
}

ScreenCoordinates worldToScreen(Matrix viewMatrix, Vector3 origin)
{
	RECT desktop;
	const HWND hDesktop = GetDesktopWindow();
	GetWindowRect(hDesktop, &desktop);

	const int width = desktop.right;
	const int height = desktop.bottom;

	float screenW = (viewMatrix.m41 * origin.x) + (viewMatrix.m42 * origin.y) + (viewMatrix.m43 * origin.z) + viewMatrix.m44;

	if (screenW > 0.001f)
	{
		float screenX = (viewMatrix.m11 * origin.x) + (viewMatrix.m12 * origin.y) + (viewMatrix.m13 * origin.z) + viewMatrix.m14;
		float screenY = (viewMatrix.m21 * origin.x) + (viewMatrix.m22 * origin.y) + (viewMatrix.m23 * origin.z) + viewMatrix.m24;

		float cameraX = width / 2;
		float cameraY = height / 2;

		float X = cameraX + (cameraX * screenX / screenW);
		float Y = cameraY - (cameraY * screenY / screenW);

		ScreenCoordinates screenCoordinates;
		screenCoordinates.X = X;
		screenCoordinates.Y = Y;

		return screenCoordinates;
	}
}


std::vector<espData> Cheats::ESP()
{
	const auto client = GetClientDll();

	if (Globals::esp)
	{
		const auto entityList = *reinterpret_cast<uintptr_t*>(client + Offsets::dwEntityList);
		const auto localPlayer = *reinterpret_cast<uintptr_t*>(client + Offsets::dwLocalPlayerController);
		const int localTeam = *reinterpret_cast<int*>(localPlayer + Offsets::iTeamNum);
		const float localOrigin = *reinterpret_cast<float*>(localPlayer + Offsets::m_vecOrigin);

		std::vector<espData> espDataVector;

		for (int i = 0; i < 64; i++)
		{

			const auto listEntry = *reinterpret_cast<uintptr_t*>(entityList + 0x8 * ((i & 0x7FFF) >> 9) + 16);
			if (!listEntry)
			{
				continue;
			}

			const auto player = *reinterpret_cast<uintptr_t*>(listEntry + 120 * (i & 0x1FF));

			if (!player)
			{
				continue;
			}

			const int playerTeam = *reinterpret_cast<int*>(player + Offsets::iTeamNum);
			if (playerTeam == localTeam)
			{
				continue;
			}

			const std::uintptr_t playerPawn = *reinterpret_cast<std::uintptr_t*>(player + Offsets::dwPlayerPawn);

			const uintptr_t listEntry2 = *reinterpret_cast<uintptr_t*>(entityList + 0x8 * ((playerPawn & 0x7FFF) >> 9) + 16);

			if (!listEntry2)
			{
				continue;
			}

			const uintptr_t pCSPlayerPawn = *reinterpret_cast<uintptr_t*>(listEntry2 + 120 * (playerPawn & 0x1FF));

			if (!pCSPlayerPawn)
			{
				continue;
			}

			const int playerHealth = *reinterpret_cast<int*>(pCSPlayerPawn + Offsets::m_iHealth);

			if (playerHealth <= 0 || playerHealth > 100)
			{
				continue;
			}

			if (pCSPlayerPawn == localPlayer)
			{
				continue;
			}

			const Vector3 origin = *reinterpret_cast<Vector3*>(pCSPlayerPawn + Offsets::m_vecOrigin);
			const Vector3 head = {origin.x, origin.y, origin.z + 75.f};
			const Matrix viewMatrix = *reinterpret_cast<Matrix*>(client + Offsets::dwViewMatrix);

			auto feetCoordinates = worldToScreen(viewMatrix, origin);
			auto headCoordinates = worldToScreen(viewMatrix, head);

			espData espData;
			espData.feetX = feetCoordinates.X;
			espData.feetY = feetCoordinates.Y;
			espData.headX = headCoordinates.X;
			espData.headY = headCoordinates.Y;
			espData.health = playerHealth;

			espDataVector.push_back(espData);
		}
		return espDataVector;
	}
}

