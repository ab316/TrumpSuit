#pragma once

#include "CSerializable.h"

#include "Card.h"


// A hand is a collection of cards. NOTE: Hand is only a collection of pointers
// to cards. The cards are actually allocated on heap. There life time is not managed
// by this class.
class CHand	:	public CSerializable
{
protected:
	std::vector<CCard*>		m_vecpCards;

public:
	// Adds a card to the hand if not already present.
	void		AddCard(const CCard *card);
	// Removes a card form the hand.
	void		RemoveCard(const CCard *card);
	// Removes cards of the given suit from the hand.
	void		RemoveCards(CARD_SUIT suit);
	// Returns true if the hand possesses the provided card.
	bool		HasCard(const CCard *card);
	// Same as above.
	bool		HasCard(CARD_SUIT suit, CARD_VALUE value);
	bool		HasSuitCard(CARD_SUIT suit);
	void		GetSuitCards(CARD_SUIT suit, CHand* pOutHand);
	CCard*		GetLowestValueCard();
	CCard*		GetHighestCardOfSuit(CARD_SUIT suit);
	CCard*		GetLowestCardOfSuit(CARD_SUIT suit);
	UINT		GetNumberOfCardsOfSuit(CARD_SUIT suit);
	CARD_SUIT	GetMostAbundantSuit();

public:
	// Clears the hand of all the cards. NOTE: This does not delete
	void	Clear()			{ m_vecpCards.clear();	};

public:
	const CCard*	GetCard(CARD_SUIT suit, CARD_VALUE value);
	int				GetNumberOfCards() const		{ return m_vecpCards.size();	};

public:
	CCard*	operator[](unsigned int i);

public:
	// pData should point to a valid parent hand because there is supposed to be only
	// one instance of a card.
	virtual bool	Read(std::ifstream& in, void* pData);
	// pData not required.
	virtual bool	Write(std::ofstream& out, void* pData);

public:
	CHand();
	~CHand();
};