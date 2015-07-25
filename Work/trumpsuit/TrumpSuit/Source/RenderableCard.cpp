#include "stdafx.h"
#include "DX3DDriver.h"
#include "DXGraphicsDriver.h"
#include "CardMesh.h"
#include "RenderableCard.h"


CRenderableCard::CRenderableCard(CCard* pCard)	:	m_bMouseOver(false)
{
	m_pParentCard = pCard;
	m_pMesh = CCardMesh::GetInstance();
}


CRenderableCard::~CRenderableCard()
{
}


void CRenderableCard::SetCenter(float fX, float fY, float fZ)
{
	m_fCenterX = fX;
	m_fCenterY = fY;
	m_fZDepth = fZ;
	m_bb = BoundingBox(XMFLOAT3(fX, fY, fZ), XMFLOAT3(0.5f, 0.7f, 0.01f));
}


bool CRenderableCard::RayIntersect(XMFLOAT3 origin, XMFLOAT3 direction, float* pfDistance)
{
	XMVECTOR o, d;
	o = XMLoadFloat3(&origin);
	d = XMLoadFloat3(&direction);

	float fDist;
	bool bIntersects = m_bb.Intersects(o, d, fDist);
	*pfDistance = fDist;

	return bIntersects;
}


void CRenderableCard::Render()
{
	XMFLOAT4X4 world;
	XMStoreFloat4x4(&world, XMMatrixTranslation(m_fCenterX, m_fCenterY, m_fZDepth));
	m_pMesh->SetWorld(world);
	m_pMesh->SetTexture(m_pParentCard->GetSuit(), m_pParentCard->GetValue());
	m_pMesh->SetHighlight(m_bMouseOver);
	m_pMesh->UpdateTransform();

	m_pMesh->Render();
}