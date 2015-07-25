#pragma once

#include "Player.h"


class CNormalAIPlayer	:	public CPlayer
{
	// Inherited from CPlayer.
public:
	virtual CCard*			SelectCard(bool bFirstInRound, CARD_SUIT suit, void* pData);
	virtual unsigned int	GetPlayerType() const		{ return PLAYER_TYPE_NORMAL_AI;	};
};