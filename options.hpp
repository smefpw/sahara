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
#pragma region Legitbot
	CONFIG(bool, LegitAimbotEnabled, false);
	CONFIG(int, LegitAimbotType, 0);
	CONFIG(int, LegitAimbotHitbox, 0);
	CONFIG(float, LegitAimbotFov, 0.f);
	CONFIG(int, LegitAimbotSmooth, 0);
	CONFIG(int, LegitAimbotRcs, 0);

	CONFIG(bool, LegitBacktrackEnabled, false);
	CONFIG(int, LegitBacktrackDuration, 200);
#pragma endregion

#pragma region Rage
	CONFIG(bool, RageAimbotEnabled, false);
	CONFIG(int, RageAimbotHitchance, 0);
	CONFIG(int, RageAimbotMinDmg, 0);
	CONFIG(bool, RageAntiaimEnabled, false);
#pragma endregion

#pragma region Visuals
	CONFIG(bool, VisualsBox, false);
	CONFIG(bool, VisualsName, false);
	CONFIG(bool, VisualsWeapon, false);
	CONFIG(bool, VisualsHealth, false);
	CONFIG(bool, VisualsChams, false);
#pragma endregion

#pragma region Misc
	CONFIG(bool, AutoBhop, false);
	CONFIG(int, MiscFakelagChoke, 0);
	CONFIG(bool, VisualsNoScope, false);
#pragma endregion
};

inline Options Feature;