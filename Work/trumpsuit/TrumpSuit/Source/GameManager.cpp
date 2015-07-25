#include "stdafx.h"
#include "Console.h"
#include "Deck.h"
#include "GameManager.h"
#include "Round.h"
#include "Player.h"
#include "HumanPlayer.h"
#include "NormalAIPlayer.h"
#include "GameApplication.h"


const unsigned int CGameManager::ms_magicGameManager = ('G' << 3) | ('A' << 2) | ('M' << 1) | ('E');


CGameManager::CGameManager()	:	m_iNumRounds(0),
									m_iCurrentPlayerToMove(0),
									m_status(STATUS_NOT_STARTED)
{
	// Create a new deck on the heap.
	m_pDeck = new CDeck();
	m_pSpareHand = new CHand();
	m_pRoundHand = new CHand();

	srand((unsigned int)time(nullptr));
}
	

CGameManager::~CGameManager()
{
	// Delete the deck.
	delete m_pDeck;
	delete m_pSpareHand;
	delete m_pRoundHand;

	auto roundIter = m_vecpRounds.begin();
	auto roundEnd = m_vecpRounds.end();

	while (roundIter != roundEnd)
	{
		delete (*roundIter);
		++roundIter;
	}
}


bool CGameManager::BeginGame()
{
	// Can't carry on if the game is already in progress.
	if (m_status != STATUS_IN_PROGRESS)
	{
		// Number of players must be greater than 2.
		if (GetNumberOfPlayers() >= 2)
		{
			// Random player starts.
			m_iCurrentPlayerToMove = rand() % GetNumberOfPlayers();

			// Update the game status.
			m_status = STATUS_IN_PROGRESS;
			// Reset the player round records.
			ResetPlayersRoundRecord();

			// Shuffle the deck.
			m_pDeck->Shuffle();

			// Find the number of rounds to be played.
			m_iNumRounds = 52 / GetNumberOfPlayers();

			CRound::SetMovesPerRound(GetNumberOfPlayers());

			// Distribute the cards among the players.
			int iCard = 0;
			for (int i=0; i<GetNumberOfPlayers(); ++i)	// loop through the players
			{
				for (int j=0; j<m_iNumRounds; ++j) // loop through the rounds.
				{
					// Give a card to each player for each round.
					m_vecpPlayers[i]->GiveCard((*m_pDeck)[iCard]);
					++iCard;
				}
			}

			// Store the spare cards.
			for (int i=iCard; i<52; ++i)
			{
				m_pSpareHand->AddCard((*m_pDeck)[iCard]);
			}

			DX_DEBUG_OUTPUT0(L"Begin Game.");

			return true;
		}
		else
		{
			DX_DEBUG_OUTPUT0(L"Not enough players.");
			// ERROR. not enough players.
			return false;
		}
	}
	else
	{
		// Game already in progress.
		return false;
	}
}


void CGameManager::EndGame(bool bRemovePlayers)
{
	m_status = STATUS_NOT_STARTED;
	m_iCurrentPlayerToMove = 0;
	ResetPlayersRoundRecord();

	if (bRemovePlayers)
	{
		m_vecpPlayers.clear();
	}
}


void CGameManager::ResetPlayersRoundRecord()
{
	auto iter = m_vecpPlayers.begin();
	auto end = m_vecpPlayers.end();

	while (iter != end)
	{
		(*iter)->ResetRoundsRecord();
		++iter;
	}

	auto roundIter = m_vecpRounds.begin();
	auto roundEnd = m_vecpRounds.end();

	while (roundIter != roundEnd)
	{
		delete (*roundIter);
		++roundIter;
	}

	m_vecpRounds.clear();

	m_pCurrentRound = new CRound();
	m_vecpRounds.push_back(m_pCurrentRound);
}


void CGameManager::IncrementCurrentPlayer()
{
	if (m_status == STATUS_IN_PROGRESS)
	{
		m_iCurrentPlayerToMove++;
		m_iCurrentPlayerToMove %= GetNumberOfPlayers();
	}
	else // Warning.
	{
		DX_DEBUG_OUTPUT0(L"IncrementCurrentPlayer called when game not started.");
	}
}


