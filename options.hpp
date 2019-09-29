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
	CONFIG(bool, RageAimbotEnabled, false);
	CONFIG(int, RageAimbotHitchance, 0);
	CONFIG(int, RageAimbotMinDmg, 0);
#pragma endregion

#pragma region Visuals
	CONFIG(bool, VisualsBox, false);
	CONFIG(bool, VisualsName, false);
	CONFIG(bool, VisualsHealth, false);
#pragma endregion

#pragma region Misc
	CONFIG(bool, MiscAutoBhop, false);
	CONFIG(bool, MiscVisualsNoScope, false);
#pragma endregion
};

inline Options Feature;