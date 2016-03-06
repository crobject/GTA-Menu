#pragma once
#include <string>
#include <stdint.h>

union hudelem_color_t
{
	hudelem_color_t(char r, char g, char b, char a)
	{
		this->r = r;
		this->g = g;
		this->b = b;
		this->a = a;
	}
	hudelem_color_t(int rgba)
	{
		this->rgba = rgba;
	}
	hudelem_color_t() {}
	struct
	{
		char r;
		char g;
		char b;
		char a;
	};
	int rgba;
};

class HudText
{
public:
	HudText() {}
	void Render();
	void SetText(std::string text, hudelem_color_t color, float x, float y, float fontScale, uint32_t font);
	std::string m_text;
	hudelem_color_t m_color;
	uint32_t m_moveStartTime;
	uint32_t m_moveEndTime;
	float m_x;
	float m_y;
	float m_fontScale;
	uint32_t m_font;
};

class HudMaterial
{
public:
	HudMaterial() {}
	void Render();
	void SetMaterial(float x, float y, float width, float height, hudelem_color_t color);
	hudelem_color_t m_color;
	uint32_t m_moveStartTime;
	uint32_t m_moveEndTime;
	float m_width;
	float m_height;
	float m_x;
	float m_y;
};
