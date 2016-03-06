#include "stdafx.h"
#include "HudElem.h"
#include "inc\natives.h"

void HudText::SetText(std::string text, hudelem_color_t color, float x, float y, float fontScale, uint32_t font)
{
	m_text = text;
	m_color = color;
	m_x = x;
	m_y = y;
	m_fontScale = fontScale;
	m_font = font;
}

void HudText::Render()
{
	int screen_w, screen_h;
	GRAPHICS::GET_SCREEN_RESOLUTION(&screen_w, &screen_h);

	UI::SET_TEXT_FONT(m_font);
	UI::SET_TEXT_SCALE(0.0, m_fontScale);
	UI::SET_TEXT_COLOUR(m_color.r, m_color.g, m_color.b, m_color.a);
	UI::SET_TEXT_CENTRE(0);
	UI::SET_TEXT_DROPSHADOW(0, 0, 0, 0, 0);
	UI::SET_TEXT_EDGE(0, 0, 0, 0, 0);
	UI::_SET_TEXT_ENTRY("STRING");
	UI::_ADD_TEXT_COMPONENT_STRING((LPSTR)m_text.c_str());
	UI::_DRAW_TEXT(m_x, m_y);
}

void HudMaterial::Render()
{
	GRAPHICS::DRAW_RECT(m_x, m_y, m_width, m_height, m_color.r, m_color.g, m_color.b, m_color.a);
}

void HudMaterial::SetMaterial(float x, float y, float width, float height, hudelem_color_t color)
{
	m_width = width;
	m_height = height;
	m_x = x;
	m_y = y;
	m_color = color;
}
