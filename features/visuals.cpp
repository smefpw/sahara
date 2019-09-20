#include "visuals.hpp"
#include "../hooks.hpp"
#include "legitbot.hpp"
#include "ragebot.hpp"

void Render::CreateFonts()
{
	Watermark = g_VGuiSurface->CreateFont_();
	g_VGuiSurface->SetFontGlyphSet(Watermark, "Verdana", 16, 600, 0, 0, FONTFLAG_OUTLINE);

	Visuals = g_VGuiSurface->CreateFont_();
	g_VGuiSurface->SetFontGlyphSet(Visuals, "Tahoma", 12, 400, 0, 0, FONTFLAG_OUTLINE);
}
void Render::Text(int X, int Y, const char* Text, vgui::HFont Font, Color DrawColor, bool Center)
{
	std::wstring WText = std::wstring(std::string_view(Text).begin(), std::string_view(Text).end());

	g_VGuiSurface->DrawSetTextFont(Font);
	g_VGuiSurface->DrawSetTextColor(DrawColor);

	if (Center)
	{
		int TextWidth, TextHeight;
		Render::Get().TextSize(TextWidth, TextHeight, Text, Font);
		g_VGuiSurface->DrawSetTextPos(X - TextWidth / 2, Y);
	}
	else
		g_VGuiSurface->DrawSetTextPos(X, Y);
	g_VGuiSurface->DrawPrintText(WText.c_str(), wcslen(WText.c_str()));
}
void Render::TextSize(int& Width, int& Height, const char* Text, vgui::HFont Font)
{
	std::wstring WText = std::wstring(std::string_view(Text).begin(), std::string_view(Text).end());
	g_VGuiSurface->GetTextSize(Font, WText.c_str(), Width, Height);
}
void Render::FilledRectange(int X1, int Y1, int X2, int Y2, Color DrawColor)
{
	g_VGuiSurface->DrawSetColor(DrawColor);
	g_VGuiSurface->DrawFilledRect(X1, Y1, X2, Y2);
}
void Render::OutlinedRectange(int X1, int Y1, int X2, int Y2, Color DrawColor)
{
	g_VGuiSurface->DrawSetColor(DrawColor);
	g_VGuiSurface->DrawOutlinedRect(X1, Y1, X2, Y2);
}
void Render::Line(int X1, int Y1, int X2, int Y2, Color DrawColor)
{
	g_VGuiSurface->DrawSetColor(DrawColor);
	g_VGuiSurface->DrawLine(X1, Y1, X2, Y2);
}
Chams::Chams()
{
	std::ofstream("csgo\\materials\\reflective.vmt") << (R"#("VertexLitGeneric" {
	  "$basetexture" "vgui/white"
      "$envmap" "env_cubemap"
      "$normalmapalphaenvmapmask" "1"
      "$envmapcontrast"  "1"
      "$nofog" "1"
      "$model" "1"
      "$nocull" "0"
      "$selfillum" "1"
      "$halflambert" "1"
      "$znearer" "0"
      "$flat" "1" 
}
)#");
	materialMetallic = g_MatSystem->FindMaterial("reflective", TEXTURE_GROUP_MODEL);
}
Chams::~Chams()
{
	std::remove("csgo\\materials\\material_reflective.vmt");
}
void Chams::OnDrawModelExecute(IMatRenderContext* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& info, matrix3x4_t* matrix)
{
	static auto fnDME = Hooks::mdlrender_hook.get_original<decltype(&Hooks::hkDrawModelExecute)>(index::DrawModelExecute);
	const auto mdl = info.pModel;

	if (g_MdlRender->IsForcedMaterialOverride()) return fnDME(g_MdlRender, 0, ctx, state, info, matrix);
	bool is_player = strstr(mdl->szName, "models/player") != nullptr;

	static IMaterial* normal = nullptr;
	static IMaterial* znormal = nullptr;
	static IMaterial* backtrack = nullptr;
	static IMaterial* zbacktrack = nullptr;

	// this could be a if statement but i'm fucking retarded :/
	switch (Variables.VisualsChamsMaterial)
	{
	case 0:
		normal = materialMetallic;
		znormal = materialMetallicZ;
		break;
	}
	switch (Variables.VisualsChamsBacktrackMaterial)
	{
	case 0:
		backtrack = materialMetallic;
		zbacktrack = materialMetallicZ;
		break;
	}
	float color[3] = 
	{
		Variables.VisualsChamsColor[0],
		Variables.VisualsChamsColor[1],
		Variables.VisualsChamsColor[2] 
	};
	float backtrackcolor[3] = 
	{
		Variables.VisualsChamsBacktrackColor[0],
		Variables.VisualsChamsBacktrackColor[1],
		Variables.VisualsChamsBacktrackColor[2] 
	};

	if (is_player)
	{
		C_BasePlayer* entity = C_BasePlayer::GetPlayerByIndex(info.entity_index);

		if (g_LocalPlayer && entity && entity->IsAlive() && !entity->IsDormant())
		{
			if (entity->IsEnemy() && Variables.VisualsChamsEnabled)
			{
				if (Variables.RageAimbotEnabled && RageAimbot::Get().BacktrackRecords[info.entity_index].size() > 0)
				{
					switch (Variables.VisualsChamsBacktrack)
					{
					case 1:
						for (int t = 0; t < RageAimbot::Get().BacktrackRecords[info.entity_index].size(); t++)
						{
							if (!RageAimbot::Get().BacktrackRecords[info.entity_index].at(t).MatrixBuilt
								|| !RageAimbot::Get().BacktrackRecords[info.entity_index].at(t).BoneMatrix)
								continue;
							g_RenderView->SetColorModulation(backtrackcolor);
							g_RenderView->SetBlend(float(Variables.VisualsChamsBacktrackAlpha) / 255.f);
							fnDME(g_MdlRender, 0, ctx, state, info, RageAimbot::Get().BacktrackRecords[info.entity_index].at(t).BoneMatrix);
						}
						break;
					case 2:
						if (RageAimbot::Get().BacktrackRecords[info.entity_index].back().MatrixBuilt
							&& RageAimbot::Get().BacktrackRecords[info.entity_index].back().BoneMatrix)
						{
							g_RenderView->SetColorModulation(backtrackcolor);
							g_RenderView->SetBlend(float(Variables.VisualsChamsBacktrackAlpha) / 255.f);
							fnDME(g_MdlRender, 0, ctx, state, info, RageAimbot::Get().BacktrackRecords[info.entity_index].back().BoneMatrix);
						}
						break;
					}
				}
				else if (Variables.LegitBacktrackEnabled && LegitBacktrack::Get().BacktrackRecords[info.entity_index].size() > 0)
				{
					switch (Variables.VisualsChamsBacktrack)
					{
					case 1:
						for (int t = 0; t < LegitBacktrack::Get().BacktrackRecords[info.entity_index].size(); t++)
						{
							if (!LegitBacktrack::Get().BacktrackRecords[info.entity_index].at(t).MatrixBuilt
								|| !LegitBacktrack::Get().BacktrackRecords[info.entity_index].at(t).BoneMatrix)
								continue;
							g_RenderView->SetColorModulation(backtrackcolor);
							g_RenderView->SetBlend(float(Variables.VisualsChamsBacktrackAlpha) / 255.f);
							fnDME(g_MdlRender, 0, ctx, state, info, LegitBacktrack::Get().BacktrackRecords[info.entity_index].at(t).BoneMatrix);
						}
						break;
					case 2:
						if (LegitBacktrack::Get().BacktrackRecords[info.entity_index].back().MatrixBuilt
							&& LegitBacktrack::Get().BacktrackRecords[info.entity_index].back().BoneMatrix)
						{
							g_RenderView->SetColorModulation(backtrackcolor);
							g_RenderView->SetBlend(float(Variables.VisualsChamsBacktrackAlpha) / 255.f);
							fnDME(g_MdlRender, 0, ctx, state, info, LegitBacktrack::Get().BacktrackRecords[info.entity_index].back().BoneMatrix);
						}
						break;
					}
				}
				g_RenderView->SetColorModulation(color);
				g_RenderView->SetBlend(float(Variables.VisualsChamsAlpha) / 255.f);
				g_MdlRender->ForcedMaterialOverride(normal);
				fnDME(g_MdlRender, 0, ctx, state, info, matrix);
			}
		}
	}
	fnDME(g_MdlRender, 0, ctx, state, info, matrix);
	g_MdlRender->ForcedMaterialOverride(nullptr);
}

