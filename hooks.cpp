#include "Hooks.h"
#include <intrin.h>  
#include "Helpers/Input.h"
#include "Options.h"
#include "Helpers/Utilities.h"
#include "Features/Visuals.h"
#include "Features/Aimbot.h"
#include "Features/Menu.h"

#pragma intrinsic(_ReturnAddress) 

namespace Hooks
{
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
		direct3d_hook.hook_index(42, hkEndScene);
		direct3d_hook.hook_index(16, hkReset);
		hlclient_hook.hook_index(37, hkFrameStageNotify);
		hlclient_hook.hook_index(22, hkCreateMove_Proxy);
		vguipanel_hook.hook_index(41, hkPaintTraverse);
		vguisurf_hook.hook_index(67, hkLockCursor);
		mdlrender_hook.hook_index(21, hkDrawModelExecute);
		clientmode_hook.hook_index(44, hkDoPostScreenEffects);
		clientmode_hook.hook_index(18, hkOverrideView);

		Render::Get().CreateFonts();

		g_InputSystem->EnableInput(true);
	}
	//--------------------------------------------------------------------------------
	long __stdcall hkEndScene(IDirect3DDevice9* pDevice)
	{
		//--------------------------------------------------------------------------------
		static ConVar* fog_enable = g_CVar->FindVar("fog_enable");
		fog_enable->m_fnChangeCallbacks.m_Size = 0;
		fog_enable->m_nFlags &= ~FCVAR_CHEAT;
		fog_enable->SetValue(0);

		static ConVar* fog_enableskybox = g_CVar->FindVar("fog_enableskybox");
		fog_enableskybox->m_fnChangeCallbacks.m_Size = 0;
		fog_enableskybox->m_nFlags &= ~FCVAR_CHEAT;
		fog_enableskybox->SetValue(0);

		static ConVar* cl_grenadepreview = g_CVar->FindVar("cl_grenadepreview");
		cl_grenadepreview->m_fnChangeCallbacks.m_Size = 0;
		cl_grenadepreview->m_nFlags &= ~FCVAR_CHEAT;
		cl_grenadepreview->SetValue(Feature.Grenade);
		//--------------------------------------------------------------------------------

		static auto oEndScene = direct3d_hook.get_original<decltype(&hkEndScene)>(42);

		DWORD colorwrite, srgbwrite;
		pDevice->GetRenderState(D3DRS_COLORWRITEENABLE, &colorwrite);
		pDevice->GetRenderState(D3DRS_SRGBWRITEENABLE, &srgbwrite);

		pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0xffffffff);
		pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, false);

		pDevice->SetSamplerState(NULL, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
		pDevice->SetSamplerState(NULL, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
		pDevice->SetSamplerState(NULL, D3DSAMP_ADDRESSW, D3DTADDRESS_WRAP);
		pDevice->SetSamplerState(NULL, D3DSAMP_SRGBTEXTURE, NULL);
		
		Menu::Get().RenderMenu();

		pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, colorwrite);
		pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, srgbwrite);

		return oEndScene(pDevice);
	}
	//--------------------------------------------------------------------------------
	long __stdcall hkReset(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* pPresentationParameters)
	{
		static auto oReset = direct3d_hook.get_original<decltype(&hkReset)>(16);

		Menu::Get().OnDeviceLost();

		auto hr = oReset(device, pPresentationParameters);

		if (hr >= 0)
		{
			Menu::Get().OnDeviceReset();
			Render::Get().CreateFonts();
		}

		return hr;
	}
	//--------------------------------------------------------------------------------

	void __stdcall hkCreateMove(int sequence_number, float input_sample_frametime, bool active, bool& bSendPacket)
	{
		static auto oCreateMove = hlclient_hook.get_original<decltype(&hkCreateMove_Proxy)>(22);

		oCreateMove(g_CHLClient, 0, sequence_number, input_sample_frametime, active);

		auto cmd = g_Input->GetUserCmd(sequence_number);
		auto verified = g_Input->GetVerifiedCmd(sequence_number);

		if (!cmd || !cmd->command_number) return;

		if (g_LocalPlayer->IsAlive() && g_EngineClient->IsInGame())
		{
			Utilities::autoBhop(cmd);
		}

		C_BaseCombatWeapon* Weapon = g_LocalPlayer->m_hActiveWeapon();

		if (Feature.InfiniteDuck) cmd->buttons |= IN_BULLRUSH;
		
		FakeLag::Get().FakeDuck(cmd, bSendPacket);
		RageAimbot::Get().SlowWalk(cmd);
		MovementFix::Get().Start(cmd);
		RageAimbot::Get().StartEnginePred(cmd);

		if (Feature.Enabled)
		{
			RageAimbot::Get().StoreRecords();
			RageAimbot::Get().Do(cmd, Weapon, bSendPacket);
		}

		RageAimbot::Get().EndEnginePred();
		MovementFix::Get().End(cmd);

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
		static auto oPaintTraverse = vguipanel_hook.get_original<decltype(&hkPaintTraverse)>(41);

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

			const auto margin = 10;
			int tall, wide;		
			const char* watermark = "Sahara for Counter-Strike: Global Offensive";
			
			Render::Get().TextSize(wide, tall, watermark, Render::Get().Visuals);
			Render::Get().Text(ScreenWidth - wide - margin, margin, watermark, Render::Get().Visuals, Color(255, 55, 55, 255), false);

			if (!g_EngineClient->IsInGame() || !g_EngineClient->IsConnected() || !g_LocalPlayer) return;

			C_BaseCombatWeapon* Weapon = g_LocalPlayer->m_hActiveWeapon();

			if (InputSys::Get().IsKeyDown(VK_TAB)) Utilities::RankRevealAll();

			if (Weapon && Feature.RemoveScope && Weapon->IsSniper() && g_LocalPlayer->m_bIsScoped())
			{
				Render::Get().Line(ScreenWidth / 2, 0, ScreenWidth / 2, ScreenHeight, Color(0, 0, 0, 150));
				Render::Get().Line(0, ScreenHeight / 2, ScreenWidth, ScreenHeight / 2, Color(0, 0, 0, 150));
			}

			if (Feature.Recoil && g_EngineClient->IsInGame() && g_EngineClient->IsConnected() && g_LocalPlayer->IsAlive())
			{
				Visuals::Get().Recoil();
			}
			
			for (int i = 1; i <= 64; i++) 
			{
				auto pEntity = C_BasePlayer::GetPlayerByIndex(i);

				if (!pEntity) continue;

				if (i < 65 && pEntity->IsAlive())
				{
					if (Visuals::Get().Begin(pEntity))
					{
						if (Feature.Health) Visuals::Get().Health();
						if (Feature.Name) Visuals::Get().Name();
						if (Feature.Box) Visuals::Get().Box();
						if (Feature.Radar) Visuals::Get().Radar();
					}
				}
			}
		}
	}
	//--------------------------------------------------------------------------------
	int __fastcall hkDoPostScreenEffects(void* _this, int edx, int a1)
	{
		static auto oDoPostScreenEffects = clientmode_hook.get_original<decltype(&hkDoPostScreenEffects)>(44);
		return oDoPostScreenEffects(g_ClientMode, edx, a1);
	}
	//--------------------------------------------------------------------------------
	void __fastcall hkFrameStageNotify(void* _this, int edx, ClientFrameStage_t stage)
	{
		static auto ofunc = hlclient_hook.get_original<decltype(&hkFrameStageNotify)>(37);
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
	void __fastcall hkLockCursor(void* _this)
	{
		static auto ofunc = vguisurf_hook.get_original<decltype(&hkLockCursor)>(67);

		if (Menu::Get().IsVisible()) 
		{
			g_VGuiSurface->UnlockCursor();
			return;
		}
		ofunc(g_VGuiSurface);
	}
	//--------------------------------------------------------------------------------
	void __fastcall hkOverrideView(void* _this, int edx, CViewSetup* vsView)
	{
		static auto ofunc = clientmode_hook.get_original<decltype(&hkOverrideView)>(18);

		if (g_EngineClient->IsInGame() && !g_LocalPlayer->m_bIsScoped() && g_LocalPlayer->IsAlive())
			vsView->fov = static_cast<float>(Feature.ViewDistance);

		Visuals::Get().Thirdperson();

		ofunc(g_ClientMode, edx, vsView);
	}
	//--------------------------------------------------------------------------------
	void __fastcall hkDrawModelExecute(void* _this, int edx, IMatRenderContext* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& pInfo, matrix3x4_t* pCustomBoneToWorld)
	{
		static auto ofunc = mdlrender_hook.get_original<decltype(&hkDrawModelExecute)>(21);
		static auto vmt_material = g_MatSystem->FindMaterial("debug/debugambientcube", TEXTURE_GROUP_MODEL);

		if (!vmt_material || vmt_material->IsErrorMaterial()) return;

		auto p_entity = C_BasePlayer::GetPlayerByIndex(pInfo.entity_index);

		if (p_entity && p_entity == g_LocalPlayer && g_Input->m_fCameraInThirdPerson)
		{
			float alpha;
			g_LocalPlayer->m_bIsScoped() ? alpha = 0.25f : alpha = 1.f;

			vmt_material->IncrementReferenceCount();
			vmt_material->ColorModulate(1.f, 1.f, 1.f);
			vmt_material->AlphaModulate(alpha);

			g_MdlRender->ForcedMaterialOverride(vmt_material);
		}
		
		ofunc(g_MdlRender, 0, ctx, state, pInfo, pCustomBoneToWorld);
		g_MdlRender->ForcedMaterialOverride(nullptr);
	}
}