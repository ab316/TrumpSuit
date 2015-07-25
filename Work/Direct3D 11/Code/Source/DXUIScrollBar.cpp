#include "DXStdafx.h"
#include "DXSprite.h"
#include "DXMouseHandler.h"
#include "DXUIScrollBar.h"


namespace DXFramework
{ // DXFramework namespace begin

	CDXUIScrollBar::CDXUIScrollBar()	:	m_uiPosition(0),
											m_uiTotalItems(0),
											m_uiPageSize(1),
											m_ScrollStatus(DX_UISCROLLBAR_SCROLL_NONE),
											m_bShowSlider(true),
											m_f4ActiveTint(1.0f, 1.0f, 1.0f, 1.0f),
											m_f4DisabledTint(0.5f, 0.5f, 0.5f, 1.0f)
	{
		m_bRenderText = false;
		m_bMouseFocusable = true;
		m_pMouseHandler = new CDXUIScrollBarMouseHandler(this);
	}


	CDXUIScrollBar::~CDXUIScrollBar()
	{
		DX_SAFE_DELETE(m_pMouseHandler);
	}


	HRESULT CDXUIScrollBar::Create(float fX, float fY, float fLength, float fThickness, UINT uiNumItems, bool bVertical)
	{
		HRESULT hr = S_OK;

		m_uiTotalItems = uiNumItems;
		m_uiPageSize = m_uiTotalItems / 2;

		m_bVertical = bVertical;

		if (bVertical)
		{
			m_rect.fLeft = fX;
			m_rect.fTop = fY;
			m_rect.fRight = fX + fThickness;
			m_rect.fBottom = fY + fLength;
		}
		else
		{	m_rect.fLeft = fX;
			m_rect.fTop = fY;
			m_rect.fRight = fX + fLength;
			m_rect.fBottom = fY + fThickness;
		}

		OnDimensionsUpdate();

		return hr;
	}

	void Swap(float* p1, float* p2)
	{
		float temp = *p1;
		*p1 = *p2;
		*p2 = temp;
	}

	void CDXUIScrollBar::OnDimensionsUpdate()
	{
		m_rectButtonBackward.fLeft = m_rect.fLeft;
		m_rectButtonBackward.fTop = m_rect.fTop;

		m_rectButtonForeward.fRight = m_rect.fRight;
		m_rectButtonForeward.fBottom = m_rect.fBottom;


		if (m_bVertical)
		{
			float fButtonsLength = m_rect.fRight - m_rect.fLeft;

			m_rectButtonBackward.fRight = m_rect.fRight;
			m_rectButtonBackward.fBottom = m_rect.fTop + fButtonsLength;

			m_rectButtonForeward.fLeft = m_rect.fLeft;
			m_rectButtonForeward.fTop = m_rect.fBottom - fButtonsLength;

			m_rectStrip.fLeft = m_rect.fLeft;
			m_rectStrip.fTop = m_rectButtonBackward.fBottom;
			m_rectStrip.fRight = m_rect.fRight;
			m_rectStrip.fBottom = m_rectButtonForeward.fTop;

			m_rectSlider.fLeft = m_rect.fLeft;
			m_rectSlider.fRight = m_rect.fRight;

			m_rectTexture = DXRect(0.024f, 0.484f, 0.936f, 0.625f);
			m_rectBackwardButtonTexture = DXRect(0.768f, 0.744f, 0.850f, 0.826f);
			m_rectForewardButtonTexture = DXRect(0.768f, 0.873f, 0.850f, 0.951f);
			m_rectSliderTexture = DXRect(0.855f, 0.744f, 0.932f, 0.920f);
		}
		else
		{
			float fButtonsLength = m_rect.fBottom - m_rect.fTop;

			m_rectButtonBackward.fRight = m_rect.fLeft + fButtonsLength;
			m_rectButtonBackward.fBottom = m_rect.fBottom;

			m_rectButtonForeward.fLeft = m_rect.fRight - fButtonsLength;
			m_rectButtonForeward.fTop = m_rect.fBottom - fButtonsLength;

			m_rectStrip.fLeft = m_rectButtonBackward.fRight;
			m_rectStrip.fTop = m_rect.fTop;
			m_rectStrip.fRight = m_rectButtonForeward.fLeft;
			m_rectStrip.fBottom = m_rect.fBottom;

			m_rectSlider.fTop = m_rect.fTop;
			m_rectSlider.fBottom = m_rect.fBottom;

			m_rectTexture = DXRect(0.054f, 0.482f, 0.936f, 0.627f);
			m_rectBackwardButtonTexture = DXRect(0.768f, 0.744f, 0.850f, 0.826f);
			m_rectForewardButtonTexture = DXRect(0.768f, 0.873f, 0.850f, 0.951f);
			m_rectSliderTexture = DXRect(0.857f, 0.744f, 0.932f, 0.920f);
		}

		UpdateSliderRect();
	}


	void CDXUIScrollBar::Render()
	{
		switch (m_State)
		{
		case DX_UIELEMENT_STATE_DISABLED:
			m_f4Color = m_f4DisabledTint;
			break;

		default:
			m_f4Color = m_f4ActiveTint;
		}

		m_pSprite->BatchRenderSprite(&m_rectStrip, &m_rectTexture, &m_f4Color, 0);
		m_pSprite->BatchRenderSprite(&m_rectButtonBackward, &m_rectBackwardButtonTexture, &m_f4Color, 0);
		m_pSprite->BatchRenderSprite(&m_rectButtonForeward, &m_rectForewardButtonTexture, &m_f4Color, 0);
		if (m_bShowSlider)
		{
			m_pSprite->BatchRenderSprite(&m_rectSlider, &m_rectSliderTexture, &m_f4Color, 0);
		}
	}


	void CDXUIScrollBar::Update(float fDelta)
	{
		static float fTime = 0.0f;

		switch (m_ScrollStatus)
		{
		case DX_UISCROLLBAR_SCROLL_FOREWARD:
			fTime += fDelta;
			if (fTime >= DX_UISCROLLBAR_SCROLL_DELAY)
			{
				ScrollBy(1);
				m_ScrollStatus = DX_UISCROLLBAR_SCROLL_REPEAT_FOREWARD;
				fTime = 0;
			}
			break;

		case DX_UISCROLLBAR_SCROLL_BACKWARD:
			fTime += fDelta;
			if (fTime >= DX_UISCROLLBAR_SCROLL_DELAY)
			{
				ScrollBy(-1);
				m_ScrollStatus = DX_UISCROLLBAR_SCROLL_REPEAT_BACKWARD;
				fTime = 0;
			}
			break;

		case DX_UISCROLLBAR_SCROLL_REPEAT_FOREWARD:
			fTime += fDelta;
			if (fTime >= DX_UISCROLLBAR_SCROLL_REPEAT_DELAY)
			{
				ScrollBy(1);
				fTime = 0;
			}
			break;

		case DX_UISCROLLBAR_SCROLL_REPEAT_BACKWARD:
			fTime += fDelta;
			if (fTime >= DX_UISCROLLBAR_SCROLL_REPEAT_DELAY)
			{
				ScrollBy(-1);
				fTime = 0;
			}
			break;

		default:
			fTime = 0;
		}
	}

} // DXFramework namespace end