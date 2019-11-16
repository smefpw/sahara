#pragma once
#include "../Options.h"
#include "../valve_sdk/CSGOStructs.h"
#include "../Helpers/Math.h"
#include "../Helpers/Utilities.h"
#include "../Singleton.h"
#include "../Helpers/Input.h"

class FakeLag : public Singleton<FakeLag>
{
	friend class Singleton<FakeLag>;
public:
	void FakeDuck(CUserCmd* cmd, bool& bSendPacket);
	int duck_moment;
};

class MovementFix : public Singleton<MovementFix>
{
	friend class Singleton<MovementFix>;
public:
	void Start(CUserCmd* cmd);
	void End(CUserCmd* cmd);
private:
	float m_oldforward, m_oldsidemove;
	QAngle m_oldangle;
};

class RageAimbot : public Singleton<RageAimbot>
{
	friend class Singleton<RageAimbot>;
private:
	struct TickInfo
	{
		TickInfo()
		{
			SimulationTime = 0.f;
			Origin = Vector{};
			MatrixBuilt = false;
		}
		TickInfo(C_BasePlayer* Player)
		{
			SimulationTime = Player->m_flSimulationTime();
			Origin = Player->m_vecOrigin();
			MatrixBuilt = false;
			if (Player->SetupBones(BoneMatrix, 128, BONE_USED_BY_HITBOX, g_GlobalVars->curtime)) MatrixBuilt = true;
		}
		float SimulationTime;
		Vector Origin;
		bool MatrixBuilt;
		matrix3x4_t BoneMatrix[128];
	};
	bool Hitscan(C_BasePlayer* Player, Vector& HitboxPos, bool Backtrack, matrix3x4_t* BoneMatrix);
	bool Hitchance(C_BaseCombatWeapon* weapon, QAngle angles, C_BasePlayer* ent, float chance);
public:
	void StoreRecords();
	std::vector<TickInfo> BacktrackRecords[65];
	void StartEnginePred(CUserCmd* cmd);
	void EndEnginePred();
	void Do(CUserCmd* cmd, C_BaseCombatWeapon* Weapon, bool& bSendPacket);
};