#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <time.h>
#include <algorithm>
#include "DXStdafx.h"


// Macro for input to file (Adhoc approach for now).
#define WRITE_STREAM(s, x) (s << x << std::endl)


class CPlayer;
class CCard;

// Store info about a card played by a player.
struct InplayCard
{
	CPlayer*	pPlayer;	// The player who played the card.
	CCard*		pCard;		// The card played.
};


// Stores info about all the moves in a round.
struct RoundInfo
{
	std::vector<InplayCard>		vecInplayCards;
	// Winner of the round.
	CPlayer*					pWinner;
};


enum GAME_STATUS
{
	STATUS_NOT_STARTED,		// Game is not yet started.
	STATUS_IN_PROGRESS,		// Game is in progress.
	STATUS_FINISHED,		// Game is finished.
};