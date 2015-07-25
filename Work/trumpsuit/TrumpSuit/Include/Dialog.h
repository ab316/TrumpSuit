#pragma once

#include "DXUIManager.h"

using namespace DXFramework;

#define GAME_UI_START_STATIC_WELCOME_ID			0
#define GAME_UI_START_STATIC_TRUMP_ID			1
#define GAME_UI_START_STATIC_NUMPLAYERS_ID		2
#define GAME_UI_START_STATIC_2PLAYERS_ID		3
#define GAME_UI_START_STATIC_3PLAYERS_ID		4
#define GAME_UI_START_STATIC_4PLAYERS_ID		5
#define GAME_UI_START_BUTTON_START_ID			6
#define GAME_UI_START_EDIT_NAME_ID				7
#define GAME_UI_START_CHECK_TRUMP_ID			8
#define GAME_UI_START_RADIO_TWO_PLAYERS			9
#define GAME_UI_START_RADIO_THREE_PLAYERS		10
#define GAME_UI_START_RADIO_FOUR_PLAYERS		11
#define GAME_UI_START_BUTTON_LOAD_ID			12

class IDialog
{
protected:
	std::vector<IDXUIElement*>		m_vecpElements;

protected:
	void	AddElement(IDXUIElement* pElement);
	void	RemoveElement(IDXUIElement* pElement);

public:
	void	Show(bool bShow);

public:
	virtual void	OnResize(float fWidth, float fHeight)		{ fWidth; fHeight; };
	virtual	bool	OnWindowMsg(UINT uiControl, DX_UI_MESSAGE msg)	=	0;

public:
	IDialog();
	~IDialog();
};