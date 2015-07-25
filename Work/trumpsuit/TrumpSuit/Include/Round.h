#pragma once

#include "CSerializable.h"

class CPlayer;
class CHand;

class CRound	:	public CSerializable
{
private:
	static int					ms_iMovesPerRound;
	std::vector<InplayCard>		m_vecInplayCards;

public:
	// Returns true if the round is finished.
	bool				AddMove(CPlayer* pPlayer, CCard* pCard);
	// Returns the legal suit of the round (the suit of the first card played).
	CARD_SUIT			GetSuit();
	// Adds the cards in the round in the provided hand.
	void				PopulateHand(CHand* pOutHand);

public:
	bool	Finished()			const	{ return GetNumberOfMoves() == ms_iMovesPerRound;	};
	int		GetNumberOfMoves()	const	{ return m_vecInplayCards.size();					};
	bool	IsFirstMove()		const	{ return GetNumberOfMoves() == 0;					};

public:
	InplayCard	GetMove(int i)	const	{ return m_vecInplayCards[i];	};

public:
	static void		SetMovesPerRound(int i)			{ ms_iMovesPerRound = i;		};


	// Inherited from CSerializable class.
public:
	// pData points to the game manager.
	virtual	bool	Read(std::ifstream& in, void* pData);
	// No additional data needed.
	virtual bool	Write(std::ofstream& out, void* pData);

public:
	CRound();
	~CRound();
};