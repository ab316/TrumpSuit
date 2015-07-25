#pragma once

#include "CSerializable.h"

class CDeck;
class CHand;
class CRound;

typedef std::vector<CPlayer*>	PlayersVector;


class CGameManager	:	public CSerializable
{
private:
	static const unsigned int ms_magicGameManager;

protected:
	// The players vector. Stores the pointer to the players in the game.
	PlayersVector				m_vecpPlayers;
	// The rounds vector. Holds record of all the moves in all the rounds played.
	std::vector<CRound*>		m_vecpRounds;
	// Pointer to the deck.
	CDeck*						m_pDeck;
	// Cards not distributed among any body.
	CHand*						m_pSpareHand;
	// Cards in play in the current round.
	CHand*						m_pRoundHand;
	CRound*						m_pCurrentRound;
	// Total number of rounds in the game.
	int							m_iNumRounds;
	//Stores which player's turn it is.
	int							m_iCurrentPlayerToMove;
	// Status of the game.
	GAME_STATUS					m_status;


protected:
	// Moves the turn to the next player.
	void		IncrementCurrentPlayer();

private:
	// Informs the players about the round information. This tells the players about all the
	// moves that have been made so far in the game.
	void		UpdatePlayersRoundInfo();
	// Resets the rounds won record of the players.
	void		ResetPlayersRoundRecord();
	
public:
	// Begins a new game if not already in progress. Shuffles the deck and distributes the cards
	// among the players.
	bool		BeginGame();
	// Ends the game (if in progress). Resets the game stats any way.
	void		EndGame(bool bRemovePlayers=false);
	// Starts playing the game. The method returns when it's human player's turn.
	// The application should then calls the HumanPlayerMoved method to continue the game.
	void		PlayGame();
	// Records a move made by a player. Starts a new round if current round is finished.
	void		RecordMove(CPlayer* pPlayer, CCard* pCard);

public:
	int			GetTotalRounds()		const		{ return m_iNumRounds;			};
	int			GetCurrentRoundNumber()	const		{ return m_vecpRounds.size()-1;	};
	int			GetNumberOfPlayers()	const		{ return m_vecpPlayers.size();	};
	GAME_STATUS	GetGameStatus()			const		{ return m_status;				};
	CPlayer*	GetCurrentPlayer()		const		{ return m_vecpPlayers[m_iCurrentPlayerToMove];	};

public:
	const CHand*					GetSpareHand()	const	{ return m_pSpareHand;	};
	const CHand*					GetRoundHand()	const	{ return m_pRoundHand;	};
	const CDeck*					GetDeck()		const	{ return m_pDeck;		};

public:
	// Should be called after the game is finished. Returns the final standings of the players.
	PlayersVector	GetStandings();
	// Adds a player to the game. True if succeeded. Can not be done while game is in
	// progress.
	bool			AddPlayer(CPlayer* pPlayer);
	// Removes player from the game. Can not be done while game is in progress.
	void			RemovePlayer(CPlayer* pPlayer);
	// Removes player with the specified name from the game. Can not be done 
	// while game is in progress.
	void			RemovePlayer(std::string strName);
	// True if the game has a player with the given name.
	bool			HasPlayer(std::string strName);
	// Returns a player with the given name. nullptr if not present.
	CPlayer*		GetPlayer(std::string strName);
	CPlayer*		GetPlayer(int index);
	int				GetPlayerIndex(CPlayer* pPlayer);

public:
	// If the user implements new player types, then these methods should be overloaded
	// to properly read and write those new types. The methods return true if the method
	// was designed for the given type, otherwise false. The ReadPlayer methods is responsible
	// for creating the player object.
	virtual bool		ReadPlayer(unsigned int uiType, std::ifstream& in, void* pData, CPlayer** ppOutPlayer);
	virtual bool		WritePlayer(unsigned int uiType, std::ofstream& out, void* pData, CPlayer* pPlayer);
	// Validates the move made by a player.
	virtual bool		IsValidMove(CCard* pCard, CPlayer* pPlayer);
	virtual CPlayer*	GetRoundWinner(CRound* pRound);
	// Asks a player to make a move and then records it. Returns true if it is a human
	// player and the manager must wait for the player.
	virtual bool		AskPlayerToMove(CPlayer* pPlayer);
	// Should be called when the human player make a move.
	virtual void		HumanPlayerMoved(CPlayer* pPlayer, CCard* pCard);

	// Inherited from CSerializable class.
public:
	// pData not required.
	virtual	bool	Read(std::ifstream& in, void* pData);
	// pData not required.
	virtual bool	Write(std::ofstream& out, void* pData);

public:
	CGameManager();
	virtual ~CGameManager();
};