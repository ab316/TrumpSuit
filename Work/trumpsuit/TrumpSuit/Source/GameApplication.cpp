#include "stdafx.h"

#include "DXFileSystem.h"
#include "DXBaseApplication.h"
#include "DX3DDriver.h"
#include "DXGraphicsDriver.h"
#include "DXTextRenderer.h"
#include "DXUIButton.h"

#include "GameGraphics.h"
#include "GameApplication.h"
#include "CardMesh.h"
#include "Card.h"
#include "RenderableCard.h"
#include "GameManager.h"
#include "TrumpGameManager.h"
#include "HumanPlayer.h"
#include "NormalAIPlayer.h"
#include "TrumpAIPlayer.h"

#include "StartDialog.h"

using namespace DXFramework;

#define DX_UI_PLAY_SAVE		20
#define SAVE_GAME_FILE		"save.dat"


CGameApplication::CGameApplication()	:	m_pDialogStart(nullptr),
											m_pManager(nullptr),
											m_pPlayer(nullptr),
											m_screen(GAME_SCREEN_START),
											m_bWaitingPlayerMove(false)
{
	CDXFileSystem* pFile = CDXFileSystem::GetInstance();
	pFile->AddAdditionalDirectory(L"Textures\\Cards");
	m_pDXApp = CDXBaseApplication::GetInstance();

	m_pDXApp->SetCallbackOnFrameRender(OnFrameRender);
	m_pDXApp->SetCallbackOnResize(OnResize);
	m_pDXApp->SetCallbackOnWindowMessage(OnWindowMessage);

	CDXUIManager::GetInstance()->AddButton(DX_UI_PLAY_SAVE, L"Save", 700, 20, 75, 50);
	m_pSaveButton = (CDXUIButton*)CDXUIManager::GetInstance()->GetElement(DX_UI_PLAY_SAVE);
	m_pSaveButton->SetVisible(false);

	m_pGameGraphics = new CGameGraphics();
}


CGameApplication::~CGameApplication()
{
	DX_SAFE_DELETE(m_pGameGraphics);
	DX_SAFE_DELETE(m_pPlayer);
	DX_SAFE_DELETE(m_pManager);
}


bool CGameApplication::Startup(HINSTANCE hInstance)
{
	m_pDXApp->Startup(hInstance, 800, 600, true, L"Card Game");
	if (FAILED(m_pGameGraphics->Create()))
	{
		DX_ERROR_MSGBOX(L"Unable to create game graphics.");
		return false;
	}

	CDXUIManager* pUIMan = CDXUIManager::GetInstance();
	pUIMan->SetCallbackOnUIMessage(OnUIMessage);

	m_pDialogStart = new CStartDialog();

	return true;
}


void CGameApplication::Shutdown()
{
	DX_SAFE_DELETE(m_pDialogStart);
	m_pGameGraphics->Destroy();

	m_pDXApp->Shutdown();
}


void CGameApplication::Run()
{
	m_pDXApp->Run();
}


void CGameApplication::Reset()
{
	m_screen = GAME_SCREEN_START;
	m_pSaveButton->SetVisible(false);

	if (m_pManager)
	{
		for (int i=0; i<m_pManager->GetNumberOfPlayers(); ++i)
		{
			CPlayer* pPlayer = m_pManager->GetPlayer(i);
			DX_SAFE_DELETE(pPlayer);
		}

		DX_SAFE_DELETE(m_pManager);
	}

	m_pDialogStart->Show(true);
}


void CGameApplication::CreateHumanPlayer(LPCWSTR szName)
{
	if (!m_pPlayer)
	{
		m_pPlayer = new CHumanPlayer;
	}
	
	int iLength = wcslen(szName);
	char* name = new char[iLength+1];

	WideCharToMultiByte(CP_ACP, 0, szName, -1, name, iLength+1, 0, 0);
	m_pPlayer->SetName(name);
	m_pPlayer->ResetStats();
}


