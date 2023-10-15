#pragma once
#include <stdint.h>
#include <vector>

struct ScreenCoordinates
{
	float X;
	float Y;
};


class Cheats
{
public:
	uintptr_t GetClientDll();
	void GetMyHealth();
	void BHop();
	void BHopThread();
	 std::vector<ScreenCoordinates> ESP();
};