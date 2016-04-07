#include "stdafx.h"
#include "UI.h"
#include "inc\natives.h"

Notification::Notification(int handle) : _handle(handle)
{
}

void Notification::Hide()
{
	UI::_REMOVE_NOTIFICATION(_handle);
}

Notification GTAUI::Notify(std::string message)
{
	return Notify(message, false);
}
Notification GTAUI::Notify(std::string message, bool blinking)
{
	UI::_SET_NOTIFICATION_TEXT_ENTRY("STRING");
	UI::_ADD_TEXT_COMPONENT_STRING((char*)message.c_str());

	return Notification(UI::_DRAW_NOTIFICATION(blinking, 1));
}

void GTAUI::ShowSubtitle(std::string message)
{
	ShowSubtitle(message, 2500);
}
void GTAUI::ShowSubtitle(std::string message, int duration)
{
	UI::_SET_TEXT_ENTRY_2("STRING");
	UI::_ADD_TEXT_COMPONENT_STRING((char*)message.c_str());
	UI::_DRAW_SUBTITLE_TIMED(duration, 1);
}

bool GTAUI::IsHudComponentActive(HudComponent component)
{
	return UI::IS_HUD_COMPONENT_ACTIVE(static_cast<int>(component));
}
void GTAUI::ShowHudComponentThisFrame(HudComponent component)
{
	UI::SHOW_HUD_COMPONENT_THIS_FRAME(static_cast<int>(component));
}
void GTAUI::HideHudComponentThisFrame(HudComponent component)
{
	UI::HIDE_HUD_COMPONENT_THIS_FRAME(static_cast<int>(component));
}

Point GTAUI::WorldToScreen(Vector3 position)
{
	float pointX, pointY;

	if (!GRAPHICS::_WORLD3D_TO_SCREEN2D(position.x, position.y, position.z, &pointX, &pointY))
	{
		return Point(0, 0);
	}

	return Point(static_cast<int>(pointX * GTAUI::WIDTH), static_cast<int>(pointY * GTAUI::HEIGHT));
}

UIText::UIText()
{
}
UIText::UIText(std::string caption, Point position, float scale)
{
	Enabled = true;
	Text = caption;
	Position = position;
	Scale = scale;
	Color = Color_t(20, 20, 20, 200);
	Font = Font_t::ChaletLondon;
	Centered = false;
	Shadow = false;
	Outline = false;
}

UIText::UIText(std::string caption, Point position, float scale, Color_t color)
{
	Enabled = true;
	Text = caption;
	Position = position;
	Scale = scale;
	Color = color;
	Font = Font_t::ChaletLondon;
	Centered = false;
	Shadow = false;
	Outline = false;
}

UIText::UIText(std::string caption, Point position, float scale, Color_t color, Font_t font, bool centered)
{
	Enabled = true;
	Text = caption;
	Position = position;
	Scale = scale;
	Color = color;
	Font = font;
	Centered = centered;
	Shadow = false;
	Outline = false;
}

UIText::UIText(std::string caption, Point position, float scale, Color_t color, Font_t font, bool centered, bool shadow, bool outline)
{
	Enabled = true;
	Text = caption;
	Position = position;
	Scale = scale;
	Color = color;
	Font = font;
	Centered = centered;
	Shadow = shadow;
	Outline = outline;
}

void UIText::Draw()
{
	Draw(Size_t(0, 0));
}

void UIText::Draw(Size_t offset)
{
	if (!Enabled)
	{
		return;
	}

	const float x = (static_cast<float>(Position.m_x) + offset.m_width) / GTAUI::WIDTH;
	const float y = (static_cast<float>(Position.m_y) + offset.m_height) / GTAUI::HEIGHT;

	if (Shadow)
	{
		UI::SET_TEXT_DROP_SHADOW();
	}
	if (Outline)
	{
		UI::SET_TEXT_OUTLINE();
	}
	UI::SET_TEXT_FONT(static_cast<int>(Font));
	UI::SET_TEXT_SCALE(Scale, Scale);
	UI::SET_TEXT_COLOUR(Color.r, Color.g, Color.b, Color.a);
	UI::SET_TEXT_CENTRE(Centered ? 1 : 0);
	UI::_SET_TEXT_ENTRY("STRING");
	UI::_ADD_TEXT_COMPONENT_STRING((char*)GetText().c_str());
	UI::_DRAW_TEXT(x, y);
}