void CGameApplication::BeginGame(bool bTrumpSuit, int iAIPlayers)
{
	m_bTrumpGame = bTrumpSuit;

	DX_SAFE_DELETE(m_pManager);

	if (m_bTrumpGame)
	{
		m_pManager = new CTrumpGameManager();
	}
	else
	{
		m_pManager = new CGameManager();
	}

	m_pManager->AddPlayer(m_pPlayer);

	float afPlayerX[4] = { 0.0f, 0.0f, -5.0f, 5.0f };
	float afPlayerY[4] = { -3.0f, 3.0f, 0.0f, 0.0f };
	bool abHorizontal[4] = { true, true, false, false };
	char aszAINames[4][30] = { "AI1", "AI2", "AI3" };

	for (int i=0; i<iAIPlayers; ++i)
	{
		CPlayer* ai;
		if (m_bTrumpGame)
		{
			ai = new CTrumpAIPlayer();
		}
		else
		{
			ai = new CNormalAIPlayer();
		}
		ai->SetName(aszAINames[i]);
		m_pManager->AddPlayer(ai);
	}

	m_pManager->BeginGame();

	for (int i=0; i<m_pManager->GetNumberOfPlayers(); ++i)
	{
		CPlayer* pPlayer = m_pManager->GetPlayer(i);
		m_pGameGraphics->PlaceHandCards(pPlayer->GetHand(), afPlayerX[i], afPlayerY[i], abHorizontal[i]);
		m_pGameGraphics->m_players.push_back(pPlayer);
	}

	m_pManager->PlayGame();
}


void CGameApplication::LoadGame()
{
	std::ifstream in(SAVE_GAME_FILE);
	in >> m_bTrumpGame;

	DX_SAFE_DELETE(m_pManager);
	if (m_bTrumpGame)
	{
		m_pManager = new CTrumpGameManager();
	}
	else
	{
		m_pManager = new CGameManager();
	}

	float afPlayerX[4] = { 0.0f, 0.0f, -5.0f, 5.0f };
	float afPlayerY[4] = { -3.0f, 3.0f, 0.0f, 0.0f };
	bool abHorizontal[4] = { true, true, false, false };

	if (!m_pManager->Read(in, nullptr))
	{
		MessageBox(NULL, L"Error loading saved file.", L"Error", MB_ICONERROR);
		Reset();

		return;
	}

	for (int i=0; i<m_pManager->GetNumberOfPlayers(); ++i)
	{
		CPlayer* pPlayer = m_pManager->GetPlayer(i);
		m_pGameGraphics->PlaceHandCards(pPlayer->GetHand(), afPlayerX[i], afPlayerY[i], abHorizontal[i]);
		m_pGameGraphics->m_players.push_back(pPlayer);
	}

	m_pPlayer = (CHumanPlayer*)m_pManager->GetPlayer(0);

	m_pManager->PlayGame();
}


void CGameApplication::PlayerCardClicked(CCard* pCard)
{
	m_pManager->HumanPlayerMoved(m_pPlayer, pCard);
}


bool CGameApplication::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);

	if (m_screen != GAME_SCREEN_PLAY || !m_bWaitingPlayerMove)
	{
		return false;
	}

	static CCard* pCardUnder = nullptr;
	static CCard* pPressedCard = nullptr;
	CCard* pCard;

	switch (uMsg)
	{
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
		pCard = GetPlayerCardUnderMouse((float)LOWORD(lParam), (float)HIWORD(lParam));
		break;

	default:
		return false;
	}

	switch (uMsg)
	{
	case WM_MOUSEMOVE:
		if (pCard)
		{
			pCard->GetRenderable()->SetMouseOver(true);
			if (pCardUnder && pCardUnder != pCard)
			{
				pCardUnder->GetRenderable()->SetMouseOver(false);
			}
			pCardUnder = pCard;

			return true;
		}
		else if (pCardUnder)
		{
			pCardUnder->GetRenderable()->SetMouseOver(false);
			pCardUnder = nullptr;
		}

		break;

	case WM_LBUTTONDOWN:
		pPressedCard = pCard;
		break;

	case WM_LBUTTONUP:
		if (pPressedCard && pPressedCard == pCard)
		{
			PlayerCardClicked(pPressedCard);
		}
		break;
	}

	return false;
}