Glow::Glow()
{
}

Glow::~Glow()
{
	// We cannot call shutdown here unfortunately.
	// Reason is not very straightforward but anyways:
	// - This destructor will be called when the dll unloads
	//   but it cannot distinguish between manual unload 
	//   (pressing the Unload button or calling FreeLibrary)
	//   or unload due to game exit.
	//   What that means is that this destructor will be called
	//   when the game exits.
	// - When the game is exiting, other dlls might already 
	//   have been unloaded before us, so it is not safe to 
	//   access intermodular variables or functions.
	//   
	//   Trying to call Shutdown here will crash CSGO when it is
	//   exiting (because we try to access g_GlowObjManager).
	//
}

void Glow::Shutdown()
{
	for (int i = 0; i < g_GlowObjManager->m_GlowObjectDefinitions.Count(); i++) 
	{
		GlowObjectDefinition_t & glowObject = g_GlowObjManager->m_GlowObjectDefinitions[i];
		C_BasePlayer* entity = reinterpret_cast<C_BasePlayer*>(glowObject.m_pEntity);

		if (glowObject.IsUnused())
			continue;

		if (!entity || entity->IsDormant())
			continue;

		glowObject.m_flAlpha = 0.0f;
	}
}

void Glow::Run()
{
	if (Variables.VisualsGlowEnabled)
	{
		for (int i = 0; i < g_GlowObjManager->m_GlowObjectDefinitions.Count(); i++) 
		{
			GlowObjectDefinition_t & glowObject = g_GlowObjManager->m_GlowObjectDefinitions[i];
			C_BasePlayer* entity = reinterpret_cast<C_BasePlayer*>(glowObject.m_pEntity);

			if (glowObject.IsUnused())
				continue;

			if (!entity || entity->IsDormant())
				continue;

			ClassId class_id = entity->GetClientClass()->m_ClassID;
			Color color = Color{};

			switch (class_id) 
			{
			case ClassId_CCSPlayer:

				if (!entity->IsAlive() || !entity->IsEnemy())
					continue;

				color = Color(
					int(Variables.VisualsGlowColor[0] * 255),
					int(Variables.VisualsGlowColor[1] * 255),
					int(Variables.VisualsGlowColor[2] * 255),
					int(Variables.VisualsGlowAlpha));
				break;
			}

			glowObject.m_flRed = color.r() / 255.0f;
			glowObject.m_flGreen = color.g() / 255.0f;
			glowObject.m_flBlue = color.b() / 255.0f;
			glowObject.m_flAlpha = color.a() / 255.0f;
			glowObject.m_bRenderWhenOccluded = true;
			glowObject.m_bRenderWhenUnoccluded = false;
			glowObject.m_nGlowStyle = Variables.VisualsGlowGlowstyle;
		}
	}
}

