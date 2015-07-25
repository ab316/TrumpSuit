#pragma once

#include "Card.h"

using namespace DXFramework;

struct CardVertex
{
	XMFLOAT3	f3Position;
	XMFLOAT3	f3Normal;
	XMFLOAT2	f2TextureCoords;
};


struct CardConstantBuffer
{
	XMFLOAT4X4		matWorld;
	XMFLOAT4X4		matView;
	XMFLOAT4X4		matProjection;
	XMFLOAT4		f4Color;
};


class CCardMesh
{
private:
	CardConstantBuffer		m_ConstantBuffer;
	CDXHardwareBuffer*		m_pVertexBuffer;
	CDXHardwareBuffer*		m_pIndexBuffer;
	CDXHardwareBuffer*		m_pCBTransforms;
	CDXInputLayout*			m_pInputLayout;
	CDXShader*				m_pVertexShader;
	CDXShader*				m_pPixelShader;
	CDXTexture*				m_pTextures[53];
	UINT					m_uiTextureId;
	bool					m_bBackSide;

public:
	HRESULT		Create();
	void		Destroy();

public:
	void		Render();

public:
	void	SetView(XMFLOAT4X4 view)		{ m_ConstantBuffer.matView = view;			};
	void	SetProjection(XMFLOAT4X4 proj)	{ m_ConstantBuffer.matProjection = proj;	};
	void	SetWorld(XMFLOAT4X4 world)		{ m_ConstantBuffer.matWorld = world;		};
	void	SetBackSide(bool b)				{ m_bBackSide = b;							};
	void	SetHighlight(bool b);
	void	UpdateTransform();
	void	SetTexture(CARD_SUIT suit, CARD_VALUE value);

public:
	static CCardMesh*	GetInstance()
	{
		static CCardMesh mesh;
		return &mesh;
	}

private:
	CCardMesh();
public:
	~CCardMesh();
};