void CGameApplication::ConvertMouseCoordsToRay(float fX, float fY, XMFLOAT3* pf3Origin, XMFLOAT3* pf3Direction)
{
	static CDXGraphicsDriver* pGraphics = CDXGraphicsDriver::GetInstance();

	XMFLOAT4X4 view = m_pGameGraphics->GetViewMatrix();
	XMFLOAT4X4 projection = m_pGameGraphics->GetProjectionMatrix();
	XMFLOAT4X4 inverseView;
	XMMATRIX matInverseView = XMMatrixInverse(nullptr, XMLoadFloat4x4(&view));
	XMStoreFloat4x4(&inverseView, matInverseView);

	float fWidth = (float)pGraphics->GetClientWidth();
	float fHeight = (float)pGraphics->GetClientHeight();

	fX = ((2.0f * fX) / (float)fWidth) - 1.0f;
	fY = -1.0f * (((2.0f * fY) / (float)fHeight) - 1.0f);

	fX /= projection._11;
	fY /= projection._22;

	XMFLOAT3 direction;
	direction.x = (fX * inverseView._11) + (fY * inverseView._21) + inverseView._31;
	direction.y = (fX * inverseView._12) + (fY * inverseView._22) + inverseView._32;
	direction.z = (fX * inverseView._13) + (fY * inverseView._23) + inverseView._33;

	*pf3Origin = m_pGameGraphics->GetEye();
	XMStoreFloat3(pf3Direction, XMVector3Normalize(XMLoadFloat3(&direction)));
}


CCard* CGameApplication::GetPlayerCardUnderMouse(float fX, float fY)
{
	const CHand* pHand = m_pPlayer->GetHand();
	int iNumCards = pHand->GetNumberOfCards();

	CCard* pCardUnder = nullptr;
	float fDistance = 100.0f;

	XMFLOAT3 origin;
	XMFLOAT3 direction;
	ConvertMouseCoordsToRay(fX, fY, &origin, &direction);

	for (int i=0; i<iNumCards; ++i)
	{
		CCard* pCard = ((CHand)(*pHand))[i];
		CRenderableCard* pRenderable = pCard->GetRenderable();

		float f;
		if (pRenderable->RayIntersect(origin, direction, &f))
		{
			if (f < fDistance)
			{
				fDistance = f;
				pCardUnder = pCard;
			}
		}
	}

	return pCardUnder;
}


