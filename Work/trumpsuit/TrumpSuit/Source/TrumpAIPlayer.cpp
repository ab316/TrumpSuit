#include "stdafx.h"
#include "Hand.h"
#include "Round.h"
#include "TrumpGameManager.h"
#include "TrumpAIPlayer.h"


CCard* CTrumpAIPlayer::SelectCard(bool bFirstInRound, CARD_SUIT suit, void* pData)
{
	TrumpData* pTrumpData = (TrumpData*)pData;

	// Trump selection phase?
	if (pTrumpData->bTrumpSelect)
	{
		// Return a card of most abundant suit in player's hand.
		return m_pHand->GetHighestCardOfSuit(m_pHand->GetMostAbundantSuit());
	}

	int numCards = m_pHand->GetNumberOfCards();

	// If it is the first turn. Just throw the highest card. Don't throw the trump suit.
	if (bFirstInRound)
	{
		CCard* pHighestCard = (*m_pHand)[0];

		for (int i=1; i<numCards; ++i)
		{
			CCard* pCard = (*m_pHand)[i];
			if ( (pCard->GetValue() > pHighestCard->GetValue()) && (pCard->GetSuit() != pTrumpData->trumpSuit) )
			{
				pHighestCard = pCard;
			}
		}

		if (!pHighestCard)
		{
			pHighestCard = m_pHand->GetHighestCardOfSuit(m_pHand->GetMostAbundantSuit());
		}

		return pHighestCard;
	}
	else // Otherwise (Its not our first turn).
	{
		// Get the current round.
		std::vector<CRound*> rounds = *m_pcvecpRounds;
		CRound* pCurrent = rounds[rounds.size()-1];

		CHand roundHand;
		pCurrent->PopulateHand(&roundHand);

		// Was there a trump suit card thrown in the round.
		if (roundHand.HasSuitCard(pTrumpData->trumpSuit))
		{
			// Get the highest trump card thrown.
			CCard* pTrumpCard = roundHand.GetHighestCardOfSuit(pTrumpData->trumpSuit);

			// Get the highest trump suit card we have.
			CCard* pCard = m_pHand->GetHighestCardOfSuit(pTrumpData->trumpSuit);
			if (pCard) // Do we have one.
			{
				// Is our trump card bigger?
				if (pCard->GetValue() > pTrumpCard->GetValue())
				{
					return pCard;
				}
				else // smaller?
				{
					if (m_pHand->HasSuitCard(suit)) // Do have a card of required suit?
					{
						// Just throw the smallest card we have of the required suit.
						return m_pHand->GetLowestCardOfSuit(suit);
					}
					else
					{
						// Throw the lowest value card making suring its not of trump suit.
						CHand noTrumps = *m_pHand;
						noTrumps.RemoveCards(pTrumpData->trumpSuit);
						return noTrumps.GetLowestValueCard();
					}
				}
			}
			else // Trump card in round and we don't have a trump suit card.
			{
				// If we have a card of the correct suit.
				if (m_pHand->HasSuitCard(suit))
				{
					// Just throw the smallest card we have of the required suit.
					return m_pHand->GetLowestCardOfSuit(suit);
				}
				else // Don't have the correct suit.
				{
					// Throw the lowest value card.
					return m_pHand->GetLowestValueCard();
				}
			}
		}
		else // No trump suit card in play.
		{
			// Get the highest legal card in the round. (Round may have other suit cards
			// thrown by player who did not have any suit card).
			CCard* pHighestPlay = roundHand.GetHighestCardOfSuit(suit);
			// Get our highest card of proper suit.
			CCard* pCard = m_pHand->GetHighestCardOfSuit(suit);

			if (pCard) // Have the proper card?
			{
				// If our card is bigger?
				if (pCard->GetValue() > pHighestPlay->GetValue())
				{
					// To add some randomness to decision.
					int choice = rand() % 10;
					// 30% chance of throwing a trump suit card instead of highest card of correct suit.
					// Must have a trump suit card!
					if (choice > 6 && m_pHand->HasSuitCard(pTrumpData->trumpSuit))
					{
						// Select a trump suit card instead.
						pCard = m_pHand->GetLowestCardOfSuit(pTrumpData->trumpSuit);
					}

					return pCard;
				}
				else // Our card is smaller.
				{
					// Select the proper card of lowest value.
					pCard = m_pHand->GetLowestCardOfSuit(suit);

					// Randomness.
					int choice = rand() % 10;

					// Do we have a trump suit card.
					if (m_pHand->HasSuitCard(pTrumpData->trumpSuit))
					{
						if (choice < 8) // 80% chance
						{
							// Throw a trump suit card of lowest value.
							pCard = m_pHand->GetLowestCardOfSuit(pTrumpData->trumpSuit);
						}
					}
					return pCard;
				}
			}
			else // Don't have a card of proper suit.
			{
				// Select the lowest value card.
				pCard = m_pHand->GetLowestValueCard();

				int random = rand() % 10;

				// 70% chance if having a trump suit card.
				if (random < 7 && m_pHand->HasSuitCard(pTrumpData->trumpSuit))
				{
					// Select a trump suit card of lowest value.
					pCard = m_pHand->GetLowestCardOfSuit(pTrumpData->trumpSuit);
				}

				return pCard;
			}
		}
	}
}