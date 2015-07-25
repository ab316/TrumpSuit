#pragma once

class CCardMesh;
class CHand;
class CPlayer;

using namespace DXFramework;

// Houses methods helpful for rendering cards.
class CGameGraphics
{
private:
	// The camera eye.
	XMFLOAT3		m_f3Eye;
	// View matrix.
	XMFLOAT4X4		m_matView;
	// Projection matrix.
	XMFLOAT4X4		m_matProjection;
	// The card mesh.
	CCardMesh*		m_pCardMesh;

public:
	std::vector<CPlayer*>	m_players;

public:
	HRESULT		Create();
	void		Destroy();

public:
	XMFLOAT3	GetEye()				const		{ return m_f3Eye;			};
	XMFLOAT4X4	GetViewMatrix()			const		{ return m_matView;			};
	XMFLOAT4X4	GetProjectionMatrix()	const		{ return m_matProjection;	};

public:
	// Updates the projection matrix which depends on the aspect ratio of the rendering
	// region. Should be called whenever the window is resized.
	void		OnResize(float fWidth, float fHeight);
	// Clears the backbuffer and renders the players' cards.
	void		OnRender(double fTime, float fElapsedTime);
	// Sets the position of the cards in a hand.
	void		PlaceHandCards(const CHand* pHand, float fCentreX, float fCentreY, bool bHorizontal, bool bClose=false);
	// Renders a player's cards.
	void		RenderPlayerHand(const CPlayer* pPlayer);
	// Renders a hand.
	void		RenderHand(const CHand* pHand, bool  bBackSide);

public:
	CGameGraphics();
	~CGameGraphics();
};