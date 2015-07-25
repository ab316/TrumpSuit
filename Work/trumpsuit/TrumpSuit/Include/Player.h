#pragma once


#include "CSerializable.h"
#include "Hand.h"

#define PLAYER_TYPE_HUMAN			0
#define PLAYER_TYPE_NORMAL_AI		1


class CRound;

// Abstract Player class.
class CPlayer	:	public CSerializable
{
private:
	std::string							m_strName;
	int									m_iRoundsWon;
	int									m_iGamesWon;

protected:
	// The cards the player is holding.
	CHand*								m_pHand;
	// The actual vector is held by the game manager. It is made constant here because
	// the player should not be allowed to modify any information regarding the game but he
	// should be allowed to access information which in real world a human player can use.
	const std::vector<CRound*>*			m_pcvecpRounds;

public:
	int		GetRoundsWon()	const		{ return m_iRoundsWon;		};
	int		GetGamesWon()	const		{ return m_iGamesWon;		};

public:
	const CHand*	GetHand() const		{ return m_pHand;			};

	// Name set/get.
public:
	std::string		GetName() const					{ return m_strName;			};
	void			SetName(const char* szName)		{ m_strName.assign(szName);	};

public:
	void	ResetRoundsRecord()			{ m_iRoundsWon = 0;				};
	void	ResetGamesRecord()			{ m_iGamesWon = 0;				};
	void	ResetStats()				{ m_iRoundsWon=m_iGamesWon=0;	};
	void	ClearHand()					{ m_pHand->Clear();				};

	// These methods are only to be used by the game manager.
public:
	// Updates the game information.
	void				InformRoundInfo(const std::vector<CRound*>* pInfo)
	{
		m_pcvecpRounds = pInfo;
	}
	void				InformRoundVictory()		{ ++m_iRoundsWon;	};
	void				InformGameVictory()			{ ++m_iGamesWon;	};

	// Called by game manager.
public:
	void				TakeCard(CCard* pCard);
	void				GiveCard(CCard* pCard)		{ m_pHand->AddCard(pCard);	};

	// Derived classes are to implement this method to select a card from their hand to throw.
public:
	virtual CCard*		SelectCard(bool bFirstInRound, CARD_SUIT suit, void* pData=nullptr) = 0;

	// Derived classes are to implement this method to return the correct type.
public:
	virtual unsigned int	GetPlayerType() const = 0;

	// Inherited from CSerializable class.
public:
	// pData should point to a valid parent hand because there is supposed to be only
	// one instance of a card.
	virtual bool	Read(std::ifstream& in, void* pData);
	// pData not required.
	virtual bool	Write(std::ofstream& out, void* pData);

public:
	CPlayer();
	virtual ~CPlayer();
};