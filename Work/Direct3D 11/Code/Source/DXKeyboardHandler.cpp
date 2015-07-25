#include "DXStdafx.h"
#include "DXUIEditBox.h"
#include "DXTextRenderer.h"
#include "DXKeyboardHandler.h"


namespace DXFramework
{ // DXFramework namespace begin

	static CDXTextRenderer* gs_pTextRenderer = CDXTextRenderer::GetInstance();


	CDXUIEditBoxKeyboardHandler::CDXUIEditBoxKeyboardHandler(CDXUIEditBox* pControl) :	
																				m_pControl(pControl)
	{
		m_bSendWCHARMessages = true;

		puiMaxLength = &m_pControl->m_uiMaxLength;
		puiTextLength = &m_pControl->m_uiTextLength;
		puiCaretPos = &m_pControl->m_uiCaretPos;
		puiFirstPos = &m_pControl->m_uiFirstCharPosSeen;
		puiLastPos = &m_pControl->m_uiLastCharPosSeen;
		puiMaxCharsAtOnce = &m_pControl->m_uiMaxCharsCanBeSeen;
	}


	CDXUIEditBoxKeyboardHandler::~CDXUIEditBoxKeyboardHandler()
	{
	}


	void CDXUIEditBoxKeyboardHandler::UpdateText()
	{
		wcscpy_s(m_pControl->m_szTemp, *puiMaxLength+1, m_pControl->m_szText + *puiFirstPos);
		m_pControl->m_szTemp[*puiMaxCharsAtOnce] = 0;
	}


	bool CDXUIEditBoxKeyboardHandler::OnKeyDown(UINT virtualKeyCode)
	{
		switch (virtualKeyCode)
		{
		case VK_BACK:
			if (*puiCaretPos > 0)
			{
				if (*puiCaretPos == *puiTextLength)
				{
					m_pControl->m_szText[*puiCaretPos-1] = 0;
				}
				else
				{
					WCHAR* pszTemp = m_pControl->m_szTemp;
					wcscpy_s(pszTemp, *puiMaxLength + 1, m_pControl->m_szText);
					pszTemp[*puiCaretPos-1] = 0;
					wcscpy_s(pszTemp + (*puiCaretPos-1), 1 + *puiMaxLength - *puiCaretPos, m_pControl->m_szText + *puiCaretPos);
					wcscpy_s(m_pControl->m_szText, *puiMaxLength + 1, pszTemp);
				}

				--m_pControl->m_uiTextLength;
				--(*puiLastPos);
				StepCaretBackward();

				gs_pTextRenderer->GetTextMetricsW(m_pControl->m_szText, m_pControl->m_fFontSize, m_pControl->m_uiTextLength, m_pControl->m_pfTextOffsets);
			}
			break;

		case VK_LEFT:
			StepCaretBackward();
			break;

		case VK_RIGHT:
			StepCaretForeward();
			break;
		}

		return true;
	}


	bool CDXUIEditBoxKeyboardHandler::OnKeyUp(UINT virtualKeyCode)
	{
		UNREFERENCED_PARAMETER(virtualKeyCode);
		return false;
	}


	bool CDXUIEditBoxKeyboardHandler::OnKey(WCHAR szKey)
	{

		if (szKey >= ' ' && szKey <= '~')
		{
			if (*puiTextLength < *puiMaxLength)
			{
				if (*puiCaretPos == *puiTextLength)
				{
					m_pControl->m_szText[*puiCaretPos] = szKey;
					m_pControl->m_szText[*puiCaretPos+1] = 0;
				}
				else
				{
					WCHAR* pszTemp = m_pControl->m_szTemp;
					wcscpy_s(pszTemp, *puiMaxLength+1, m_pControl->m_szText);
					pszTemp[*puiCaretPos] = szKey;
					
					wcscpy_s(pszTemp+*puiCaretPos+1, (*puiMaxLength - *puiCaretPos), m_pControl->m_szText+*puiCaretPos);
					wcscpy_s(m_pControl->m_szText, *puiMaxLength+1, pszTemp);
				}

				++m_pControl->m_uiTextLength;
				++(*puiLastPos);
				StepCaretForeward(false);

				m_pControl->UpdateMaxCharsSeen();
				//gs_pTextRenderer->GetTextMetricsW(m_pControl->m_szText, m_pControl->m_fFontSize, *puiTextLength, m_pControl->m_pfTextOffsets);
			}
		}
		return true;
	}

} // DXFramework namespace end