bool CGameManager::AskPlayerToMove(CPlayer* pPlayer)
{
	bool bFirst = m_pCurrentRound->IsFirstMove();

	if (pPlayer->GetPlayerType() == PLAYER_TYPE_HUMAN)
	{
		CGameApplication::GetInstance()->SetWaitingForPlayer(true);
		return true;
	}
	else
	{
		CARD_SUIT suit = SUIT_SPADE;
		if (!bFirst)
		{
			suit = m_pCurrentRound->GetSuit();
		}

		CCard* pCard;
		do
		{
			pCard = pPlayer->SelectCard(bFirst, suit);
		}
		while (!IsValidMove(pCard, pPlayer));

		pPlayer->TakeCard(pCard);
		RecordMove(pPlayer, pCard);

		return false;
	}
}


void CGameManager::HumanPlayerMoved(CPlayer* pPlayer, CCard* pCard)
{
	if (IsValidMove(pCard, pPlayer))
	{
		CGameApplication::GetInstance()->SetWaitingForPlayer(false);
		RecordMove(pPlayer, pCard);
		((CHand*)(pPlayer->GetHand()))->RemoveCard(pCard);
	}
	PlayGame();
}


void CGameManager::RecordMove(CPlayer* pPlayer, CCard* pCard)
{
	// Record the move in the round.
	m_pCurrentRound->AddMove(pPlayer, pCard);
	// Add the card to the round hand.
	m_pRoundHand->AddCard(pCard);
	
	// Inform the players about the round.
	UpdatePlayersRoundInfo();

	// Next player.
	IncrementCurrentPlayer();

	// If this round is finished.
	if (m_pCurrentRound->Finished())
	{
		// Get the winner of the round.
		CPlayer* pWinner = GetRoundWinner(m_pCurrentRound);
		// Give him the good news.
		pWinner->InformRoundVictory();

		// This player starts the next round.
		int index = GetPlayerIndex(pWinner);
		m_iCurrentPlayerToMove = index;

		// Create a new round.
		m_pCurrentRound = new CRound();
		m_vecpRounds.push_back(m_pCurrentRound);

		// Clear the round hand. New hand for new round.
		m_pRoundHand->Clear();
	}
}


bool CGameManager::IsValidMove(CCard* pCard, CPlayer* pPlayer)
{
	if (m_pCurrentRound->IsFirstMove())
	{
		return true;
	}
	else
	{
		CHand* pHand = (CHand*)pPlayer->GetHand();
		if (pHand->HasSuitCard(m_pCurrentRound->GetSuit()))
		{
			return (pCard->GetSuit() == m_pCurrentRound->GetSuit());
		}
		else
		{
			return true;
		}
	}
}


CPlayer* CGameManager::GetRoundWinner(CRound* pRound)
{
	int iNumMoves = pRound->GetNumberOfMoves();
	InplayCard winner = pRound->GetMove(0);

	for (int i=1; i<iNumMoves; ++i)
	{
		InplayCard move = pRound->GetMove(i);
		if (move.pCard->GetSuit() == pRound->GetSuit())
		{
			if (move.pCard->GetValue() > winner.pCard->GetValue())
			{
				winner = move;
			}
		}
	}

	return winner.pPlayer;
}


void CGameManager::PlayGame()
{
	// Are we playing?
	if (m_status == STATUS_IN_PROGRESS)
	{
		while (GetCurrentRoundNumber() < GetTotalRounds())
		{
			CPlayer* pPlayer = m_vecpPlayers[m_iCurrentPlayerToMove];
			if (AskPlayerToMove(pPlayer))
			{
				return;
			}
		}

		if (GetCurrentRoundNumber() == GetTotalRounds())
		{
			m_status = STATUS_FINISHED;
			CGameApplication::GetInstance()->GameFinished();
		}
	}
}


