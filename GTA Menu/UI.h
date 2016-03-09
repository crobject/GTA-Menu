#include <string>
#include "inc\types.h"
#include <vector>

union Color_t
{
	Color_t(char r, char g, char b, char a)
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
		char r;
		char g;
		char b;
		char a;
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

private:
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

	bool Enabled;
	Point Position;
	Color_t Color;
};

class UIText : public UIElement
{
public:
	UIText();
	UIText(std::string caption, Point position, float scale);
	UIText(std::string caption, Point position, float scale, Color_t color);
	UIText(std::string caption, Point position, float scale, Color_t color, Font_t font, bool centered);
	UIText(std::string caption, Point position, float scale, Color_t color, Font_t font, bool centered, bool shadow, bool outline);

	bool Enabled;
	Point Position;
	Color_t Color;
	std::string Caption;
	Font_t Font;
	float Scale;
	bool Centered;
	bool Shadow;
	bool Outline;

	void Draw();
	void Draw(Size_t offset);
};

class UIRectangle : public UIElement
{
public:
	UIRectangle();
	UIRectangle(Point position, Size_t size);
	UIRectangle(Point position, Size_t size, Color_t color);

	bool Enabled;
	Point Position;
	Size_t Size;
	Color_t Color;

	void Draw();
	void Draw(Size_t offset);
};

class UIContainer : public UIRectangle
{
public:
	UIContainer();
	UIContainer(Point position, Size_t size);
	UIContainer(Point position, Size_t size, Color_t color);

	std::vector<UIElement*> Items;

	void AddItem(UIElement* elem);
	void Draw();
	void Draw(Size_t offset);
};

class UISprite : public UIElement
{
public:
	UISprite(std::string textureDict, std::string textureName, Size_t scale, Point position);
	UISprite(std::string textureDict, std::string textureName, Size_t scale, Point position, Color_t color);
	UISprite(std::string textureDict, std::string textureName, Size_t scale, Point position, Color_t color, float rotation);

	~UISprite();

	bool Enabled;
	Point Position;
	Color_t Color;
	Size_t Scale;
	float Rotation;

	void Draw();
	void Draw(Size_t offset);

private:
	std::string _textureDict;
	std::string _textureName;
};
