#include "DXStdafx.h"
#include "DXUIStatic.h"
#include "DXTextRenderer.h"


namespace DXFramework
{ // DXFramework namespace begin

	CDXUIStatic::CDXUIStatic()	:	m_uiMaxLength(0),
									m_uiTextLength(0),
									m_szText(0),
									m_fFontSize(18.0f)
	{
		m_bActive = false;
	}


	CDXUIStatic::~CDXUIStatic()
	{
		DX_SAFE_DELETE_ARRAY(m_szText);
	}


	HRESULT CDXUIStatic::Create(LPCWSTR szText, float fX, float fY, float fFontSize)
	{
		HRESULT hr = S_OK;

		m_uiTextLength = (UINT)wcslen(szText);
		m_uiMaxLength = (UINT)wcslen(szText);

		if (!m_uiMaxLength || !szText)
		{
			return E_INVALIDARG;
		}

		m_szText = new WCHAR[m_uiMaxLength + 1];
		wcscpy_s(m_szText, m_uiMaxLength+1, szText);

		m_fFontSize = fFontSize;
		m_rect.fLeft = fX;
		m_rect.fTop = fY;
		m_rect.fRight = 100.0f;
		m_rect.fBottom = 100.0f;

		return hr;
	}


	void CDXUIStatic::SetText(LPCWSTR szText)
	{
		if (szText)
		{
			UINT uiLength = (UINT)wcslen(szText);
			if (uiLength <= m_uiMaxLength)
			{
				wcscpy_s(m_szText, m_uiMaxLength+1, szText);
				m_uiTextLength = uiLength;
			}
			else
			{
				DX_SAFE_DELETE_ARRAY(m_szText);
				m_szText = new WCHAR[uiLength + 1];
				m_uiMaxLength = uiLength;
				m_uiTextLength = uiLength;
			}
		}
	}


	void CDXUIStatic::RenderText()
	{
		static CDXTextRenderer* pText = CDXTextRenderer::GetInstance();
		pText->SetFontSize(m_fFontSize);
		pText->SetFontColor(m_f4Color);
		pText->RenderText(m_szText, m_rect.fLeft, m_rect.fTop, 0, true);
	}

} // DXFramework namespace end