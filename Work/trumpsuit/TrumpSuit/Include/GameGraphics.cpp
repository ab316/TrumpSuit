#include "stdafx.h"
#include "DX3DDriver.h"
#include "DXGraphicsDriver.h"
#include "CardMesh.h"
#include "RenderableCard.h"
#include "Hand.h"
#include "Player.h"
#include "GameGraphics.h"
#include "GameManager.h"

using namespace DXFramework;

CGameGraphics::CGameGraphics()
{
	m_pCardMesh = CCardMesh::GetInstance();
}


CGameGraphics::~CGameGraphics()
{
}


HRESULT CGameGraphics::Create()
{
	HRESULT hr = S_OK;

	DX_V_RETURN(m_pCardMesh->Create());
	
	m_f3Eye = XMFLOAT3(0.0f, 0.0f, -10.0f);
	XMFLOAT3 at(0.0f, 0.0f, 0.0f);
	XMFLOAT3 up(0.0f, 1.0f, 0.0f);

	XMStoreFloat4x4(&m_matView, XMMatrixLookAtLH(XMLoadFloat3(&m_f3Eye), XMLoadFloat3(&at), XMLoadFloat3(&up)));
	m_pCardMesh->SetView(m_matView);

	return hr;
}


void CGameGraphics::Destroy()
{
	m_pCardMesh->Destroy();
}


void CGameGraphics::OnResize(float fWidth, float fHeight)
{
	XMStoreFloat4x4(&m_matProjection, XMMatrixPerspectiveFovLH(XM_PIDIV4, fWidth/fHeight, 0.01f, 30.0f));
	m_pCardMesh->SetProjection(m_matProjection);
}


void CGameGraphics::OnRender(double fTime, float fElapsedTime)
{
	UNREFERENCED_PARAMETER(fTime);
	UNREFERENCED_PARAMETER(fElapsedTime);

	static CDXGraphicsDriver* pGraphics = CDXGraphicsDriver::GetInstance();
	static CDX3DDriver* p3d = CDX3DDriver::GetInstance();

	static float backColor[4] = { 0.6f, 0.6f, 0.6f, 1.0f };

	pGraphics->Clear(true, backColor);
	
	for (int i=0; i<(int)m_players.size(); ++i)
	{
		RenderPlayerHand(m_players[i]);
	}
}


void CGameGraphics::RenderPlayerHand(const CPlayer* pPlayer)
{
	bool bBackSide = pPlayer->GetPlayerType() != PLAYER_TYPE_HUMAN;
	RenderHand(pPlayer->GetHand(), bBackSide);
}


void CGameGraphics::RenderHand(const CHand* pHand, bool  bBackSide)
{
	int iNumCards = pHand->GetNumberOfCards();

	m_pCardMesh->SetBackSide(bBackSide);
	for (int i=0; i<iNumCards; ++i)
	{
		// This line is taking 19.7% of total application time because a new CHand instance is
		// being created every time.
		const CCard* pCard = ((CHand)*pHand)[i];
		pCard->GetRenderable()->Render();
	}
}


void CGameGraphics::PlaceHandCards(const CHand* pHand, float fCentreX, float fCentreY, bool bHorizontal, bool bClose)
{
	int iNumCards = pHand->GetNumberOfCards();

	float fOffset;
	if (bClose)
	{
		fOffset = 3.0f / iNumCards;
	}
	else
	{
		fOffset = 5.0f / iNumCards;
	}
	float f;
	
	if (bHorizontal)
	{
		f = fCentreX - (fOffset * (float)(iNumCards / 2));
	}
	else
	{
		f = fCentreY - (fOffset * (float)(iNumCards / 2));
	}

	for (int i=0; i<iNumCards; ++i)
	{
		CRenderableCard* pCard = ((CHand)*pHand)[i]->GetRenderable();
		float fX;
		float fY;
		float fZ;

		if (bHorizontal)
		{
			fX = f;
			fY = fCentreY;
			fZ = i * -0.003f;
		}
		else
		{
			fX = fCentreX;
			fY = f;
			fZ = i * 0.005f;
		}

		pCard->SetCenter(fX, fY, fZ);
		f += fOffset;
	}
}