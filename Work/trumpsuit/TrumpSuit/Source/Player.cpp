#include "stdafx.h"
#include "GameManager.h"
#include "Player.h"

using namespace DXFramework;

CPlayer::CPlayer()	:	m_iRoundsWon(0),
						m_iGamesWon(0),
						m_pcvecpRounds(nullptr)
{
	m_pHand = new CHand();
}


CPlayer::~CPlayer()
{
	delete m_pHand;
}


void CPlayer::TakeCard(CCard* pCard)
{
	if (m_pHand->HasCard(pCard))
	{
		m_pHand->RemoveCard(pCard);
	}
	else
	{
		DX_DEBUG_OUTPUT(L"CPlayer::TakeCard: Card not present in hand.");
	}
}


bool CPlayer::Read(std::ifstream& in, void* pData)
{
	in >> m_strName;
	in >> m_iRoundsWon;
	in >> m_iGamesWon;
	
	// Read the hand.
	m_pHand->Read(in, pData);

	return true;
}


bool CPlayer::Write(std::ofstream& out, void* pData)
{
	pData;

	WRITE_STREAM(out, m_strName);
	WRITE_STREAM(out, m_iRoundsWon);
	WRITE_STREAM(out, m_iGamesWon);

	// Write the hand.
	m_pHand->Write(out, nullptr);

	return true;
}