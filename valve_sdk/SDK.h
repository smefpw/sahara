#pragma once

#define NOMINMAX
#include <Windows.h>

#include "Misc/Enums.h"
#include "Misc/VFunc.h"

#include "Math/VMatrix.h"
#include "Math/QAngle.h"
#include "Math/Vector.h"
#include "Misc/Studio.h"

#include "Interfaces/IAppSystem.h"
#include "Interfaces/IBaseClientDll.h"
#include "Interfaces/IClientEntity.h"
#include "Interfaces/IClientEntityList.h"
#include "Interfaces/IClientMode.h"
#include "Interfaces/IConVar.h"
#include "Interfaces/ICvar.h"
#include "Interfaces/IEngineTrace.h"
#include "Interfaces/IVEngineClient.h"
#include "Interfaces/IVDebugOverlay.h"
#include "Interfaces/ISurface.h"
#include "Interfaces/CInput.h"
#include "Interfaces/IVModelInfoClient.h"
#include "Interfaces/IVModelRender.h"
#include "Interfaces/IRenderView.h"
#include "Interfaces/IGameEventmanager.h"
#include "Interfaces/IMaterialSystem.h"
#include "Interfaces/IMoveHelper.h"
#include "Interfaces/IMDLCache.h"
#include "Interfaces/IPrediction.h"
#include "Interfaces/IPanel.h"
#include "Interfaces/IEngineSound.h"
#include "Interfaces/IViewRender.h"
#include "Interfaces/CClientState.h"
#include "Interfaces/IPhysics.h"
#include "Interfaces/IInputSystem.hpp"
#include "interfaces/IRefCounted.h"

#include "Misc/Convar.h"
#include "Misc/CUserCmd.h"
#include "Misc/glow_outline_effect.h"
#include "Misc/datamap.h"

#include "Netvars.h"

struct IDirect3DDevice9;

namespace Interfaces
{
    void Initialize();
}

inline IVEngineClient*       g_EngineClient;
inline IBaseClientDLL*       g_CHLClient;
inline IClientEntityList*    g_EntityList;
inline CGlobalVarsBase*      g_GlobalVars;
inline IEngineTrace*         g_EngineTrace;
inline ICvar*                g_CVar;
inline IPanel*               g_VGuiPanel;
inline IClientMode*          g_ClientMode;
inline IVDebugOverlay*       g_DebugOverlay;
inline ISurface*             g_VGuiSurface;
inline CInput*               g_Input;
inline IVModelInfoClient*    g_MdlInfo;
inline IVModelRender*        g_MdlRender;
inline IVRenderView*         g_RenderView;
inline IMaterialSystem*      g_MatSystem;
inline IGameEventManager2*   g_GameEvents;
inline IMoveHelper*          g_MoveHelper;
inline IMDLCache*            g_MdlCache;
inline IPrediction*          g_Prediction;
inline CGameMovement*        g_GameMovement;
inline IEngineSound*         g_EngineSound;
inline CGlowObjectManager*   g_GlowObjManager;
inline IViewRender*          g_ViewRender;
inline IDirect3DDevice9*     g_D3DDevice9;
inline CClientState*         g_ClientState;
inline IPhysicsSurfaceProps* g_PhysSurface;
inline IInputSystem*         g_InputSystem;

template<typename... Args>
void ConMsg(const char* pMsg, Args... args)
{
    static auto import = (void(*)(const char*, ...))GetProcAddress(GetModuleHandleW(L"tier0.dll"), "?ConMsg@@YAXPBDZZ");
    return import(pMsg, args...);
}
template<typename... Args>
void ConColorMsg(const Color& clr, const char* pMsg, Args... args)
{
    static auto import = (void(*)(const Color&, const char*, ...))GetProcAddress(GetModuleHandleW(L"tier0.dll"), "?ConColorMsg@@YAXABVColor@@PBDZZ");
    return import(clr, pMsg, args...);
}

#include "Misc/EHandle.h"

class C_LocalPlayer
{
    friend bool operator==(const C_LocalPlayer& lhs, void* rhs);
public:
    C_LocalPlayer() : m_local(nullptr) {}

    operator bool() const { return *m_local != nullptr; }
    operator C_BasePlayer*() const { return *m_local; }

    C_BasePlayer* operator->() { return *m_local; }

private:
    C_BasePlayer** m_local;
};

inline C_LocalPlayer g_LocalPlayer;

