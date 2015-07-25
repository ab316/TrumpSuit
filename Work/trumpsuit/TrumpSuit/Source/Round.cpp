#include "stdafx.h"
#include "Card.h"
#include "Round.h"
#include "GameManager.h"
#include "Deck.h"
#include "Player.h"


int CRound::ms_iMovesPerRound = 0;

CRound::CRound()
{
}


CRound::~CRound()
{
}


bool CRound::AddMove(CPlayer* pPlayer, CCard* pCard)
{
	if (Finished())
	{
		return true;
	}

	InplayCard play;
	play.pCard = pCard;
	play.pPlayer = pPlayer;

	m_vecInplayCards.push_back(play);

	return Finished();
}


CARD_SUIT CRound::GetSuit()
{
	if (GetNumberOfMoves())
	{
		return m_vecInplayCards[0].pCard->GetSuit();
	}
	else
	{
		return SUIT_SPADE;
	}
}


void CRound::PopulateHand(CHand* pOutHand)
{
	auto iter = m_vecInplayCards.begin();
	auto end = m_vecInplayCards.end();

	while (iter != end)
	{
		pOutHand->AddCard((*iter).pCard);
		++iter;
	}
}


bool CRound::Read(std::ifstream& in, void* pData)
{
	CGameManager* pManager = (CGameManager*)pData;

	int iMoves;
	in >> iMoves;

	CDeck* pDeck = (CDeck*)pManager->GetDeck();

	std::string strPlayer;

	for (int i=0; i<iMoves; ++i)
	{
		InplayCard play;
		
		CCard card;
		card.Read(in, nullptr);
		
		play.pCard = (CCard*)pDeck->GetCard(card.GetSuit(), card.GetValue());

		in >> strPlayer;
		play.pPlayer = pManager->GetPlayer(strPlayer);

		m_vecInplayCards.push_back(play);
	}

	return true;
}


bool CRound::Write(std::ofstream& out, void* pData)
{
	UNREFERENCED_PARAMETER(pData);

	WRITE_STREAM(out, GetNumberOfMoves());

	auto iter = m_vecInplayCards.begin();
	auto end = m_vecInplayCards.end();

	while (iter != end)
	{
		(*iter).pCard->Write(out, nullptr);
		WRITE_STREAM(out, (*iter).pPlayer->GetName());

		++iter;
	}

	return true;
}