#pragma once

#include "Dialog.h"

namespace DXFramework
{
	class CDXUIStatic;
	class CDXUIButton;
	class CDXUIEditBox;
	class CDXUIRadioButton;
	class CDXUICheckBox;
}

class CStartDialog	:	public IDialog
{
private:
	CDXUIStatic*		m_pStaticWelcome;
	CDXUIStatic*		m_pStaticTrumpSuit;
	CDXUIStatic*		m_pStaticNumberOfPlayers;
	CDXUIStatic*		m_pStatic2;
	CDXUIStatic*		m_pStatic3;
	CDXUIStatic*		m_pStatic4;
	CDXUIButton*		m_pButtonStart;
	CDXUIButton*		m_pButtonLoad;
	CDXUIEditBox*		m_pEditName;
	CDXUICheckBox*		m_pCheckTrumpSuit;
	CDXUIRadioButton*	m_pRadio2Players;
	CDXUIRadioButton*	m_pRadio3Players;
	CDXUIRadioButton*	m_pRadio4Players;
	bool				m_bLoad;


public:
	LPCWSTR		GetName()				const;
	bool		GetIsTrumpSuit()		const;
	int			GetNumberOfPlayers()	const;
	bool		GetLoadFile()			const		{ return m_bLoad;	};

public:
	virtual void	OnResize(float fWidth, float fHeight);
	virtual	bool	OnWindowMsg(UINT uiControl, DX_UI_MESSAGE msg);

public:
	CStartDialog();
	~CStartDialog();
};