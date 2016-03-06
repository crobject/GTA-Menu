#pragma once
#include "stdafx.h"
#include "Menu.h"
#include "Client.h"
const float ITEM_DIFFERENCE = 15.0f;
class MenuItemObject
{
public:
	MenuItemObject(void);
	MenuItemObject(std::string name, void (*action)(Client* client, void* param));
	virtual ~MenuItemObject(void);
	virtual void OnHoverEnter() {};
	virtual void OnHoverLeave() {};
	virtual void OnClick(Client* client, void* param) = NULL;
	virtual void Render();
	virtual void OnScrollRight() {}
	virtual void OnScrollLeft() {}
	virtual std::string GetName();
	virtual void FreeHuds();
	void SetParent(Menu* parent);
	HudElem* GetElem();
	int GetItemPosition() const;
protected:
	std::string m_name;
	void (*m_action)(Client* client, void* param);
	Menu* m_parent;
	HudElem* m_elem;
};