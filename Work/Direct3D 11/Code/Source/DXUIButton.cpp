#include "DXStdafx.h"
#include "DXSprite.h"
#include "DX3DDriver.h"
#include "DXUIButton.h"
#include "DXMouseHandler.h"
#include "DXTextRenderer.h"

namespace DXFramework
{ // DXFramework namespace begin

	CDXUIButton::CDXUIButton()
	{
		m_bMouseFocusable = true;
		m_pMouseHandler = new CDXUIButtonMouseHandler(this);

		m_f4NormalTint = XMFLOAT4A(1.0f, 1.0f, 1.0f, 1.0f);
		m_f4MouseDownTint = XMFLOAT4A(0.4f, 0.4f, 0.4f, 1.0f);
		m_f4MouseOverTint = XMFLOAT4A(1.5f, 1.5f, 1.5f, 1.0f);
		m_f4DisabledTint = XMFLOAT4A(0.5f, 0.5f, 0.5f, 1.0f);
		m_f4TextColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		m_f4Color = m_f4NormalTint;
		m_f4TextColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		m_rectTexture.fLeft = 0.001f;
		m_rectTexture.fTop = 0.006;
		m_rectTexture.fRight = 0.526f;
		m_rectTexture.fBottom = 0.203f;
	}


	CDXUIButton::~CDXUIButton()
	{
	}

	
	HRESULT CDXUIButton::Create(WCHAR* szCaption, float fUpperLeftX, float fUpperLeftY, float fWidth, float fHeight)
	{
		HRESULT hr = S_OK;

		m_rect.fLeft = fUpperLeftX;
		m_rect.fTop = fUpperLeftY;
		m_fWidth = fWidth;
		m_fHeight = fHeight;
		m_rect.fRight = fUpperLeftX + fWidth;
		m_rect.fBottom = fUpperLeftY + fHeight;
		m_rectButton = m_rect;
		m_fFontSize = m_fHeight * 0.5f;

		wcscpy_s(m_szCaption, DX_MAX_NAME, szCaption);

		return hr;
	}


	void CDXUIButton::Render()
	{
		//float fDepth = (float)m_cZOrder / 256.0f;
		m_pSprite->BatchRenderSprite(&m_rectButton, &m_rectTexture, &m_f4Color, 0.0f);
	}


	void CDXUIButton::RenderText()
	{
		static CDXTextRenderer* pText = CDXTextRenderer::GetInstance();

		float* pOffsets = new float[wcslen(m_szCaption)];
		pText->GetTextMetricsW(m_szCaption, m_fFontSize, wcslen(m_szCaption), pOffsets);
		float fWidth = (pOffsets[wcslen(m_szCaption)-1] - pOffsets[0]) * (0.5f * (float)CDX3DDriver::GetInstance()->GetBackbufferWidth());
		float fX = m_rectButton.fLeft + fWidth / 2.0f;
		fX = m_rectButton.fLeft + (m_rectButton.fRight - m_rectButton.fLeft) / wcslen(m_szCaption);
		float fY = m_rectButton.fTop + (m_rectButton.fBottom - m_rectButton.fTop) / 2.0f - m_fFontSize / 3.0f;

		//float fDepth = ((float)(m_cZOrder) - 0.5f) / 256.0f;
		pText->SetFontColor(m_f4TextColor);
		pText->SetFontSize(m_fFontSize);
		pText->RenderText(m_szCaption, fX, fY, 0, true);
		//pText->RenderText(m_szCaption, m_rectButton.fLeft+5.0f, m_rectButton.fTop+(m_fFontSize/3.0f), 0, true);
	}


	void CDXUIButton::Update(float fDelta)
	{
		XMFLOAT4 reqColor;
	
		float fOffsetX = 0;
		float fOffsetY = 0;

		switch (m_State)
		{
		case DX_UIELEMENT_STATE_DISABLED:
			reqColor = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
			break;

		case DX_UIELEMENT_STATE_MOUSEOVER:
			reqColor = m_f4MouseOverTint;
			fOffsetX = -2;
			fOffsetY = -2;
			break;
				
		case DX_UIELEMENT_STATE_NORMAL:
			reqColor = m_f4NormalTint;
			break;

		case DX_UIELEMENT_STATE_PRESSED:
			reqColor = m_f4MouseDownTint;
			fOffsetX = 3;
			fOffsetY = 3;
			break;

		}

		DXRect rect = m_rect;
		DX_OFFSET_RECT(&rect, fOffsetX, fOffsetY);
		m_rectButton = rect;
		XMVECTOR curColor = XMLoadFloat4(&m_f4Color);
		XMStoreFloat4(&m_f4Color, XMVectorLerp(curColor, XMLoadFloat4(&reqColor), 10.0f * fDelta));
	

		UNREFERENCED_PARAMETER(fDelta);
	}

} // DXFramework namespace end