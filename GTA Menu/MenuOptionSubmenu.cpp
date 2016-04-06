#include "stdafx.h"
#include "MenuOptionSubmenu.h"
#include "Menu.h"
#include "Client.h"

MenuOptionSubmenu::MenuOptionSubmenu(std::string name)
{
	Text = name;
}

void MenuOptionSubmenu::Render()
{
//	Elem.SetText(Text, hudelem_color_t(0xFFFFFFFF), MENU_X_POSITION, MENU_Y_POSITION + ((float)Position * MENU_ITEM_DIFFERENCE), DEFAULT_OPTION_SCALE, 4);
//	Elem.Render();
}

void MenuOptionSubmenu::Call()
{
	if (Parent && Parent->Owner)
	{
		if (Parent->Owner->PreviousMenu && Parent->Owner->PreviousMenu != Parent->Owner->BaseMenu)
		{
			delete Parent->Owner->PreviousMenu;
		}
		Parent->Owner->PreviousMenu = Parent->Owner->CurrentMenu;
		//Parent->Owner->CurrentMenu = new UIMenu(Text, Parent->Owner);
	}
	if (Callback)
		Callback(this);
}

MenuOptionSubmenu::~MenuOptionSubmenu()
{
}