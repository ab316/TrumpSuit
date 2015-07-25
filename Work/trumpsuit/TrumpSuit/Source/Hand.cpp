#include "stdafx.h"
#include "Hand.h"

CHand::CHand()
{
}

CHand::~CHand()
{
}

void CHand::AddCard(const CCard *card)
{
	if(!HasCard(card))
	{
		m_vecpCards.push_back((CCard*)card);
	}
}

void CHand::RemoveCard(const CCard *card)
{
	auto iter=m_vecpCards.begin();
	auto end=m_vecpCards.end();

	while(iter!=end)
	{
		if(*iter==card)
		{
			m_vecpCards.erase(iter);
			break;
		}
		iter++;
	}
}


void CHand::RemoveCards(CARD_SUIT suit)
{
	while (HasSuitCard(suit))
	{
		RemoveCard(GetLowestCardOfSuit(suit));
	}
}


bool CHand::HasCard(const CCard *card)
{
	auto iter=m_vecpCards.begin();
	auto end=m_vecpCards.end();

	while(iter!=end)
	{
		if(*iter==card)
		{
			return true;
		}
		iter++;
	}
	return false;
}


bool CHand::HasCard(CARD_SUIT suit, CARD_VALUE value)
{
	auto iter=m_vecpCards.begin();
	auto end=m_vecpCards.end();

	while(iter!=end)
	{
		if ( ((*iter)->GetSuit() == suit) && ((*iter)->GetValue() == value) )
		{
			return true;
		}
		++iter;
	}

	return false;
}



bool CHand::HasSuitCard(CARD_SUIT suit)
{
	auto iter=m_vecpCards.begin();
	auto end=m_vecpCards.end();

	while(iter!=end)
	{
		if ((*iter)->GetSuit() == suit)
		{
			return true;
		}
		++iter;
	}

	return false;
}


const CCard* CHand::GetCard(CARD_SUIT suit, CARD_VALUE value)
{
	auto iter=m_vecpCards.begin();
	auto end=m_vecpCards.end();

	while(iter!=end)
	{
		if ( ((*iter)->GetSuit() == suit) && ((*iter)->GetValue() == value) )
		{
			return *iter;
		}
		++iter;
	}

	return (CCard*)nullptr;
}


void CHand::GetSuitCards(CARD_SUIT suit, CHand* pOutHand)
{
	auto iter = m_vecpCards.begin();
	auto end = m_vecpCards.end();

	while (iter != end)
	{
		CCard* pCard = *iter;
		if (pCard->GetSuit() == suit)
		{
			pOutHand->AddCard(pCard);
		}

		++iter;
	}
}


CCard* CHand::GetLowestValueCard()
{
	if (!GetNumberOfCards())
	{
		return (CCard*)nullptr;
	}

	auto iter = m_vecpCards.begin();
	auto end = m_vecpCards.end();

	CCard* pLowestCard = *iter;
	++iter;

	while (iter != end)
	{
		CCard* pCard = *iter;
		if (pCard->GetValue() < pLowestCard->GetValue())
		{
			pLowestCard = pCard;
		}

		++iter;
	}

	return pLowestCard;
}


CCard* CHand::GetHighestCardOfSuit(CARD_SUIT suit)
{
	CHand suitHand;
	GetSuitCards(suit, &suitHand);

	if (!suitHand.GetNumberOfCards())
	{
		return (CCard*)nullptr;
	}

	auto iter = suitHand.m_vecpCards.begin();
	auto end = suitHand.m_vecpCards.end();

	CCard* pHighest = *iter;
	++iter;

	while (iter != end)
	{
		CCard* pCard = *iter;
		if (pCard->GetValue() > pHighest->GetValue())
		{
			pHighest = pCard;
		}
		++iter;
	}

	return pHighest;
}


UINT CHand::GetNumberOfCardsOfSuit(CARD_SUIT suit)
{
	UINT iCards = 0;

	auto iter = m_vecpCards.begin();
	auto end = m_vecpCards.end();

	while (iter != end)
	{
		if ((*iter)->GetSuit() == suit)
		{
			++iCards;
		}

		++iter;
	}

	return iCards;
}


CCard* CHand::GetLowestCardOfSuit(CARD_SUIT suit)
{
	CHand suitHand;
	GetSuitCards(suit, &suitHand);

	if (!suitHand.GetNumberOfCards())
	{
		return (CCard*)nullptr;
	}

	auto iter = suitHand.m_vecpCards.begin();
	auto end = suitHand.m_vecpCards.end();

	CCard* pLowest = *iter;
	++iter;

	while (iter != end)
	{
		CCard* pCard = *iter;
		if (pCard->GetValue() < pLowest->GetValue())
		{
			pLowest = pCard;
		}
		++iter;
	}

	return pLowest;
}


CARD_SUIT CHand::GetMostAbundantSuit()
{
	int iSpade = GetNumberOfCardsOfSuit(SUIT_SPADE);
	int iClub = GetNumberOfCardsOfSuit(SUIT_CLUB);
	int iDiamond = GetNumberOfCardsOfSuit(SUIT_DIAMOND);
	int iHearts = GetNumberOfCardsOfSuit(SUIT_HEARTS);

	if ( (iSpade >= iClub) && (iSpade >= iDiamond) && (iSpade >= iHearts) )
	{
		return SUIT_SPADE;
	}
	else if ( (iClub >= iSpade) && (iClub >= iDiamond) && (iClub >= iHearts) )
	{
		return SUIT_CLUB;
	}
	else if ( (iDiamond >= iSpade) && (iDiamond >= iClub) && (iDiamond >= iHearts) )
	{
		return SUIT_DIAMOND;
	}
	else if ( (iHearts >= iSpade) && (iHearts >= iDiamond) && (iHearts >= iClub) )
	{
		return SUIT_CLUB;
	}
	else
	{
		return SUIT_SPADE;
	}
}


bool CHand::Read(std::ifstream& in, void* pData)
{
	unsigned int num;

	// Read the number of cards.
	in >> num;
	// Number of cards can not be greater than 52.
	if (num > 52)
	{
		return false;
	}

	// Get the parent deck.
	CHand* pDeck = (CHand*)pData;

	// We do not want multiple copies of card. So we read the card info from the file
	// and then see if the card is present in the parent hand, if that is the case then we
	// just add the pointer to the card in the parent hand.
	for (unsigned int i=0; i<num; i++)
	{
		CCard card;
		card.Read(in, nullptr);
		
		// Acquire the pointer to the card in the parent deck.
		const CCard* pCard = pDeck->GetCard(card.GetSuit(), card.GetValue());
		if (pCard) // found?
		{
			// Add it.
			AddCard(pCard);
		}
	}

	return true;
}


bool CHand::Write(std::ofstream& out, void* pData)
{
	UNREFERENCED_PARAMETER(pData);

	// First write the number of cards.
	WRITE_STREAM(out, (unsigned int)m_vecpCards.size());
	
	auto iter = m_vecpCards.begin();
	auto end = m_vecpCards.end();

	// Now write the cards.
	while (iter != end)
	{
		(*iter)->Write(out, nullptr);
		++iter;
	}

	return true;
}


CCard* CHand::operator[](unsigned int i)
{
	return m_vecpCards[i];
}