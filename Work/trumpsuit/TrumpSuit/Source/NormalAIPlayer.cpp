#include "Stdafx.h"
#include "Console.h"
#include "Card.h"
#include "Round.h"
#include "NormalAIPlayer.h"


CCard* CNormalAIPlayer::SelectCard(bool bFirstInRound, CARD_SUIT suit, void* pData)
{
	UNREFERENCED_PARAMETER(pData);

	//Console::DisplayMessage(GetName());
	//Console::DisplayMessage("'s turn.");
	//Console::NewLine();

	int numCards = m_pHand->GetNumberOfCards();

	// If its the first turn. Just throw the highest card.
	if (bFirstInRound)
	{
		CCard* pHighestCard = (*m_pHand)[0];

		for (int i=1; i<numCards; ++i)
		{
			CCard* pCard = (*m_pHand)[i];
			if (pCard->GetValue() > pHighestCard->GetValue())
			{
				pHighestCard = pCard;
			}
		}

		return pHighestCard;
	}
	else // Otherwise.
	{
		// If we don't have a card of required suit.
		if (!m_pHand->HasSuitCard(suit))
		{
			// Throw the lowest value card.
			CCard* pLowest = (*m_pHand)[0];

			for (int i=1; i<numCards; ++i)
			{
				CCard* pCard = (*m_pHand)[i];
				if (pCard->GetValue() < pLowest->GetValue())
				{
					pLowest = pCard;
				}
			}
			return pLowest;
		}
		else // We have a card of required suit.
		{
			// Get the current round.
			std::vector<CRound*> rounds = *m_pcvecpRounds;
			CRound* pCurrent = rounds[rounds.size()-1];

			int numMoves = pCurrent->GetNumberOfMoves();

			InplayCard highestMove = pCurrent->GetMove(0);

			// Find the highest move played (making sure it is of the correct suit). (A player
			// who did not have a card of correct suit would have thrown a card of any suit)
			// We need to ignore it here.
			for (int i=1; i<numMoves; ++i)
			{
				InplayCard move = pCurrent->GetMove(i);
				if ( (move.pCard->GetSuit() == suit) && (move.pCard->GetValue() > highestMove.pCard->GetValue()) )
				{
					highestMove = move;
				}
			}

			// The highest card we possess of the required suit.
			CCard* pCard = m_pHand->GetHighestCardOfSuit(suit);
			// Is this card higher than the highest move?
			if (pCard->GetValue() > highestMove.pCard->GetValue())
			{
				return pCard;
			}
			else
			{
				// Throw the lowest value card of the required suit.
				return m_pHand->GetLowestCardOfSuit(suit);
			}
		}
	}

	//int index = rand() % numCards;
	//CCard* pCard = (*m_pHand)[index];

	////Console::DisplayMessage("throws ");
	////Console::DisplayCard(pCard);

	//return (*m_pHand)[index];
}