UIRectangle::UIRectangle()
{
	Enabled = true;
	Position = Point();
	Size = Size_t(GTAUI::WIDTH, GTAUI::HEIGHT);
	Color = Color_t(0, 0, 0, 0);
}
UIRectangle::UIRectangle(Point position, Size_t size)
{
	Enabled = true;
	Position = position;
	Size = size;
	Color = Color_t(0, 0, 0, 0);
}
UIRectangle::UIRectangle(Point position, Size_t size, Color_t color)
{
	Enabled = true;
	Position = position;
	Size = size;
	Color = color;
}

void UIRectangle::Draw()
{
	Draw(Size_t());
}
void UIRectangle::Draw(Size_t offset)
{
	if (!Enabled)
	{
		return;
	}

	const float w = static_cast<float>(Size.m_width) / GTAUI::WIDTH;
	const float h = static_cast<float>(Size.m_height) / GTAUI::HEIGHT;
	const float x = ((static_cast<float>(Position.m_x) + offset.m_width) / GTAUI::WIDTH) + w * 0.5f;
	const float y = ((static_cast<float>(Position.m_y) + offset.m_height) / GTAUI::HEIGHT) + h * 0.5f;

	GRAPHICS::DRAW_RECT(x, y, w, h, Color.r, Color.g, Color.b, Color.a);
}

UIContainer::UIContainer() : UIRectangle()
{
}
UIContainer::UIContainer(Point position, Size_t size) : UIRectangle(position, size)
{
}
UIContainer::UIContainer(Point position, Size_t size, Color_t color) : UIRectangle(position, size, color)
{
}

void UIContainer::Draw()
{
	Draw(Size_t());
}
void UIContainer::Draw(Size_t offset)
{
	if (!Enabled)
	{
		return;
	}

	UIRectangle::Draw(offset);

	for each (auto item in Items)
	{
		auto point = offset;
		item->Draw();
	}
}
void UIContainer::AddItem(UIElement* elem)
{
	Items.push_back(elem);
}
UISprite::UISprite(std::string textureDict, std::string textureName, Size_t scale, Point position)
{
	Enabled = true;
	_textureDict = textureDict;
	_textureName = textureName;
	Scale = scale;
	Position = position;
	Color = Color_t(0, 0, 0, 0);
	Rotation = 0.0F;
	GRAPHICS::REQUEST_STREAMED_TEXTURE_DICT((char*)_textureDict.c_str(), FALSE);
}
UISprite::UISprite(std::string textureDict, std::string textureName, Size_t scale, Point position, Color_t color)
{
	Enabled = true;
	_textureDict = textureDict;
	_textureName = textureName;
	Scale = scale;
	Position = position;
	Color = color;
	Rotation = 0.0F;
	GRAPHICS::REQUEST_STREAMED_TEXTURE_DICT((char*)_textureDict.c_str(), FALSE);
}
UISprite::UISprite(std::string textureDict, std::string textureName, Size_t scale, Point position, Color_t color, float rotation)
{
	Enabled = true;
	_textureDict = textureDict;
	_textureName = textureName;
	Scale = scale;
	Position = position;
	Color = color;
	Rotation = rotation;
	GRAPHICS::REQUEST_STREAMED_TEXTURE_DICT((char*)_textureDict.c_str(), FALSE);
}
UISprite::~UISprite()
{
	GRAPHICS::SET_STREAMED_TEXTURE_DICT_AS_NO_LONGER_NEEDED((char*)_textureDict.c_str());
}

void UISprite::Draw()
{
	Draw(Size_t());
}
void UISprite::Draw(Size_t offset)
{
	if (!Enabled || !GRAPHICS::HAS_STREAMED_TEXTURE_DICT_LOADED((char*)_textureDict.c_str()))
	{
		return;
	}

	const float scaleX = static_cast<float>(Scale.m_width) / GTAUI::WIDTH;
	const float scaleY = static_cast<float>(Scale.m_height) / GTAUI::HEIGHT;
	const float positionX = ((static_cast<float>(Position.m_x) + offset.m_width) / GTAUI::WIDTH) + scaleX * 0.5f;
	const float positionY = ((static_cast<float>(Position.m_y) + offset.m_height) / GTAUI::HEIGHT) + scaleY * 0.5f;

	GRAPHICS::DRAW_SPRITE((char*)_textureDict.c_str(), (char*)_textureName.c_str(), positionX, positionY, scaleX, scaleY, Rotation, Color.r, Color.g, Color.b, Color.a);
}