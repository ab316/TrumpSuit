#pragma once

#include <DirectXCollision.h>

using namespace DirectX;

class CCardMesh;
class CCard;

class CRenderableCard
{
private:
	BoundingBox	m_bb;
	CCardMesh*	m_pMesh;
	CCard*		m_pParentCard;
	float		m_fCenterX;
	float		m_fCenterY;
	float		m_fZDepth;
	bool		m_bMouseOver;

public:
	void		SetCenter(float fX, float fY, float fZ);
	void		SetMouseOver(bool b)		{ m_bMouseOver = b;	};

public:
	float		GetCenterX()	const				{ return m_fCenterX;	};
	float		GetCenterY()	const				{ return m_fCenterY;	};
	float		GetZDepth()		const				{ return m_fZDepth;		};

public:
	void		Render();
	bool		RayIntersect(XMFLOAT3 origin, XMFLOAT3 direction, float* pfDistance);

public:
	CRenderableCard(CCard* pCard);
	~CRenderableCard();
};