void CGameManager::UpdatePlayersRoundInfo()
{
	if (m_status == STATUS_IN_PROGRESS)
	{
		auto iter = m_vecpPlayers.begin();
		auto end = m_vecpPlayers.end();

		while (iter != end)
		{
			(*iter)->InformRoundInfo(&m_vecpRounds);
			++iter;
		}
	}
	else // Warning.
	{
		DX_DEBUG_OUTPUT0(L"UpdatePlayersRoundInfo called before game started.");
	}
}


bool StandingsPredicate(CPlayer* p1, CPlayer* p2)
{
	return (p1->GetRoundsWon() > p2->GetRoundsWon());
}


PlayersVector CGameManager::GetStandings()
{
	if (m_status == STATUS_FINISHED)
	{
		std::stable_sort(m_vecpPlayers.begin(), m_vecpPlayers.end(), StandingsPredicate);
		return m_vecpPlayers;
	}
	else
	{
		DX_DEBUG_OUTPUT0(L"Can not return player standings during game play.");
		// Error handling. Can't return standings while game is in progress.
		PlayersVector vec;
		return vec;
	}
}


bool CGameManager::AddPlayer(CPlayer* pPlayer)
{
	// Can't go on if game is in progress.
	if (m_status != STATUS_IN_PROGRESS)
	{
		if (!HasPlayer(pPlayer->GetName()))
		{
			m_vecpPlayers.push_back(pPlayer);
			return true;
		}
		else // ERROR: player already in the game.
		{
			DX_DEBUG_OUTPUT0(L"Player with the given name already exists.");
			return false;
		}
	}
	else // WARNING: Can't add player while game is in progress.
	{
		DX_DEBUG_OUTPUT0(L"AddPlayer called during gameplay.");
		return false;
	}
}


void CGameManager::RemovePlayer(CPlayer* pPlayer)
{
	// Can't do if the game is in progress.
	if (m_status != STATUS_IN_PROGRESS)
	{
		auto iter = m_vecpPlayers.begin();
		auto end = m_vecpPlayers.end();

		while(iter != end)
		{
			if(*iter == pPlayer)
			{
				// Remove the player.
				m_vecpPlayers.erase(iter);
				break;
			}
			iter++;
		}
	}
	else // WARNING.
	{
		DX_DEBUG_OUTPUT0(L"RemovePlayer called during gameplay.");
	}
}


void CGameManager::RemovePlayer(std::string strName)
{
	if (m_status != STATUS_IN_PROGRESS)
	{
		auto iter = m_vecpPlayers.begin();
		auto end = m_vecpPlayers.end();

		while (iter != end)
		{
			if (_stricmp(strName.data(), (*iter)->GetName().data()) == 0)
			{
				m_vecpPlayers.erase(iter);
			}

			++iter;
		}
	}
	else // WARNING.
	{
		DX_DEBUG_OUTPUT0(L"RemovePlayer called during gameplay.");
	}
}


bool CGameManager::HasPlayer(std::string strName)
{
	auto iter = m_vecpPlayers.begin();
	auto end = m_vecpPlayers.end();

	while (iter != end)
	{
		// Match found?
		if (_stricmp(strName.data(), (*iter)->GetName().data()) == 0)
		{
			return true;
		}

		++iter;
	}

	return false;
}


CPlayer* CGameManager::GetPlayer(std::string strName)
{
	auto iter = m_vecpPlayers.begin();
	auto end = m_vecpPlayers.end();

	while (iter != end)
	{
		if (_stricmp(strName.data(), (*iter)->GetName().data()) == 0)
		{
			return *iter;
		}

		++iter;
	}

	return (CPlayer*)nullptr;
}


CPlayer* CGameManager::GetPlayer(int index)
{
	if (index < GetNumberOfPlayers())
	{
		return m_vecpPlayers[index];
	}
	else
	{
		return (CPlayer*)nullptr;
	}
}


int CGameManager::GetPlayerIndex(CPlayer* pPlayer)
{
	auto iter = m_vecpPlayers.begin();
	auto end = m_vecpPlayers.end();

	int index = 0;

	while (iter != end)
	{
		if (*iter == pPlayer)
		{
			return index;
		}

		++iter;
		++index;
	}

	return -1;
}


