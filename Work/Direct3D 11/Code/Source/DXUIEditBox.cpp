#include "DXStdafx.h"
#include "DX3DDriver.h"
#include "DXSprite.h"
#include "DXUIEditBox.h"
#include "DXTextRenderer.h"
#include "DXMouseHandler.h"
#include "DXKeyboardHandler.h"

namespace DXFramework
{ // DXFramework namespace begin


	CDXUIEditBox::CDXUIEditBox()	:	m_bShowCaret(true),
										m_bCaretState(false),
										m_bBlinkCaret(true),
										m_uiMaxLength(0),
										m_uiTextLength(0),
										m_uiCaretPos(0),
										m_uiMaxCharsCanBeSeen(0),
										m_uiFirstCharPosSeen(0),
										m_uiLastCharPosSeen(0),
										m_szText(0),
										m_szTemp(0),
										m_rectCaretTexture(0.1f, 0.51f, 0.11f, 0.52f),
										m_f4CaretColor(0, 0, 0, 1.0f),
										m_f4NormalTint(1.0f, 1.0f, 1.0f, 1.0f),
										m_f4DisabledTint(0.5f, 0.5f, 0.5f, 1.0f),
										m_fCaretWidth(2.0f),
										m_fCaretBlinkTimer(0)
	{
		m_bKeyboardFocusable = true;
		m_pMouseHandler = new CDXUIEditBoxMouseHandler(this);
		m_pKeyboardHandler = new CDXUIEditBoxKeyboardHandler(this);

		m_rectTexture.fLeft = 0.018f;
		m_rectTexture.fTop = 0.322f;
		m_rectTexture.fRight = 0.965f;
		m_rectTexture.fBottom = 0.479f;

		m_f4Color = m_f4NormalTint;
		m_f4TextColor = XMFLOAT4(0, 0, 0, 1);
	}


	CDXUIEditBox::~CDXUIEditBox()
	{
		DX_SAFE_DELETE(m_szText);
		DX_SAFE_DELETE(m_szTemp);
		DX_SAFE_DELETE(m_pfTextOffsets);
		DX_SAFE_DELETE(m_pMouseHandler);
		DX_SAFE_DELETE(m_pKeyboardHandler);
	}


	HRESULT CDXUIEditBox::Create(UINT maxLength, float fUpperLeftX, float fUpperLeftY, float fWidth, float fHeight)
	{
		HRESULT hr = S_OK;

		m_uiMaxLength = maxLength;

		m_szText = new WCHAR[maxLength+1];
		m_szTemp = new WCHAR[maxLength+1];
		m_pfTextOffsets = new float[maxLength+1];
		
		m_rect.fLeft = fUpperLeftX;
		m_rect.fTop = fUpperLeftY;
		m_rect.fRight = fUpperLeftX + fWidth;
		m_rect.fBottom = fUpperLeftY + fHeight;
		OnDimensionsUpdate();

		// Zero the first offset.
		m_pfTextOffsets[0] = 0;
		m_szTemp[0] = 0;
		
		// Zero the text data.
		WCHAR* p = m_szText;
		WCHAR* end = m_szText + maxLength + 1;
		while (p != end)
		{
			*p = 0;
			++p;
		}

		return hr;
	}


	void CDXUIEditBox::OnDimensionsUpdate()
	{
		m_fWidth = m_rect.fRight - m_rect.fLeft;
		m_fHeight = m_rect.fBottom - m_rect.fTop;
		
		float fdx = (5.0f / 100.0f) * m_fWidth;
		float fdy = (10.0f / 40.0f) * m_fHeight;
		m_rectText.fLeft = m_rect.fLeft + fdx;
		m_rectText.fTop = m_rect.fTop + fdy;
		m_rectText.fRight = m_rect.fRight - fdx * 0.8f;
		m_rectText.fBottom = m_rect.fBottom - fdy * 0.6f;

		m_fFontSize = (m_rectText.fBottom - m_rectText.fTop);

		m_rectCaret = m_rectText;
		m_rectCaret.fTop += 2.0f;
		m_rectCaret.fBottom -= fdy * 0.6f;


		if (!UpdateMaxCharsSeen())
		{
			m_uiMaxCharsCanBeSeen = m_uiMaxLength;
		}
	}


