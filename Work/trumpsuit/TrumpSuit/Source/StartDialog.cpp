#include "stdafx.h"
#include "DXUIElement.h"
#include "DXUIManager.h"
#include "DXUIEditBox.h"
#include "DXUICheckBox.h"
#include "DXUIRadioButton.h"
#include "StartDialog.h"


using namespace DXFramework;


CStartDialog::CStartDialog()
{
	CDXUIManager* pMan = CDXUIManager::GetInstance();

	pMan->AddStatic(GAME_UI_START_STATIC_WELCOME_ID, L"Welcome to card game", 200, 40, 48.0f);
	pMan->AddStatic(GAME_UI_START_STATIC_TRUMP_ID, L"Trump Suit", 180, 225);
	pMan->AddStatic(GAME_UI_START_STATIC_NUMPLAYERS_ID, L"Number of players:", 180, 275);
	pMan->AddStatic(GAME_UI_START_STATIC_2PLAYERS_ID, L"2", 200, 315.0f);
	pMan->AddStatic(GAME_UI_START_STATIC_3PLAYERS_ID, L"3", 200, 345.0f);
	pMan->AddStatic(GAME_UI_START_STATIC_4PLAYERS_ID, L"4", 200, 375.0f);
	pMan->AddButton(GAME_UI_START_BUTTON_START_ID, L"Start", 400, 150, 75, 50);
	pMan->AddButton(GAME_UI_START_BUTTON_LOAD_ID, L"Load Game", 500, 150, 150, 50);
	pMan->AddEditBox(GAME_UI_START_EDIT_NAME_ID, 30, 150, 150, 250, 50);
	pMan->AddCheckBox(GAME_UI_START_CHECK_TRUMP_ID, 150, 220);
	pMan->AddRadioButton(GAME_UI_START_RADIO_TWO_PLAYERS, 0, 180.0f, 320.0f);
	pMan->AddRadioButton(GAME_UI_START_RADIO_THREE_PLAYERS, 0, 180.0f, 350.0f);
	pMan->AddRadioButton(GAME_UI_START_RADIO_FOUR_PLAYERS, 0, 180.0f, 380.0f);

	m_pStaticWelcome = (CDXUIStatic*)pMan->GetElement(GAME_UI_START_STATIC_WELCOME_ID);
	m_pStaticTrumpSuit = (CDXUIStatic*)pMan->GetElement(GAME_UI_START_STATIC_TRUMP_ID);
	m_pStaticNumberOfPlayers = (CDXUIStatic*)pMan->GetElement(GAME_UI_START_STATIC_NUMPLAYERS_ID);
	m_pStatic2 = (CDXUIStatic*)pMan->GetElement(GAME_UI_START_STATIC_2PLAYERS_ID);
	m_pStatic3 = (CDXUIStatic*)pMan->GetElement(GAME_UI_START_STATIC_3PLAYERS_ID);
	m_pStatic4 = (CDXUIStatic*)pMan->GetElement(GAME_UI_START_STATIC_4PLAYERS_ID);

	m_pButtonStart = (CDXUIButton*)pMan->GetElement(GAME_UI_START_BUTTON_START_ID);
	m_pButtonLoad = (CDXUIButton*)pMan->GetElement(GAME_UI_START_BUTTON_LOAD_ID);
	m_pEditName = (CDXUIEditBox*)pMan->GetElement(GAME_UI_START_EDIT_NAME_ID);
	m_pCheckTrumpSuit = (CDXUICheckBox*)pMan->GetElement(GAME_UI_START_CHECK_TRUMP_ID);

	m_pRadio2Players = (CDXUIRadioButton*)pMan->GetElement(GAME_UI_START_RADIO_TWO_PLAYERS);
	m_pRadio3Players = (CDXUIRadioButton*)pMan->GetElement(GAME_UI_START_RADIO_THREE_PLAYERS);
	m_pRadio4Players = (CDXUIRadioButton*)pMan->GetElement(GAME_UI_START_RADIO_FOUR_PLAYERS);

	m_pRadio2Players->SetChecked();

	AddElement((IDXUIElement*)m_pStaticWelcome);
	AddElement((IDXUIElement*)m_pStaticTrumpSuit);
	AddElement((IDXUIElement*)m_pStaticNumberOfPlayers);
	AddElement((IDXUIElement*)m_pStatic2);
	AddElement((IDXUIElement*)m_pStatic3);
	AddElement((IDXUIElement*)m_pStatic4);
	AddElement((IDXUIElement*)m_pButtonStart);
	AddElement((IDXUIElement*)m_pButtonLoad);
	AddElement((IDXUIElement*)m_pEditName);
	AddElement((IDXUIElement*)m_pCheckTrumpSuit);
	AddElement((IDXUIElement*)m_pRadio2Players);
	AddElement((IDXUIElement*)m_pRadio3Players);
	AddElement((IDXUIElement*)m_pRadio4Players);

	m_bLoad = false;
}


CStartDialog::~CStartDialog()
{
}


void CStartDialog::OnResize(float fWidth, float fHeight)
{
	UNREFERENCED_PARAMETER(fWidth);
	UNREFERENCED_PARAMETER(fHeight);
}


bool CStartDialog::OnWindowMsg(UINT uiControl, DX_UI_MESSAGE msg)
{
	UNREFERENCED_PARAMETER(msg);

	if (uiControl == GAME_UI_START_BUTTON_START_ID)
	{
		if (wcslen(m_pEditName->GetText()))
		{
			m_bLoad = false;
			Show(false);
			return true;
		}
		else
		{
			return false;
		}
	}
	else if (uiControl == GAME_UI_START_BUTTON_LOAD_ID)
	{
		m_bLoad = true;
		Show(false);
		return true;
	}

	return false;
}


LPCWSTR CStartDialog::GetName() const
{
	return m_pEditName->GetText();
}


bool CStartDialog::GetIsTrumpSuit() const
{
	return m_pCheckTrumpSuit->GetChecked();
}


int CStartDialog::GetNumberOfPlayers()	const
{
	if (m_pRadio2Players->GetChecked())
	{
		return 2;
	}
	else if (m_pRadio3Players->GetChecked())
	{
		return 3;
	}
	else
	{
		return 4;
	}
}