#pragma once

#include "Player.h"

#define PLAYER_TYPE_TRUMP_AI	2

class CTrumpAIPlayer	:	public CPlayer
{
	// Inherited from CPlayer.
public:
	virtual CCard*			SelectCard(bool bFirstInRound, CARD_SUIT suit, void* pData);
	virtual unsigned int	GetPlayerType() const		{ return PLAYER_TYPE_TRUMP_AI;	};
};