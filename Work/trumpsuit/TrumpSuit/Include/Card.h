#pragma once

#include "CSerializable.h"

// A container for suit
enum CARD_SUIT
{
	SUIT_SPADE,
	SUIT_CLUB,
	SUIT_DIAMOND,
	SUIT_HEARTS
};

// An enum for card values.
enum CARD_VALUE
{
	VALUE_2,
	VALUE_3,
	VALUE_4,
	VALUE_5,
	VALUE_6,
	VALUE_7,
	VALUE_8,
	VALUE_9,
	VALUE_10,
	VALUE_J,
	VALUE_Q,
	VALUE_K,
	VALUE_A,
};


class CRenderableCard;

// card class. Holds information regarding a card.
class CCard	:	public CSerializable
{
private:
	CARD_SUIT			m_suit;
	CARD_VALUE			m_value;
	CRenderableCard*	m_pRenderable;

public:
	CARD_SUIT		GetSuit()	const		{ return m_suit;	};
	CARD_VALUE		GetValue()	const		{ return m_value;	};

public:
	CRenderableCard*	GetRenderable() const		{ return m_pRenderable;	};


	// Inherited from CSerializable.
public:
	// pData not required.
	virtual bool	Read(std::ifstream& in, void* pData);
	// pData not required.
	virtual bool	Write(std::ofstream& out, void* pData);

public:
	CCard(CARD_SUIT suit=SUIT_SPADE, CARD_VALUE value=VALUE_2);
	~CCard();
};