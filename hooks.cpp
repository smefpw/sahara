#include "hooks.hpp"
#include <intrin.h>  
#include "helpers/input.hpp"
#include "options.hpp"
#include "helpers/Utilities.hpp"
#include "features/Visuals.hpp"
#include "features/Aimbot.hpp"
#include "features/Menu.hpp"

#pragma intrinsic(_ReturnAddress) 

namespace Hooks 
{
	vfunc_hook direct3d_hook;
	vfunc_hook hlclient_hook;
	vfunc_hook vguipanel_hook;
	vfunc_hook vguisurf_hook;
	vfunc_hook mdlrender_hook;
	vfunc_hook viewrender_hook;
	vfunc_hook enginehook_hook;
	vfunc_hook clientmode_hook;

	void Initialize()
	{
		// Set up virtual function hooks
		direct3d_hook.setup(g_D3DDevice9);
		hlclient_hook.setup(g_CHLClient);
		vguipanel_hook.setup(g_VGuiPanel);
		vguisurf_hook.setup(g_VGuiSurface);
		mdlrender_hook.setup(g_MdlRender);
		enginehook_hook.setup(g_EngineClient);
		clientmode_hook.setup(g_ClientMode);

		// Hook functions
		direct3d_hook.hook_index(index::EndScene, hkEndScene);
		direct3d_hook.hook_index(index::Reset, hkReset);
		hlclient_hook.hook_index(index::FrameStageNotify, hkFrameStageNotify);
		hlclient_hook.hook_index(index::CreateMove, hkCreateMove_Proxy);
		enginehook_hook.hook_index(index::EngineHook, IsConnected);
		vguipanel_hook.hook_index(index::PaintTraverse, hkPaintTraverse);
		vguisurf_hook.hook_index(index::LockCursor, hkLockCursor);
		mdlrender_hook.hook_index(index::DrawModelExecute, hkDrawModelExecute);
		clientmode_hook.hook_index(index::DoPostScreenSpaceEffects, hkDoPostScreenEffects);
		clientmode_hook.hook_index(index::GetViewmodelFOV, hkGetViewmodelFOV);
		clientmode_hook.hook_index(index::OverrideView, hkOverrideView);

		Render().CreateFonts();

		g_InputSystem->EnableInput(true);
	}
	//--------------------------------------------------------------------------------
	long __stdcall hkEndScene(IDirect3DDevice9* pDevice)
	{
		//--------------------------------------------------------------------------------
		static ConVar* fog_override = g_CVar->FindVar("fog_override");
		fog_override->m_fnChangeCallbacks.m_Size = 0;
		fog_override->SetValue(1);

		static ConVar* fog_enable = g_CVar->FindVar("fog_enable");
		fog_enable->m_fnChangeCallbacks.m_Size = 0;
		fog_enable->SetValue(0);

		static ConVar* sv_skyname = g_CVar->FindVar("sv_skyname");
		sv_skyname->m_fnChangeCallbacks.m_Size = 0;
		sv_skyname->SetValue("sky_csgo_night02");

		static ConVar* fog_enableskybox = g_CVar->FindVar("fog_enableskybox");
		fog_enableskybox->m_fnChangeCallbacks.m_Size = 0;
		fog_enableskybox->SetValue(0);
		//--------------------------------------------------------------------------------

		static auto oEndScene = direct3d_hook.get_original<decltype(&hkEndScene)>(index::EndScene);

		DWORD colorwrite, srgbwrite;
		pDevice->GetRenderState(D3DRS_COLORWRITEENABLE, &colorwrite);
		pDevice->GetRenderState(D3DRS_SRGBWRITEENABLE, &srgbwrite);

		pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0xffffffff);
		pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, false);

		pDevice->SetSamplerState(NULL, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
		pDevice->SetSamplerState(NULL, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
		pDevice->SetSamplerState(NULL, D3DSAMP_ADDRESSW, D3DTADDRESS_WRAP);
		pDevice->SetSamplerState(NULL, D3DSAMP_SRGBTEXTURE, NULL);
		
		Menu().RenderMenu();

		pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, colorwrite);
		pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, srgbwrite);

		return oEndScene(pDevice);
	}
	//--------------------------------------------------------------------------------
	long __stdcall hkReset(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* pPresentationParameters)
	{
		static auto oReset = direct3d_hook.get_original<decltype(&hkReset)>(index::Reset);

		Menu().OnDeviceLost();

		auto hr = oReset(device, pPresentationParameters);

		if (hr >= 0)
		{
			Menu().OnDeviceReset();
			Render().CreateFonts();
		}

		return hr;
	}
	//--------------------------------------------------------------------------------

	void __stdcall hkCreateMove(int sequence_number, float input_sample_frametime, bool active, bool& bSendPacket)
	{
		static auto oCreateMove = hlclient_hook.get_original<decltype(&hkCreateMove_Proxy)>(index::CreateMove);

		oCreateMove(g_CHLClient, 0, sequence_number, input_sample_frametime, active);

		auto cmd = g_Input->GetUserCmd(sequence_number);
		auto verified = g_Input->GetVerifiedCmd(sequence_number);

		if (!cmd || !cmd->command_number) return;

		if (g_LocalPlayer->IsAlive() && g_EngineClient->IsInGame())
		{
			Utilities::autoBhop(cmd);
		}

		C_BaseCombatWeapon* Weapon = g_LocalPlayer->m_hActiveWeapon();

		MovementFix().Start(cmd);
		RageAimbot().StartEnginePred(cmd);

		if (Feature.Enabled)
		{
			RageAimbot().StoreRecords();
			RageAimbot().Do(cmd, Weapon, bSendPacket);
		}

		RageAimbot().EndEnginePred();
		MovementFix().End(cmd);

		Math::Normalize(cmd->viewangles);
		Math::ClampAngles(cmd->viewangles);

		verified->m_cmd = *cmd;
		verified->m_crc = cmd->GetChecksum();
	}
	//--------------------------------------------------------------------------------
	__declspec(naked) void __fastcall hkCreateMove_Proxy(void* _this, int, int sequence_number, float input_sample_frametime, bool active)
	{
		__asm
		{
			push ebp
			mov  ebp, esp
			push ebx
			lea  ecx, [esp]
			push ecx
			push dword ptr[active]
			push dword ptr[input_sample_frametime]
			push dword ptr[sequence_number]
			call Hooks::hkCreateMove
			pop  ebx
			pop  ebp
			retn 0Ch
		}
	}
	//--------------------------------------------------------------------------------
	void __fastcall hkPaintTraverse(void* _this, int edx, vgui::VPANEL panel, bool forceRepaint, bool allowForce)
	{
		static auto panelId = vgui::VPANEL{ 0 };
		static auto oPaintTraverse = vguipanel_hook.get_original<decltype(&hkPaintTraverse)>(index::PaintTraverse);

		if (Feature.RemoveScope && strcmp("HudZoom", g_VGuiPanel->GetName(panel)) == 0) return;

		oPaintTraverse(g_VGuiPanel, edx, panel, forceRepaint, allowForce);

		if (!panelId) 
		{
			const auto panelName = g_VGuiPanel->GetName(panel);
			if (!strcmp(panelName, "FocusOverlayPanel")) 
			{
				panelId = panel;
			}
		}
		else if (panelId == panel) 
		{
			int ScreenWidth, ScreenHeight;
			g_EngineClient->GetScreenSize(ScreenWidth, ScreenHeight);

			if (!g_EngineClient->IsInGame() || !g_EngineClient->IsConnected() || !g_LocalPlayer) return;

			C_BaseCombatWeapon* Weapon = g_LocalPlayer->m_hActiveWeapon();

			if (InputSys().IsKeyDown(VK_TAB)) Utilities::RankRevealAll();

			if (Weapon && Feature.RemoveScope && Weapon->IsSniper() && g_LocalPlayer->m_bIsScoped())
			{
				Render().Line(ScreenWidth / 2, 0, ScreenWidth / 2, ScreenHeight, Color(0, 0, 0, 150));
				Render().Line(0, ScreenHeight / 2, ScreenWidth, ScreenHeight / 2, Color(0, 0, 0, 150));
			}

			// Recoil already has checks for if the localplayer is alive.
			if (Feature.Recoil) Visuals().Recoil();

			for (int i = 1; i <= 64; i++) 
			{
				C_BasePlayer* Player = C_BasePlayer::GetPlayerByIndex(i);

				if (!Player) continue;

				if (i < 65 && Player->IsAlive())
				{
					if (Visuals().Begin(Player)) 
					{
						if (Feature.Health) Visuals().Health();
						if (Feature.Name) Visuals().Name();
						if (Feature.Box) Visuals().Box();
						if (Feature.Radar) Visuals().Radar();
					}
				}
			}
		}
	}
	//--------------------------------------------------------------------------------
	int __fastcall hkDoPostScreenEffects(void* _this, int edx, int a1)
	{
		static auto oDoPostScreenEffects = clientmode_hook.get_original<decltype(&hkDoPostScreenEffects)>(index::DoPostScreenSpaceEffects);
		return oDoPostScreenEffects(g_ClientMode, edx, a1);
	}
	//--------------------------------------------------------------------------------
	void __fastcall hkFrameStageNotify(void* _this, int edx, ClientFrameStage_t stage)
	{
		static auto ofunc = hlclient_hook.get_original<decltype(&hkFrameStageNotify)>(index::FrameStageNotify);
		if (!g_EngineClient->IsInGame() || !g_EngineClient->IsConnected() || !g_LocalPlayer)
		{
			ofunc(g_CHLClient, edx, stage); return;
		}
		if (stage == FRAME_RENDER_START)
		{
			for (int i = 1; i <= 64; i++)
			{
				C_BasePlayer* Player = C_BasePlayer::GetPlayerByIndex(i);
				if (!Player || !Player->IsPlayer() || Player == g_LocalPlayer) continue;

				*(int*)((uintptr_t)Player + 0xA30) = g_GlobalVars->framecount;
				*(int*)((uintptr_t)Player + 0xA28) = 0;
			}
		}
		if (stage == FRAME_NET_UPDATE_POSTDATAUPDATE_END)
		{
			for (int i = 1; i <= 64; i++)
			{
				C_BasePlayer* Player = C_BasePlayer::GetPlayerByIndex(i);
				if (!Player || !Player->IsAlive()) continue;
				if (Player->IsDormant()) continue;

				auto varmap = reinterpret_cast<uintptr_t>(Player) + 36;
				auto varmap_size = *reinterpret_cast<int*>(varmap + 20);

				for (auto index = 0; index < varmap_size; index++)* 
					reinterpret_cast<uintptr_t*>(*reinterpret_cast<uintptr_t*>(varmap) + index * 12) = 0;
			}
		}

		ofunc(g_CHLClient, edx, stage);
	}
	//--------------------------------------------------------------------------------
	bool __stdcall IsConnected() {

		auto ofunc = enginehook_hook.get_original<IsConnected_t>(27);

		static void* unk = Utilities::PatternScan(GetModuleHandleA("client_panorama.dll"), "75 04 B0 01 5F") - 2;
		if (_ReturnAddress() == unk && Feature.ForceInventory) {
			return false;
		}

		return ofunc(g_EngineClient);
	}
	//--------------------------------------------------------------------------------
	void __fastcall hkLockCursor(void* _this)
	{
		static auto ofunc = vguisurf_hook.get_original<decltype(&hkLockCursor)>(index::LockCursor);

		if (Menu().IsVisible()) 
		{
			g_VGuiSurface->UnlockCursor();
			g_InputSystem->ResetInputState();
			return;
		}
		ofunc(g_VGuiSurface);
	}
	//--------------------------------------------------------------------------------
	void __fastcall hkOverrideView(void* _this, int edx, CViewSetup* vsView)
	{
		static auto ofunc = clientmode_hook.get_original<decltype(&hkOverrideView)>(index::OverrideView);
		if (!g_EngineClient->IsInGame() || !g_EngineClient->IsConnected())
		{
			ofunc(g_ClientMode, edx, vsView);
			return;
		}
		vsView->fov = 90.f;

		ofunc(g_ClientMode, edx, vsView);
	}
	float __stdcall hkGetViewmodelFOV()
	{
		static auto ofunc = clientmode_hook.get_original<GetViewmodelFOV>(index::GetViewmodelFOV);
		while (!g_EngineClient->IsTakingScreenshot())
		{
			if (Feature.FOV) return ofunc() + 35.f;
			else return ofunc();

		} return ofunc();
	}
	//--------------------------------------------------------------------------------
	void __fastcall hkDrawModelExecute(void* _this, int edx, IMatRenderContext* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& pInfo, matrix3x4_t* pCustomBoneToWorld)
	{
		mdlrender_hook.get_original<decltype(&hkDrawModelExecute)>(index::DrawModelExecute)(g_MdlRender, 0, ctx, state, pInfo, pCustomBoneToWorld);
		g_MdlRender->ForcedMaterialOverride(nullptr);
	}
}