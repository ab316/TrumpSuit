#include "DXStdafx.h"
#include "DXMouseHandler.h"
#include "DXKeyboardHandler.h"
#include "DXUIButton.h"
#include "DXUIEditBox.h"
#include "DXUICheckBox.h"
#include "DXUIRadioButton.h"
#include "DXUIScrollBar.h"
#include "DXUIManager.h"

namespace DXFramework
{ // DXFramework namespace begin

	/////////////////////// BUTTON MOUSE HANDLER /////////////////////////////

	bool CDXUIButtonMouseHandler::OnMouseMove(DXPoint point)
	{
		// Can't use DX_POINT_IN_RECT here because control's may be overlapping and the
		// mouse is only on this control if it the foremost according to Z order.
		if (m_pControl->m_pUIManager->GetElementAtPoint(point) == m_pControl)
		{
			if (m_bMouseDown)
			{
				m_pControl->SetState(DX_UIELEMENT_STATE_PRESSED);
			}
			else
			{
				m_pControl->SetState(DX_UIELEMENT_STATE_MOUSEOVER);
			}
		}
		else
		{
			m_pControl->SetState(DX_UIELEMENT_STATE_NORMAL);
		}

		return true;
	}


	bool CDXUIButtonMouseHandler::OnLeftMouseDown(DXPoint point)
	{
		UNREFERENCED_PARAMETER(point);
		m_pControl->SetState(DX_UIELEMENT_STATE_PRESSED);
		m_bMouseDown = true;

		return true;
	}


	bool CDXUIButtonMouseHandler::OnLeftMouseUp(DXPoint point)
	{
		bool bReturnValue = false;
		// We need to check if the control under the mouse is this control because
		// this method is called by the ui manager's message handler on the control that
		// was pressed with the left mouse button. When the button is released the mouse
		// may not be on this control. Can't use DX_POINT_IN_RECT because it will not work
		// correctly if the control is overlapped. GetElementAtPoint will return the
		// foremost control according to Z-order.
		if (m_pControl->m_pUIManager->GetElementAtPoint(point) == m_pControl)
		{
			m_pControl->SetState(DX_UIELEMENT_STATE_MOUSEOVER);
			bReturnValue = m_pControl->m_pUIManager->CallUIMessageCallback(m_pControl, DX_UI_MESSAGE_LEFT_MOUSE_CLICK);
		}
		else
		{
			m_pControl->SetState(DX_UIELEMENT_STATE_NORMAL);
		}

		m_bMouseDown = false;

		return bReturnValue;
	}



	//////////////////// EDIT BOX MOUSE HANDLER //////////////////////

	bool CDXUIEditBoxMouseHandler::OnMouseMove(DXPoint point)
	{
		UNREFERENCED_PARAMETER(point);
		return true;
	}


	// Needs work.
	bool CDXUIEditBoxMouseHandler::OnLeftMouseDown(DXPoint point)
	{
		if (DX_POINT_IN_RECT(point, m_pControl->m_rectText))
		{
			m_pControl->m_pUIManager->SetKeyboardFocus(m_pControl);
			m_pControl->SetFocussed(true);
			m_pControl->m_fCaretBlinkTimer = 0;
			m_pControl->m_bCaretState = true;

			if (m_pControl->m_uiTextLength)
			{
				float* pOffsets = m_pControl->m_pfTextOffsets + m_pControl->m_uiFirstCharPosSeen;
				float fMouseOffsetX = point.x - m_pControl->m_rectText.fLeft;
				UINT numOffsets = m_pControl->m_uiLastCharPosSeen - m_pControl->m_uiFirstCharPosSeen;
				UINT iClosestPos = 0;
				for (UINT i=0; i<=numOffsets; i++)
				{
					if (fMouseOffsetX > pOffsets[i] - pOffsets[0])
					{
						iClosestPos = i;
					}
				}

				m_pControl->m_uiCaretPos = m_pControl->m_uiFirstCharPosSeen + iClosestPos;
				if (m_pControl->m_uiCaretPos == m_pControl->m_uiFirstCharPosSeen)
				{
					((CDXUIEditBoxKeyboardHandler*)(m_pControl->m_pKeyboardHandler))->StepCaretBackward();
				}
				else if (m_pControl->m_uiCaretPos == m_pControl->m_uiLastCharPosSeen + 1)
				{
					((CDXUIEditBoxKeyboardHandler*)(m_pControl->m_pKeyboardHandler))->StepCaretForeward();
				}
			}
		}

		return true;
	}


	bool CDXUIEditBoxMouseHandler::OnLeftMouseUp(DXPoint point)
	{
		UNREFERENCED_PARAMETER(point);
		return true;
	}



	///////////////////////// CHECKBOX MOUSE HANDLER ///////////////////////////
	
	bool CDXUICheckBoxMouseHandler::OnMouseMove(DXPoint point)
	{
		if (m_pControl->m_pUIManager->GetElementAtPoint(point) == m_pControl)
		{
			m_pControl->SetState(DX_UIELEMENT_STATE_MOUSEOVER);
		}
		else
		{
			m_pControl->SetState(DX_UIELEMENT_STATE_NORMAL);
		}

		return true;
	}


	bool CDXUICheckBoxMouseHandler::OnLeftMouseDown(DXPoint point)
	{
		UNREFERENCED_PARAMETER(point);
		return true;
	}


