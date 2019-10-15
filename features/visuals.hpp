#pragma once
#include "../options.hpp"
#include "../valve_sdk/csgostructs.hpp"
#include "../helpers/math.hpp"
#include "../helpers/Utilities.hpp"
#include "../singleton.hpp"


class Render : public Singleton<Render>{
	friend class Singleton<Render>;
private:
public:
	vgui::HFont Watermark;
	vgui::HFont Visuals;

	void CreateFonts();
	void Text(int X, int Y, const char* Text, vgui::HFont Font, Color DrawColor, bool Center);
	void TextSize(int& Width, int& Height, const char* Text, vgui::HFont Font);
	void FilledRectange(int X1, int Y1, int X2, int Y2, Color DrawColor);
	void OutlinedRectange(int X1, int Y1, int X2, int Y2, Color DrawColor);
	void Line(int X1, int Y1, int X2, int Y2, Color DrawColor);
};

class Visuals : public Singleton<Visuals>{
	friend class Singleton<Visuals>;
private:
	RECT GetBBox(C_BasePlayer* Player, Vector TransformedPoints[]);
	struct
	{
		C_BasePlayer* Player;
		RECT Box;
		Vector HeadPos;
		Vector Origin;
	} Context;
public:
	bool Begin(C_BasePlayer* Player);

	void Health();
	void Name();
	void Box();
	void Radar();
	void Recoil();
};