void CGameApplication::OnFrameRender(double fTime, float fElapsedTime)
{
	static CGameApplication* pApp = CGameApplication::GetInstance();
	pApp->m_pGameGraphics->OnRender(fTime, fElapsedTime);

	CDXTextRenderer* pText = CDXTextRenderer::GetInstance();
	
	WCHAR szScore[64];
	WCHAR szName[32];

	if (pApp->m_screen == GAME_SCREEN_PLAY)
	{
		const CHand* pSpareHand = pApp->m_pManager->GetRoundHand();
		pApp->m_pGameGraphics->PlaceHandCards(pSpareHand, 0.0f, 1.0f, true, true);
		pApp->m_pGameGraphics->RenderHand(pSpareHand, false);

		int iNumPlayers = pApp->m_pManager->GetNumberOfPlayers();

		pText->SetFontSize(20.0f);
		pText->BeginText(0.3f, 0.65f, false);
		pText->RenderText(L"Score: ", 0.22f, 0.65f);

		if (pApp->m_bTrumpGame)
		{
			if (!((CTrumpGameManager*)pApp->m_pManager)->IsTrumpSelectionPhase())
			{
				switch (((CTrumpGameManager*)pApp->m_pManager)->GetTrumpSuit())
				{
				case SUIT_CLUB:
					pText->RenderText(L"Trump Suit: CLUB", 0.42f, 0.65f);
					break;

				case SUIT_DIAMOND:
					pText->RenderText(L"Trump Suit: DIAMOND", 0.42f, 0.65f);
					break;

				case SUIT_HEARTS:
					pText->RenderText(L"Trump Suit: HEARTS", 0.42f, 0.65f);
					break;

				case SUIT_SPADE:
					pText->RenderText(L"Trump Suit: SPADE", 0.42f, 0.65f);
					break;
				}
			}
		}

		CPlayer* pPlayer = pApp->m_pManager->GetCurrentPlayer();
		if (pPlayer == pApp->m_pPlayer)
		{
			pText->RenderText(L"Your turn", 0.42f, 0.68f);

			if (pApp->m_bTrumpGame)
			{
				if (((CTrumpGameManager*)(pApp->m_pManager))->IsTrumpSelectionPhase())
				{
					pText->RenderText(L"Select trump suit.", 0.42f, 0.65f);
				}
			}
		}

		for (int i=0; i<iNumPlayers; ++i)
		{
			pPlayer = pApp->m_pManager->GetPlayer(i);

			MultiByteToWideChar(CP_ACP, 0, pPlayer->GetName().data(), -1, szName, 32);
			wsprintf(szScore, L"%s: %i", szName, pPlayer->GetRoundsWon());
			pText->RenderText(szScore);
		}
		pText->EndText();
	}
	else if (pApp->m_screen == GAME_SCREEN_FINISH)
	{
		PlayersVector vecStandings = pApp->m_pManager->GetStandings();
		
		pText->BeginText(0.45f, 0.4f, false);

		pText->SetFontSize(65.0f);
		pText->SetFontColor(XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
		
		pText->RenderText(L"GAME OVER", 0.35f, 0.2f);

		pText->SetFontSize(24.0f);
		pText->SetFontColor(XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
		for (int i=0; i<(int)vecStandings.size(); ++i)
		{
			CPlayer* pPlayer = vecStandings[i];

			MultiByteToWideChar(CP_ACP, 0, pPlayer->GetName().data(), -1, szName, 32);
			wsprintf(szScore, L"%s: %i", szName, pPlayer->GetRoundsWon());

			pText->RenderText(szScore);
		}
		pText->EndText();
	}
}


void CGameApplication::OnResize(float fWidth, float fHeight)
{
	static CGameApplication* pApp = CGameApplication::GetInstance();
	pApp->m_pDialogStart->OnResize(fWidth, fHeight);
	pApp->m_pGameGraphics->OnResize(fWidth, fHeight);
	pApp->m_pSaveButton->Move(fWidth - 100.0f, 20.0f);
}


bool CGameApplication::OnWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(hWnd);
	UNREFERENCED_PARAMETER(wParam);

	static CGameApplication* pApp = CGameApplication::GetInstance();

	return pApp->MessageHandler(uMsg, wParam, lParam);
}


bool CGameApplication::OnUIMessage(UINT uiControl, DX_UI_MESSAGE msg)
{
	static CGameApplication* pApp = CGameApplication::GetInstance();

	// Allow the startup dialog to handle the message first.
	if (pApp->m_pDialogStart->OnWindowMsg(uiControl, msg))
	{
		// Message handled?
		// Start the game.
		pApp->m_screen = GAME_SCREEN_PLAY;
		pApp->m_pSaveButton->SetVisible(true);

		// Did the user request to open the saved game.
		if (pApp->m_pDialogStart->GetLoadFile())
		{
			pApp->LoadGame();
			return true;
		}
		else // Otherwise start a new game.
		{
			pApp->CreateHumanPlayer(pApp->m_pDialogStart->GetName());
			pApp->BeginGame(pApp->m_pDialogStart->GetIsTrumpSuit(), pApp->m_pDialogStart->GetNumberOfPlayers()-1);
		}
	}
	else // It wasn't the startup dialog.
	{
		// Was it the save button?
		if (uiControl == DX_UI_PLAY_SAVE)
		{
			std::ofstream out(SAVE_GAME_FILE);
			WRITE_STREAM(out, pApp->m_bTrumpGame);

			pApp->m_pManager->Write(out, nullptr);
		}
	}

	return true;
}