	bool CDXUIEditBox::UpdateMaxCharsSeen()
	{
		// Find how many full characters can we display in the edit box at once.
		float fRequiredWidth = (m_rectText.fRight - m_rectText.fLeft) / (0.5f * (float)(CDX3DDriver::GetInstance()->GetBackbufferWidth()));

		CDXTextRenderer::GetInstance()->GetTextMetricsW(m_szText, m_fFontSize, m_uiTextLength, m_pfTextOffsets);

		for (UINT i=0; i<=m_uiTextLength; i++)
		{
			if (m_pfTextOffsets[i] - m_pfTextOffsets[0] >= fRequiredWidth)
			{
				if (i > 0)
				{
					m_uiMaxCharsCanBeSeen = i - 1;
				}
				else
				{
					m_uiMaxCharsCanBeSeen = 0;
					DX_DEBUG_OUTPUT0(L"WARNING!!! The given width can't display any characters.");
				}

				return true;
			}
		}

		return false;
	}


	void CDXUIEditBox::RenderText()
	{
		static CDXTextRenderer* pText = CDXTextRenderer::GetInstance();

		//WCHAR output[300];
		//swprintf_s(output, L"%f", m_pfTextOffsets[m_uiTextLength-1] - m_pfTextOffsets[0]);
		//pText->RenderText(output, 5, 300, 0, true);

		pText->SetFontColor(m_f4TextColor);
		pText->SetFontSize(m_fFontSize);
		pText->RenderText(m_szTemp, m_rectText.fLeft, m_rectText.fTop, 0, true);
	}


	void CDXUIEditBox::Render()
	{
		static CDX3DDriver* p3d = CDX3DDriver::GetInstance();
		m_pSprite->BatchRenderSprite(&m_rect, &m_rectTexture, &m_f4Color, 0);

		if (m_bActive && m_bFocussed && m_bCaretState && m_bShowCaret)
		{
			float fDelta = 0.0f;
			if (m_uiCaretPos)
			{
				UINT iPos = m_uiCaretPos - m_uiFirstCharPosSeen - 1;
				if (iPos == (UINT)(-1))
				{
					iPos = 0;
				}
				fDelta = (p3d->GetBackbufferWidth() * 0.5f) * m_pfTextOffsets[iPos];
			}

			//for (int i=0; i<m_uiTextLength; i++)
			{
				m_rectCaret.fLeft = m_rectText.fLeft + fDelta;
				m_rectCaret.fRight = m_rectCaret.fLeft + m_fCaretWidth;
				//m_rectCaret.fLeft = m_rectText.fLeft + 400.0f * m_pfTextOffsets[i];
				//m_rectCaret.fRight = m_rectCaret.fLeft + m_fCaretWidth;
				m_pSprite->BatchRenderSprite(&m_rectCaret, &m_rectCaretTexture, &m_f4CaretColor, ((float)m_cZOrder - 0.5f)/256.0f);
			}
		}
	}


	void CDXUIEditBox::Update(float fElapsedTime)
	{
		switch (m_State)
		{
		case DX_UIELEMENT_STATE_DISABLED:
			m_f4Color = m_f4DisabledTint;
			break;

		default:
			m_f4Color = m_f4NormalTint;
			break;
		}

		if (m_bFocussed && m_bBlinkCaret)
		{
			m_fCaretBlinkTimer += fElapsedTime;
			if (m_fCaretBlinkTimer >= 0.5f)
			{
				m_fCaretBlinkTimer = 0;
				m_bCaretState = !m_bCaretState;
			}
		}
	}

} // DXFramework namespace end