	bool CDXUICheckBoxMouseHandler::OnLeftMouseUp(DXPoint point)
	{
		if (m_pControl->m_pUIManager->GetElementAtPoint(point) == m_pControl)
		{
			m_pControl->SetState(DX_UIELEMENT_STATE_MOUSEOVER);
			m_pControl->m_bChecked = !m_pControl->m_bChecked;
		}
		else
		{
			m_pControl->SetState(DX_UIELEMENT_STATE_NORMAL);
		}

		return true;
	}



	///////////////////////// RADIO BUTTON MOUSE HANDLER ///////////////////////////
	
	bool CDXUIRadioButtonMouseHandler::OnMouseMove(DXPoint point)
	{
		if (m_pControl->m_pUIManager->GetElementAtPoint(point) == m_pControl)
		{
			m_pControl->SetState(DX_UIELEMENT_STATE_MOUSEOVER);
		}
		else
		{
			m_pControl->SetState(DX_UIELEMENT_STATE_NORMAL);
		}

		return true;
	}


	bool CDXUIRadioButtonMouseHandler::OnLeftMouseDown(DXPoint point)
	{
		UNREFERENCED_PARAMETER(point);
		return true;
	}


	bool CDXUIRadioButtonMouseHandler::OnLeftMouseUp(DXPoint point)
	{
		if (m_pControl->m_pUIManager->GetElementAtPoint(point) == m_pControl)
		{
			DXRadioButtonGroup* pGroup = m_pControl->m_pRadioButtonGroup;
			// Uncheck the previous active radio.
			DXRadioButtonMapIterator iter = pGroup->m_mapRadioButtons.find(pGroup->uiActiveId);
			if (iter != pGroup->m_mapRadioButtons.end())
			{
				iter->second->m_bChecked = false;
			}

			m_pControl->SetState(DX_UIELEMENT_STATE_MOUSEOVER);
			pGroup->uiActiveId = m_pControl->m_uiRadioId;
			m_pControl->m_bChecked = true;
		}
		else
		{
			m_pControl->SetState(DX_UIELEMENT_STATE_NORMAL);
		}
		return true;
	}



	/////////////////////// SCROLL BAR MOUSE HANDLER /////////////////////////////

	bool CDXUIScrollBarMouseHandler::OnMouseMove(DXPoint point)
	{
		if (m_bDrag)
		{
			float fDelta;
			float fStripLength;

			if (m_pControl->m_bVertical)
			{
				fDelta = point.y - m_ptMousePos.y;
				fStripLength = m_pControl->m_rectStrip.fBottom - m_pControl->m_rectStrip.fTop;
			}
			else
			{
				fDelta = point.x - m_ptMousePos.x;
				fStripLength = m_pControl->m_rectStrip.fRight - m_pControl->m_rectStrip.fLeft;
			}
			float fAbsDelta = fabs(fDelta);
			float fTolerance = fStripLength / (float)m_pControl->m_uiTotalItems;
			if (fAbsDelta >= fTolerance)
			{
				m_ptMousePos = point;
				m_pControl->ScrollBy((int)(fDelta / fTolerance));
			}
		}
		else if (m_bMouseDown && m_pControl->m_pUIManager->GetElementAtPoint(point) == m_pControl)
		{
			// Using DX_POINT_IN_RECT to avoid things becoming complicated. One should'nt
			// placed overlapped control in the first place.
			if (DX_POINT_IN_RECT(point, m_pControl->m_rectButtonForeward))
			{
				m_pControl->m_ScrollStatus = DX_UISCROLLBAR_SCROLL_FOREWARD;
			}
			else if (DX_POINT_IN_RECT(point, m_pControl->m_rectButtonBackward))
			{
				m_pControl->m_ScrollStatus = DX_UISCROLLBAR_SCROLL_BACKWARD;
			}
			else
			{
				m_pControl->m_ScrollStatus = DX_UISCROLLBAR_SCROLL_NONE;
			}
		}

		return true;
	}


	bool CDXUIScrollBarMouseHandler::OnLeftMouseDown(DXPoint point)
	{
		m_bMouseDown = true;

		if (DX_POINT_IN_RECT(point, m_pControl->m_rectButtonForeward))
		{
			m_pControl->ScrollBy(1);
			m_pControl->m_ScrollStatus = DX_UISCROLLBAR_SCROLL_FOREWARD;
		}
		else if (DX_POINT_IN_RECT(point, m_pControl->m_rectButtonBackward))
		{
			m_pControl->ScrollBy(-1);
			m_pControl->m_ScrollStatus = DX_UISCROLLBAR_SCROLL_BACKWARD;
		}
		else if (DX_POINT_IN_RECT(point, m_pControl->m_rectSlider))
		{
			m_bDrag = true;
			m_ptMousePos = point;
		}

		return true;
	}


	bool CDXUIScrollBarMouseHandler::OnLeftMouseUp(DXPoint point)
	{
		if (DX_POINT_IN_RECT(point, *(m_pControl->GetRect())))
		{
			m_pControl->SetState(DX_UIELEMENT_STATE_MOUSEOVER);
		}
		else
		{
			m_pControl->SetState(DX_UIELEMENT_STATE_NORMAL);
		}

		m_bMouseDown = false;
		m_bDrag = false;
		m_pControl->m_ScrollStatus = DX_UISCROLLBAR_SCROLL_NONE;

		return true;
	}

} // DXFramework namespace end