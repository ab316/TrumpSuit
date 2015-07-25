#include "DXStdafx.h"
#include "DXUIElement.h"


namespace DXFramework
{ // DXFramework namespace begin

	IDXUIElement::IDXUIElement()	:	m_bActive(true),
										m_bVisible(true),
										m_bMouseFocusable(false),
										m_bKeyboardFocusable(false),
										m_bFocussed(false),
										m_bRenderText(true),
										m_cZOrder(DX_UIELEMENT_DEFAULT_ZORDER),
										m_State(DX_UIELEMENT_STATE_NORMAL),
										m_pUIManager(nullptr),
										m_pSprite(nullptr),
										m_pMouseHandler(nullptr),
										m_pKeyboardHandler(nullptr),
										m_f4Color(1.0f, 1.0f, 1.0f, 1.0f),
										m_rectTexture(0, 0, 1.0f, 1.0f)
	{
	}


	IDXUIElement::~IDXUIElement()
	{
	}

} // DXFramework namespace end