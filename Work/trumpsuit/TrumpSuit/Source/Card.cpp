#include "stdafx.h"
#include "RenderableCard.h"
#include "Card.h"


CCard::CCard(CARD_SUIT suit, CARD_VALUE value)
{
	m_suit = suit;
	m_value = value;
	m_pRenderable = new CRenderableCard(this);
}

CCard::	~CCard()
{
	DX_SAFE_DELETE(m_pRenderable);
}


bool CCard::Read(std::ifstream& in, void* pData)
{
	pData;

	int suit;
	int value;

	in >> suit;
	in >> value;

	if ( (suit > (int)SUIT_HEARTS) | (value > (int)VALUE_A) )
	{
		return false;
	}

	m_suit = (CARD_SUIT)suit;
	m_value = (CARD_VALUE)value;

	return true;
}


bool CCard::Write(std::ofstream& out, void* pData)
{
	pData;

	WRITE_STREAM(out, m_suit);
	WRITE_STREAM(out, m_value);

	return true;
}