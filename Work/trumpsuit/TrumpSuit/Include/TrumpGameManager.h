#pragma once

#include "GameManager.h"

struct TrumpData
{
	bool		bTrumpSelect;
	CARD_SUIT	trumpSuit;
};

class CTrumpGameManager	:	public CGameManager
{
private:
	bool			m_bTrumpSelectionPhase;
	CARD_SUIT		m_trumpSuit;

public:
	bool		IsTrumpSelectionPhase();
	CARD_SUIT	GetTrumpSuit()			{ return m_trumpSuit;	};

public:
	// Returns true if p1 has higher value than p2.
	bool Compare(CCard* p1, CCard* p2, bool bTrumpSuit, CARD_SUIT trumpSuit);

	// Inherited from base class.
public:
	virtual bool		IsValidMove(CCard* pCard, CPlayer* pPlayer);
	virtual bool		AskPlayerToMove(CPlayer* pPlayer);
	virtual CPlayer*	GetRoundWinner(CRound* pRound);
	virtual void		HumanPlayerMoved(CPlayer* pPlayer, CCard* pCard);

public:
	virtual bool		ReadPlayer(unsigned int uiType, std::ifstream& in, void* pData, CPlayer** ppOutPlayer);
	virtual bool		WritePlayer(unsigned int uiType, std::ofstream& out, void* pData, CPlayer* pPlayer);

	// Inherited from CSerializable class.
public:
	// pData not required.
	virtual	bool	Read(std::ifstream& in, void* pData);
	// pData not required.
	virtual bool	Write(std::ofstream& out, void* pData);

public:
	CTrumpGameManager();
	~CTrumpGameManager();
};