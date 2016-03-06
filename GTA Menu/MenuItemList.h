#pragma once
#include "MenuItemObject.h"
class MenuItemList : public MenuItemObject
{
public:
	MenuItemList(void);
	MenuItemList(std::string name, void (*action)(Client* client, void* param), std::vector<std::string>& items, bool callbyIndex);
	~MenuItemList(void);
	void OnScrollLeft();
	void OnScrollRight();
	void OnClick(Client* client, void* param);
	void Render();
private:
	std::vector<std::string> m_items;
	std::vector<std::string>::iterator m_currentListItem;
	bool m_callByIndex;
};
