#include "DXStdafx.h"
#include "DXSprite.h"
#include "DXTextRenderer.h"
#include "DXMouseHandler.h"
#include "DXUIScrollBar.h"
#include "DXUIComboBox.h"


namespace DXFramework
{ // DXFramework namespace begin

	CDXUIComboBox::CDXUIComboBox()
	{
		m_bMouseFocusable = true;
		m_pScrollBar = new CDXUIScrollBar();
	}


	CDXUIComboBox::~CDXUIComboBox()
	{
		DX_SAFE_DELETE(m_pScrollBar);
	}


	HRESULT CDXUIComboBox::Create(float fUpperLeftX, float fUpperLeftY, float Height, float fFontSize)
	{
		HRESULT hr = S_OK;

		return hr;
	}


	void CDXUIComboBox::RenderText()
	{
	}


	void CDXUIComboBox::Render()
	{
	}


	void CDXUIComboBox::Update(float fElapsedTime)
	{
		UNREFERENCED_PARAMETER(fElapsedTime);
	}
	
} // DXFramework namespace end