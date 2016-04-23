#include "stdafx.h"
#include "UI.h"
#include "inc\natives.h"
#include <regex>
#include "UIItem.h"

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

UIContainer::UIContainer()
{
}
UIContainer::UIContainer(Point position, Size_t size)
{
}
UIContainer::UIContainer(Point position, Size_t size, Color_t color, std::string spriteDict, std::string spriteName, uint32_t pageSize) : UISprite("commonmenu", "gradient_bgd", size, position, color), 
	m_pageSize(pageSize)
{
	auto s = size;
	s.m_height = 80;	
	m_logo = UISprite(spriteDict, spriteName, s, position, Color_t(255,255,255,255));
	auto p = position;
	p.m_y += s.m_height;
	m_descrptionBar = UISprite("commonmenu", "gradient_nav", Size_t(size.m_width, 30), p, Color_t(0,0,0,255));
	p.m_y += m_descrptionBar.GetSize().m_height + Size.m_height + 10;
	m_itemDescrptionBar = UISprite("commonmenu", "gradient_nav", Size_t(size.m_width, 30), p, Color_t(0, 0, 0, 255));
}


void UIContainer::Draw(const std::vector<UIItem*>::iterator& currentItem, std::string filter)
{
	if (!Enabled)
	{
		return;
	}
	m_logo.Draw();
	m_descrptionBar.Draw();
	m_itemDescrptionBar.Draw();
	UIText((*currentItem)->GetDescription(), m_itemDescrptionBar.GetPosition(), DEFAULT_FONT_SCALE, Color_t(255,255,255,255)).Draw();
	auto sizeOffset = m_logo.GetSize().m_height + m_descrptionBar.GetSize().m_height;
	Size.m_height = (min(15, Items.size()) * 30) + sizeOffset;
	m_itemDescrptionBar.SetPosition(Point(Position.m_x, sizeOffset + Size.m_height + 10));
	UISprite::Draw(Size_t());
	if (!Items.size())
		return;
	auto dist = std::distance(Items.begin(), currentItem);
	if (dist > m_pageSize - 1)
	{
		auto begin = Items.begin() + (dist - m_pageSize) + 1;
		auto i = begin;
		uint32_t screenindex = 0;
		auto pageEnd = min(m_pageSize, std::distance(begin, Items.end()));
		while (i != Items.end() && i < begin + pageEnd)
		{
			auto pos = Point(Position.m_x, Position.m_y + sizeOffset + (screenindex * 30));
			//TODO: pass a value for the difference
			(*i)->SetPosition(pos);
			(*i)->Draw();
			screenindex++;
			i++;
		}
	}
	else
	{
		uint32_t screenindex = 0;
		auto begin = Items.begin();
		auto i = begin;
		auto pageEnd = min(m_pageSize, std::distance(begin, Items.end()));
		while (i != Items.end() && i < begin + pageEnd)
		{
			auto item_name = std::regex(filter);
			if (filter != "" && !std::regex_match((*i)->GetText(), item_name))
			{
				i++;
				continue;
			}
			auto pos = Point(Position.m_x, Position.m_y + sizeOffset + (screenindex * 30));
			(*i)->SetPosition(pos);
			(*i)->Draw();
			screenindex++;
			i++;
		}
	}
}
void UIContainer::AddItem(UIItem* elem)
{
	Items.push_back(elem);
}
UISprite::UISprite() {}
UISprite::UISprite(std::string textureDict, std::string textureName, Size_t scale, Point position)
{
	Enabled = true;
	_textureDict = textureDict;
	_textureName = textureName;
	Size = scale;
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
	Size = scale;
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
	Size = scale;
	Position = position;
	Color = color;
	Rotation = rotation;
	GRAPHICS::REQUEST_STREAMED_TEXTURE_DICT((char*)_textureDict.c_str(), FALSE);
}
UISprite::~UISprite()
{
	//GRAPHICS::SET_STREAMED_TEXTURE_DICT_AS_NO_LONGER_NEEDED((char*)_textureDict.c_str());
}
void UISprite::SetTextureName(std::string val)
{
	_textureName = val;
	GRAPHICS::REQUEST_STREAMED_TEXTURE_DICT((char*)_textureDict.c_str(), FALSE);
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

	const float scaleX = static_cast<float>(Size.m_width) / GTAUI::WIDTH;
	const float scaleY = static_cast<float>(Size.m_height) / GTAUI::HEIGHT;
	const float positionX = ((static_cast<float>(Position.m_x) + offset.m_width) / GTAUI::WIDTH) + scaleX * 0.5f;
	const float positionY = ((static_cast<float>(Position.m_y) + offset.m_height) / GTAUI::HEIGHT) + scaleY * 0.5f;

	GRAPHICS::DRAW_SPRITE((char*)_textureDict.c_str(), (char*)_textureName.c_str(), positionX, positionY, scaleX, scaleY, Rotation, Color.r, Color.g, Color.b, Color.a);
}