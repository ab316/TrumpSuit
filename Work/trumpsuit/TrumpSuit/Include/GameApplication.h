#pragma once

#include "DXUIManager.h"

class CGameManager;
class CStartDialog;
class CHumanPlayer;
class CCardMesh;
class CGameGraphics;

namespace DXFramework
{
	class CDXBaseApplication;
	class CDXGraphicsDriver;
	class CDX3DDriver;
	class CDXUIButton;
}

using namespace DXFramework;


enum GAME_SCREEN
{
	GAME_SCREEN_START,
	GAME_SCREEN_PLAY,
	GAME_SCREEN_FINISH,
};


class CGameApplication
{
private:
	// The directX application.
	CDXBaseApplication*		m_pDXApp;
	// The save button.
	CDXUIButton*			m_pSaveButton;
	CGameGraphics*			m_pGameGraphics;
	CGameManager*			m_pManager;
	CStartDialog*			m_pDialogStart;
	CHumanPlayer*			m_pPlayer;
	GAME_SCREEN				m_screen;
	bool					m_bWaitingPlayerMove;
	bool					m_bTrumpGame;

public:
	void SetWaitingForPlayer(bool b)
	{
		m_bWaitingPlayerMove = b;
	}

public:
	bool	Startup(HINSTANCE hInstance);
	void	Run();
	void	Shutdown();
	void	Reset();

public:
	void	CreateHumanPlayer(LPCWSTR szName);
	void	BeginGame(bool bTrumpSuit, int iAIPlayers);
	void	LoadGame();
	void	GameFinished()		{ m_screen = GAME_SCREEN_FINISH;	};

public:
	bool	OnMouseMove(float fX, float fY);
	bool	MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam);
	CCard*	GetPlayerCardUnderMouse(float fX, float fY);
	void	ConvertMouseCoordsToRay(float fX, float fY, XMFLOAT3* f3Origin, XMFLOAT3* f3Direction);
	void	PlayerCardClicked(CCard* pCard);

public:
	static	void	OnFrameRender(double fTime, float fElapsedTime);
	static	void	OnResize(float fWidth, float fHeight);
	static	bool	OnWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static	bool	OnUIMessage(UINT uiControl, DX_UI_MESSAGE msg);

public:
	static CGameApplication* GetInstance()
	{
		static CGameApplication app;
		return &app;
	}

private:
	CGameApplication();

public:
	~CGameApplication();
};