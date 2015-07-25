#include "Stdafx.h"
#include "Console.h"
#include "HumanPlayer.h"


CCard* CHumanPlayer::SelectCard(bool bFirstInRound, CARD_SUIT suit, void* pData)
{
	UNREFERENCED_PARAMETER(bFirstInRound);
	UNREFERENCED_PARAMETER(pData);
	UNREFERENCED_PARAMETER(suit);

	//Console::DisplayMessage("You turn. You have the following cards: ");
	//Console::NewLine();

	//int numCards = m_pHand->GetNumberOfCards();
	//for (int i=0; i<numCards; ++i)
	//{
	//	CCard* pCard = (*m_pHand)[i];

	//	Console::DisplayCard(pCard);
	//}

	//Console::DisplayMessage("Enter index of card to throw: ");
	//int index = Console::ReadInteger();

	return (CCard*)nullptr;
}