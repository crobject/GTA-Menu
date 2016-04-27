#pragma once
#include <string>
#include "inc\types.h"
#include <vector>
class UIItem;
union Color_t
{
	Color_t(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
	{
		this->r = r;
		this->g = g;
		this->b = b;
		this->a = a;
	}
	Color_t(int rgba)
	{
		this->rgba = rgba;
	}
	Color_t() {}
	struct
	{
		unsigned char r;
		unsigned char g;
		unsigned char b;
		unsigned char a;
	};
	int rgba;
};

enum Font_t
{
	ChaletLondon = 0,
	HouseScript = 1,
	Monospace = 2,
	ChaletComprimeCologne = 4,
	Pricedown = 7
};

enum HudComponent
{
	WantedStars = 1,
	WeaponIcon = 2,
	Cash = 3,
	MpCash = 4,
	MpMessage = 5,
	VehicleName = 6,
	AreaName = 7,
	Unused = 8,
	StreetName = 9,
	HelpText = 10,
	FloatingHelpText1 = 11,
	FloatingHelpText2 = 12,
	CashChange = 13,
	Reticle = 14,
	SubtitleText = 15,
	RadioStationsWheel = 16,
	Saving = 17,
	GamingStreamUnusde = 18,
	WeaponWheel = 19,
	WeaponWheelStats = 20,
	DrugsPurse01 = 21,
	DrugsPurse02 = 22,
	DrugsPurse03 = 23,
	DrugsPurse04 = 24,
	MpTagCashFromBank = 25,
	MpTagPackages = 26,
	MpTagCuffKeys = 27,
	MpTagDownloadData = 28,
	MpTagIfPedFollowing = 29,
	MpTagKeyCard = 30,
	MpTagRandomObject = 31,
	MpTagRemoteControl = 32,
	MpTagCashFromSafe = 33,
	MpTagWeaponsPackage = 34,
	MpTagKeys = 35,
	MpVehicle = 36,
	MpVehicleHeli = 37,
	MpVehiclePlane = 38,
	PlayerSwitchAlert = 39,
	MpRankBar = 40,
	DirectorMode = 41,
	ReplayController = 42,
	ReplayMouse = 43,
	ReplayHeader = 44,
	ReplayOptions = 45,
	ReplayHelpText = 46,
	ReplayMiscText = 47,
	ReplayTopLine = 48,
	ReplayBottomLine = 49,
	ReplayLeftBar = 50,
	ReplayTimer = 51
};

struct Point
{
	Point() { m_x = m_y = 0; }
	Point(int32_t x, int32_t y)
	{
		m_x = x;
		m_y = y;
	}
	int32_t m_x, m_y;
};

struct PointF
{
	PointF() { m_x = m_y = 0; }
	PointF(float x, float y)
	{
		m_x = x;
		m_y = y;
	}
	float m_x, m_y;
};

struct Size_t
{
	Size_t() { m_width = m_height = 0; }
	Size_t(uint32_t width, uint32_t height)
	{
		m_width = width;
		m_height = height;
	}
	uint32_t m_width, m_height;
};

class Notification
{
public:
	Notification() { _handle = 0; }
	void Hide();
	Notification(int handle);

protected:
	int _handle;
};

class GTAUI
{
public:
	GTAUI() {}
	~GTAUI() {}
	static const int WIDTH = 1280;
	static const int HEIGHT = 720;

	Notification Notify(std::string message);
	Notification Notify(std::string message, bool blinking);

	void ShowSubtitle(std::string message);
	void ShowSubtitle(std::string message, int duration);

	bool IsHudComponentActive(HudComponent component);
	void ShowHudComponentThisFrame(HudComponent component);
	void HideHudComponentThisFrame(HudComponent component);

	Point WorldToScreen(Vector3 position);
};

class UIElement
{
public:
	UIElement() {}
	virtual void Draw() = NULL;
	virtual void Draw(Size_t offset) = NULL;

	bool GetEnabled() { return Enabled; }
	void SetEnabled(bool val) { Enabled = val; }
	Point GetPosition() { return Position; }
	void SetPosition(Point val) { Position = val; }
	Color_t GetColor() { return Color; }
	void SetColor(Color_t val) { Color = val; }
	Size_t GetSize() { return Size; }
	void SetSize(Size_t val) { Size = val; }
protected:
	bool Enabled;
	Point Position;
	Color_t Color;
	Size_t Size;
};

class UIText : public UIElement
{
public:
	UIText();
	UIText(std::string caption, Point position, float scale);
	UIText(std::string caption, Point position, float scale, Color_t color);
	UIText(std::string caption, Point position, float scale, Color_t color, Font_t font, bool centered);
	UIText(std::string caption, Point position, float scale, Color_t color, Font_t font, bool centered, bool shadow, bool outline);
	virtual std::string GetText() { return Text; }
	void SetText(std::string val) { Text = val; }
	Font_t GetFont() { return Font; }
	void SetFont(Font_t val) { Font = val; }
	float GetScale() { return Scale; }
	void SetScale(float val) { Scale = val; }
	bool GetCentered() { return Centered; }
	void SetCentered(bool val) { Centered = val; }
	bool GetShadow() { return Shadow; }
	void SetShadow(bool val) { Shadow = val; }
	bool GetOutline() { return Outline; }
	void SetOutline(bool val) { Outline = val; }

	void Draw();
	void Draw(Size_t offset);
protected:
	std::string Text;

	Font_t Font;
	float Scale;
	bool Centered;
	bool Shadow;
	bool Outline;
};

class UIRectangle : public UIElement
{
public:
	UIRectangle();
	UIRectangle(Point position, Size_t size);
	UIRectangle(Point position, Size_t size, Color_t color);

	void Draw();
	void Draw(Size_t offset);
};

class UISprite : public UIElement
{
public:
	UISprite::UISprite();
	UISprite(std::string textureDict, std::string textureName, Size_t scale, Point position);
	UISprite(std::string textureDict, std::string textureName, Size_t scale, Point position, Color_t color);
	UISprite(std::string textureDict, std::string textureName, Size_t scale, Point position, Color_t color, float rotation);

	~UISprite();

	void SetRotation(float val) { Rotation = val; }
	float GetRotation() { return Rotation; }
	void Draw();
	void Draw(Size_t offset);
	
	std::string GetTextureName() { return _textureName; }
	void SetTextureName(std::string val);
protected:
	float Rotation;

	std::string _textureDict;
	std::string _textureName;
};

class UIContainer : public UISprite
{
public:
	UIContainer();
	UIContainer(Point position, Size_t size);
	UIContainer(Point position, Size_t size, Color_t color, std::string spriteDict = "commonmenu", std::string spriteName = "interaction_bgd", uint32_t pageSize = 15);
	void AddItem(UIItem* elem);
	virtual void Draw(const std::vector<UIItem*>::iterator& currentItem, std::string filter = "");
	std::vector<UIItem*>& GetItems();
	void SetFilter(std::string& str) { m_filter = str; }
	UISprite& GetLogo() { return m_logo; }
	UISprite& GetDescriptionBar() { return m_descrptionBar; }
	UISprite& GetItemDescriptionBar() { return m_itemDescrptionBar; }
protected:
	std::string m_filter;
	std::vector<UIItem*> m_items;
	uint32_t m_pageSize;
	UISprite m_logo;
	UISprite m_descrptionBar;
	UISprite m_itemDescrptionBar;
};