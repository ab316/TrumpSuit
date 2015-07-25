#include "stdafx.h"
#include "Card.h"
#include "Deck.h"
#include "GameManager.h"
#include "HumanPlayer.h"
#include "NormalAIPlayer.h"

#include "GameApplication.h"

#pragma comment (lib, "Direct3D11.lib")
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")
#pragma comment (lib, "dxgi.lib")
#pragma comment (lib, "dxerr.lib")
#pragma comment (lib, "dxguid.lib")

#ifdef DX_DEBUG
#pragma comment (lib, "d3dx11d.lib")
#pragma comment (lib, "d3d9.lib")
#else
#pragma comment (lib, "d3dx11.lib")
#endif


int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	UNREFERENCED_PARAMETER(nShowCmd);

	CGameApplication* pApp = CGameApplication::GetInstance();
	pApp->Startup(hInstance);
	pApp->Run();
	pApp->Shutdown();

	return 0;
}


//void main()
//{
//	CGameManager game;
//	CHumanPlayer human;
//	human.SetName("AB");
//
//	CNormalAIPlayer x, y, z, w, u, v;
//	x.SetName("x");
//	y.SetName("y");
//	z.SetName("z");
//	w.SetName("w");
//	v.SetName("x");
//
//	//game.AddPlayer(&human);
//	game.AddPlayer(&x);
//	game.AddPlayer(&y);
//	game.AddPlayer(&z);
//	game.AddPlayer(&w);
//
//	game.BeginGame();
//	game.PlayRound();
//	game.PlayRound();
//	game.PlayRound();
//	game.PlayRound();
//	game.PlayRound();
//	
//	std::ofstream o("test.txt");
//	game.Write(o, nullptr);
//	o.close();
//
//	CGameManager game2;
//	std::ifstream i("test.txt");
//	game2.Read(i, nullptr);
//	while (game2.GetGameStatus() != STATUS_FINISHED)
//	{
//		game2.PlayRound();
//	}
//
//	while (game.GetGameStatus() != STATUS_FINISHED)
//	{
//		game.PlayRound();
//	}
//
//	PlayersVector stand = game.GetStandings();
//	PlayersVector stand2 = game2.GetStandings();
//
//	system("pause");
//}