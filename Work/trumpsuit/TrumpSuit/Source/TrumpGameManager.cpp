#include "stdafx.h"
#include "Console.h"
#include "Deck.h"
#include "Round.h"
#include "Player.h"
#include "HumanPlayer.h"
#include "NormalAIPlayer.h"
#include "TrumpAIPlayer.h"
#include "GameApplication.h"
#include "TrumpGameManager.h"


CTrumpGameManager::CTrumpGameManager()	:	m_bTrumpSelectionPhase(true),
											m_trumpSuit(SUIT_SPADE)
{
}


CTrumpGameManager::~CTrumpGameManager()
{
}


bool CTrumpGameManager::Compare(CCard* p1, CCard* p2, bool bTrumpSuit, CARD_SUIT trumpSuit)
{
	if (bTrumpSuit)
	{
		if (p1->GetSuit() == trumpSuit && p2->GetSuit() != trumpSuit)
		{
			return true;
		}
		else if (p1->GetSuit() != trumpSuit && p2->GetSuit() == trumpSuit)
		{
			return false;
		}
		else
		{
			return ( (UINT)p1->GetValue() > (UINT)p2->GetValue() );
		}
	}
	else
	{
		return ( (UINT)p1->GetValue() > (UINT)p2->GetValue() );
	}
}


bool CTrumpGameManager::IsTrumpSelectionPhase()
{
	return m_bTrumpSelectionPhase;
}


bool CTrumpGameManager::IsValidMove(CCard* pCard, CPlayer* pPlayer)
{
	// Trump suit selection.
	if (IsTrumpSelectionPhase())
	{
		return true;
	}
	else if (pCard->GetSuit() == m_trumpSuit)	// Trump suit throw.
	{
		return true;
	}
	else // Normal card.
	{
		// Allow base class to handle it.
		return CGameManager::IsValidMove(pCard, pPlayer);
	}
}


CPlayer* CTrumpGameManager::GetRoundWinner(CRound* pRound)
{
	int iNumMoves = pRound->GetNumberOfMoves();
	InplayCard winner = pRound->GetMove(0);

	CARD_SUIT roundSuit = pRound->GetSuit();

	for (int i=1; i<iNumMoves; ++i)
	{
		InplayCard play = pRound->GetMove(i);
		CARD_SUIT suit = play.pCard->GetSuit();

		if (suit == roundSuit || suit == m_trumpSuit)
		{
			if (Compare(play.pCard, winner.pCard, true, m_trumpSuit))
			{
				winner = play;
			}
		}
	}

	return winner.pPlayer;
}


bool CTrumpGameManager::AskPlayerToMove(CPlayer* pPlayer)
{
	bool bFirst = m_pCurrentRound->IsFirstMove();
	TrumpData data;

	// Trump selection phase?
	if (IsTrumpSelectionPhase())
	{
		data.bTrumpSelect = true;
	}
	else
	{
		data.bTrumpSelect = false;
		data.trumpSuit = m_trumpSuit;
	}


	if (pPlayer->GetPlayerType() == PLAYER_TYPE_HUMAN)
	{
		CGameApplication::GetInstance()->SetWaitingForPlayer(true);
		return true;
	}
	else
	{
		CARD_SUIT suit = SUIT_SPADE;
		if (!bFirst)
		{
			suit = m_pCurrentRound->GetSuit();
		}

		CCard* pCard;
		do
		{
			pCard = pPlayer->SelectCard(bFirst, suit, &data);
		}
		while (!IsValidMove(pCard, pPlayer));

		// Trump selection phase? Just store the trump suit.
		if (data.bTrumpSelect)
		{
			m_trumpSuit = pCard->GetSuit();
			IncrementCurrentPlayer();
			m_bTrumpSelectionPhase = false;
		}
		else // Else continue as usual.
		{
			pPlayer->TakeCard(pCard);
			RecordMove(pPlayer, pCard);
		}

		return false;
	}
}


void CTrumpGameManager::HumanPlayerMoved(CPlayer* pPlayer, CCard* pCard)
{
	if (IsValidMove(pCard, pPlayer))
	{
		CGameApplication::GetInstance()->SetWaitingForPlayer(false);

		if (IsTrumpSelectionPhase())
		{
			m_trumpSuit = pCard->GetSuit();
			IncrementCurrentPlayer();
			m_bTrumpSelectionPhase = false;
		}
		else
		{
			RecordMove(pPlayer, pCard);
			((CHand*)(pPlayer->GetHand()))->RemoveCard(pCard);
		}
	}
	PlayGame();
}


bool CTrumpGameManager::ReadPlayer(unsigned int uiType, std::ifstream& in, void* pData, CPlayer** ppOutPlayer)
{
	if (!CGameManager::ReadPlayer(uiType, in, pData, ppOutPlayer))
	{
		switch (uiType)
		{
		case PLAYER_TYPE_TRUMP_AI:
			*ppOutPlayer = new CTrumpAIPlayer;
			(*ppOutPlayer)->Read(in, pData);
			return true;
			
		default:
			return false;
		}
	}
	else
	{
		return true;
	}
}


bool CTrumpGameManager::WritePlayer(unsigned int uiType, std::ofstream& out, void* pData, CPlayer* pPlayer)
{
	if (!CGameManager::WritePlayer(uiType, out, pData, pPlayer))
	{
		switch (uiType)
		{
		case PLAYER_TYPE_TRUMP_AI:
			pPlayer->Write(out, pData);
			return true;
			
		default:
			return false;
		}
	}
	else
	{
		return true;
	}
}


bool CTrumpGameManager::Read(std::ifstream& in, void* pData)
{
	char cTrump;
	in >> cTrump;
	in >> m_bTrumpSelectionPhase;

	m_trumpSuit = (CARD_SUIT)cTrump;

	return CGameManager::Read(in, pData);
}


bool CTrumpGameManager::Write(std::ofstream& out, void* pData)
{
	char cTrump = (char)m_trumpSuit;
	WRITE_STREAM(out, cTrump);
	WRITE_STREAM(out, m_bTrumpSelectionPhase);
	out.flush();

	return CGameManager::Write(out, pData);
}