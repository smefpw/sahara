#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include "valve_sdk/Misc/Color.h"

#define CONFIG(type, var, val) type var = val

class Options
{
public:
#pragma region Aimbot
	CONFIG(bool, Enabled, false);
	CONFIG(bool, AutoScope, false);
	CONFIG(bool, InfiniteDuck, false);
	CONFIG(bool, FakeDuck, false);

	CONFIG(int, Hitchance, 65);
	CONFIG(int, Damage, 25);
	CONFIG(int, SlowWalk, 25);
#pragma endregion

#pragma region Visuals
	CONFIG(bool, Health, false);
	CONFIG(bool, Name, false);
	CONFIG(bool, Box, false);
	CONFIG(bool, Radar, false);
	CONFIG(bool, Recoil, false);
	CONFIG(bool, Grenade, false);
	CONFIG(bool, Thirdperson, false);

	CONFIG(int, Size, 5);
	CONFIG(int, ViewDistance, 90);
#pragma endregion

#pragma region Misc
	CONFIG(bool, AutoBunnyhop, false);
	CONFIG(bool, RemoveScope, false);
#pragma endregion
};

inline Options Feature;