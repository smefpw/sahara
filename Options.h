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
	CONFIG(bool, Scope, false);

	CONFIG(bool, InfiniteDuck, false);
	CONFIG(bool, FakeDuck, false);

	CONFIG(int, Hitchance, 65);
	CONFIG(int, Damage, 35);
#pragma endregion

#pragma region Visuals
	CONFIG(bool, Health, false);
	CONFIG(bool, Name, false);
	CONFIG(bool, Box, false);
	CONFIG(bool, Radar, false);
	CONFIG(bool, Recoil, false);
	CONFIG(bool, Grenade, false);
	CONFIG(bool, Viewmodel, false);
	CONFIG(bool, Thirdperson, false);

	CONFIG(int, FOV, 0);
	CONFIG(int, Size, 5);
#pragma endregion

#pragma region Misc
	CONFIG(bool, AutoBunnyhop, false);
	CONFIG(bool, RemoveScope, false);
	CONFIG(bool, ForceInventory, false);
#pragma endregion

#pragma region Debug
	CONFIG(bool, Nearby, false);
	CONFIG(bool, Alive, false);
	CONFIG(bool, Scoped, false);
	CONFIG(bool, Holding, false);
#pragma endregion
};

inline Options Feature;