RECT Visuals::GetBBox(C_BasePlayer* Player, Vector TransformedPoints[])
{
	RECT rect{};
	ICollideable* collideable = Player->GetCollideable();

	if (!collideable)
		return rect;

	Vector min = collideable->OBBMins();
	Vector max = collideable->OBBMaxs();

	const matrix3x4_t& trans = Player->m_rgflCoordinateFrame();

	Vector points[] =
	{
		Vector(min.x, min.y, min.z),
		Vector(min.x, max.y, min.z),
		Vector(max.x, max.y, min.z),
		Vector(max.x, min.y, min.z),
		Vector(max.x, max.y, max.z),
		Vector(min.x, max.y, max.z),
		Vector(min.x, min.y, max.z),
		Vector(max.x, min.y, max.z)
	};

	Vector pointsTransformed[8];

	for (int i = 0; i < 8; i++) {
		Math::VectorTransform(points[i], trans, pointsTransformed[i]);
	}

	Vector pos = Player->GetAbsOrigin();
	Vector screen_points[8] = {};

	for (int i = 0; i < 8; i++)
		if (!Math::WorldToScreen(pointsTransformed[i], screen_points[i]))
			return rect;
		else
			TransformedPoints[i] = screen_points[i];

	float left = screen_points[0].x;
	float top = screen_points[0].y;
	float right = screen_points[0].x;
	float bottom = screen_points[0].y;

	for (int i = 1; i < 8; i++)
	{
		if (left > screen_points[i].x)
			left = screen_points[i].x;
		if (top < screen_points[i].y)
			top = screen_points[i].y;
		if (right < screen_points[i].x)
			right = screen_points[i].x;
		if (bottom > screen_points[i].y)
			bottom = screen_points[i].y;
	}
	return RECT{ (long)left, (long)top, (long)right, (long)bottom };
}
bool Visuals::Begin(C_BasePlayer* Player)
{
	Context.Player = Player;

	if (!Context.Player->IsEnemy())
		return false;

	if (Context.Player->IsDormant())
		return false;

	Vector head = Context.Player->GetHitboxPos(HITBOX_HEAD);
	Vector origin = Context.Player->GetAbsOrigin();

	head.z += 15;

	if (!Math::WorldToScreen(head, Context.HeadPos) || !Math::WorldToScreen(origin, Context.Origin))
		return false;

	Vector points_transformed[8];
	RECT Box = GetBBox(Context.Player, points_transformed);

	Context.Box = Box;
	Context.Box.top = Box.bottom;
	Context.Box.bottom = Box.top;

	return true;
}

