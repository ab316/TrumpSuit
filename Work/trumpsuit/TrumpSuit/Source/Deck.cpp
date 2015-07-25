#include "stdafx.h"
#include "Deck.h"


CDeck::CDeck()
{
	// Prepare the cards.
	for (int suit=0; suit<4; suit++) // for each suit
	{
		for(int value=0; value<13; value++) // for each card value.
		{
			// Create a new card.
			CCard* vCard = new CCard((CARD_SUIT)suit, (CARD_VALUE)value);
			// Add the card.
			AddCard(vCard);
		}
	}
}


CDeck::~CDeck()
{
	auto iter=m_vecpCards.begin();
	auto end=m_vecpCards.end();

	// Delete the cards.
	while(iter!=end)
	{
		delete *iter;
		iter++;
	}

	// Erase the vector.
	m_vecpCards.erase(m_vecpCards.begin(), end);
}


int CDeck::GenerateRandom(int iRange)
{
	// AA random number algorithm (2013).
	int a = 0;
	for (int i=0; i<iRange; i++)
	{
		a += (rand() % iRange) * (rand() % iRange);
	}

	if (iRange)
	{
		a /= iRange;
		a %= iRange;
	}
	else
	{
		a /= (iRange + 1);
		a %= (iRange + 1);
	}

	return a;
}


void CDeck::Shuffle()
{
	// Modern version of Fisher-Yates shuffle by Donald E. Knuth (The Art of Computer Programming).
	int random;
	for (int i=51; i>=0; i--)
	{
		random = GenerateRandom(i);

		CCard** p1 = &m_vecpCards[i];
		CCard** p2 = &m_vecpCards[random];
		CCard* pTemp;
		
		pTemp = *p2;

		*p2 = *p1;
		*p1 = pTemp;
	}
}