bool CGameManager::Read(std::ifstream& in, void* pData)
{
	pData;

	if (m_status != STATUS_IN_PROGRESS)
	{
		// Read the magic number.
		unsigned int magic;
		in >> magic;
		if (magic != ms_magicGameManager) // Magic number not correct.
		{
			return false;
		}

		int numPlayers;
		int iStatus;

		// Read the stats.
		in >> m_iNumRounds;
		in >> m_iCurrentPlayerToMove;
		in >> iStatus;
		in >> numPlayers;

		// Add the players.
		for (int i=0; i<numPlayers; ++i)
		{
			unsigned int uiType;
			in >> uiType;

			// Create and add the player.
			CPlayer* pPlayer;
			if (ReadPlayer(uiType, in, m_pDeck, &pPlayer))
			{
				AddPlayer(pPlayer);
			}
			else // Invalid player.
			{
				return false;
			}
		}


		// Read the number of rounds.
		int numRounds;
		in >> numRounds;

		// Populate the rounds.
		for (int i=0; i<numRounds; ++i)
		{
			CRound* pRound = new CRound;
			pRound->Read(in, this);

			m_vecpRounds.push_back(pRound);
		}

		m_pCurrentRound = m_vecpRounds[GetCurrentRoundNumber()];
		int iNumMoves = m_pCurrentRound->GetNumberOfMoves();
		for (int i=0; i<iNumMoves; ++i)
		{
			m_pRoundHand->AddCard(m_pCurrentRound->GetMove(i).pCard);
		}

		// Need to update the status at the end and before UpdatePlayersRoundInfo()
		// because players can not be added during gameplay and the rounds info can not
		// be updated outside gameplay.
		m_status = (GAME_STATUS)iStatus;

		// Propagate the rounds info to the new players.
		UpdatePlayersRoundInfo();

		CRound::SetMovesPerRound(GetNumberOfPlayers());

		// In case we load a finished game.
		if (m_status == STATUS_FINISHED)
		{
			CGameApplication::GetInstance()->GameFinished();
		}
	}
	else // Game already in progress.
	{
		return false;
	}

	return true;
}


bool CGameManager::Write(std::ofstream& out, void* pData)
{
	pData;

	// Write the stats.
	WRITE_STREAM(out, ms_magicGameManager);
	WRITE_STREAM(out, m_iNumRounds);
	WRITE_STREAM(out, m_iCurrentPlayerToMove);
	WRITE_STREAM(out, m_status);
	WRITE_STREAM(out, GetNumberOfPlayers());

	// For each player.
	for (int i=0; i<GetNumberOfPlayers(); ++i)
	{
		CPlayer* pPlayer = m_vecpPlayers[i];

		// Write the type.
		unsigned int uiType = pPlayer->GetPlayerType();
		WRITE_STREAM(out, uiType);

		// Write the player data.
		WritePlayer(uiType, out, nullptr, pPlayer); 
	}


	// Write the number of rounds played.
	WRITE_STREAM(out, m_vecpRounds.size());

	auto iter = m_vecpRounds.begin();
	auto end = m_vecpRounds.end();

	// Iterate through each round.
	while (iter != end)
	{
		CRound* pRound = *iter;
		pRound->Write(out, nullptr);
		++iter;
	}

	return false;
}


bool CGameManager::ReadPlayer(unsigned int uiType, std::ifstream& in, void* pData, CPlayer** ppOutPlayer)
{
	switch (uiType)
	{
	case PLAYER_TYPE_HUMAN:
		*ppOutPlayer = new CHumanPlayer;
		(*ppOutPlayer)->Read(in, pData);
		return true;

	case PLAYER_TYPE_NORMAL_AI:
		*ppOutPlayer = new CNormalAIPlayer;
		(*ppOutPlayer)->Read(in, pData);
		return true;
	}
	return false;
}


bool CGameManager::WritePlayer(unsigned int uiType, std::ofstream& out, void* pData, CPlayer* pPlayer)
{
	switch (uiType)
	{
	case PLAYER_TYPE_HUMAN:
	case PLAYER_TYPE_NORMAL_AI:
		pPlayer->Write(out, pData);
		return true;
	}
	return false;
}