#pragma once
#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include "valve_sdk/Misc/Color.hpp"

#define CONFIG(type, var, val) type var = val

class Options
{
public:
#pragma region Rage
	CONFIG(bool, AimbotEnabled, false);
	CONFIG(int, AimbotHitchance, 0);
	CONFIG(int, AimbotMinDmg, 0);
#pragma endregion

#pragma region Visuals
	CONFIG(bool, Box, false);
	CONFIG(bool, Name, false);
	CONFIG(bool, Health, false);
#pragma endregion

#pragma region Misc
	CONFIG(bool, AutoBhop, false);
	CONFIG(bool, NoScope, false);
#pragma endregion
};

inline Options Feature;