#pragma once
#include "valve_sdk/csgostructs.hpp"
#include "helpers/vfunc_hook.hpp"
#include <d3d9.h>

namespace index
{
	constexpr auto EndScene					= 42;
	constexpr auto Reset					= 16;
    constexpr auto PaintTraverse            = 41;
    constexpr auto CreateMove               = 22;
    constexpr auto FrameStageNotify         = 37;
    constexpr auto DrawModelExecute         = 21;
    constexpr auto DoPostScreenSpaceEffects = 44;
	constexpr auto SvCheatsGetBool          = 13;
	constexpr auto OverrideView             = 18;
	constexpr auto EngineHook				= 27;
	constexpr auto LockCursor               = 67;
	constexpr auto GetViewmodelFOV			= 35;
}

namespace Hooks
{
	void Initialize();

	extern vfunc_hook direct3d_hook;
	extern vfunc_hook hlclient_hook;
	extern vfunc_hook vguipanel_hook;
	extern vfunc_hook vguisurf_hook;
	extern vfunc_hook mdlrender_hook;
	extern vfunc_hook viewrender_hook;
	extern vfunc_hook enginehook_hook;
	extern vfunc_hook clientmode_hook;

	using GetViewmodelFOV = float(__thiscall*)();
	using IsConnected_t = bool(__thiscall*)(IVEngineClient*);

	long __stdcall hkEndScene(IDirect3DDevice9* device);
	long __stdcall hkReset(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* pPresentationParameters);
    void __stdcall hkCreateMove(int sequence_number, float input_sample_frametime, bool active, bool& bSendPacket);
	void __fastcall hkCreateMove_Proxy(void* _this, int, int sequence_number, float input_sample_frametime, bool active);
	void __fastcall hkPaintTraverse(void* _this, int edx, vgui::VPANEL panel, bool forceRepaint, bool allowForce);
	void __fastcall hkDrawModelExecute(void* _this, int, IMatRenderContext* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& pInfo, matrix3x4_t* pCustomBoneToWorld);
    void __fastcall hkFrameStageNotify(void* _this, int, ClientFrameStage_t stage);
	void __fastcall hkOverrideView(void* _this, int, CViewSetup * vsView);
	int  __fastcall hkDoPostScreenEffects(void* _this, int, int a1);
	void __fastcall hkLockCursor(void* _this);
	float __stdcall hkGetViewmodelFOV();
	bool __stdcall IsConnected();
}