void Visuals::Box()
{
	Render::Get().OutlinedRectange(Context.Box.left - 1, Context.Box.top - 1, Context.Box.right + 1, Context.Box.bottom + 1, Color::Black);
	Render::Get().OutlinedRectange(Context.Box.left + 1, Context.Box.top + 1, Context.Box.right - 1, Context.Box.bottom - 1, Color::Black);
	Render::Get().OutlinedRectange(Context.Box.left, Context.Box.top, Context.Box.right, Context.Box.bottom, Color::White);
}
void Visuals::Name()
{
	player_info_t PlayerInfo;
	g_EngineClient->GetPlayerInfo(Context.Player->EntIndex(), &PlayerInfo);

	int TextWidth, TextHeight;
	Render::Get().TextSize(TextWidth, TextHeight, PlayerInfo.szName, Render::Get().Visuals);
	Render::Get().Text(Context.Box.left + (Context.Box.right - Context.Box.left) / 2, Context.Box.top - TextHeight, PlayerInfo.szName, Render::Get().Visuals, Color(255, 255, 255, 255), true);
}
void Visuals::Weapon()
{
	C_BaseCombatWeapon* Weapon = Context.Player->m_hActiveWeapon();
	if (!Weapon) return;

	std::string WeaponName = std::string(Weapon->GetCSWeaponData()->szHudName + std::string("(") + std::to_string(Weapon->m_iClip1()) + std::string("/") + std::to_string(Weapon->m_iPrimaryReserveAmmoCount()) + std::string(")"));
	WeaponName.erase(0, 13);
	int TextWidth, TextHeight;

	Render::Get().TextSize(TextWidth, TextHeight, WeaponName.c_str(), Render::Get().Visuals);
	Render::Get().Text(Context.Box.left + (Context.Box.right - Context.Box.left) / 2, Context.Box.bottom - 1, WeaponName.c_str(), Render::Get().Visuals, Color(255, 255, 255, 255), true);
}
void Visuals::Health()
{
	int HealthValue = Context.Player->m_iHealth();
	std::clamp(HealthValue, 0, 100);

	float Height = (Context.Box.bottom - Context.Box.top) * float(HealthValue / 100.f);

	Render::Get().FilledRectange(Context.Box.left - 7, Context.Box.top - 1, Context.Box.left - 2, Context.Box.bottom + 1, Color(0, 0, 0, 150));
	Render::Get().FilledRectange(Context.Box.left - 6, Context.Box.top, Context.Box.left - 3, Context.Box.top + Height, Color(0, 220, 50, 255));
}