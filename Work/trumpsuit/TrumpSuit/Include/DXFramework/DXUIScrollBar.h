#pragma once

#include "DXUIElement.h"


namespace DXFramework
{ // DXFramework namespace begin

#define DX_UISCROLLBAR_SCROLL_DELAY 0.3f
#define DX_UISCROLLBAR_SCROLL_REPEAT_DELAY 0.05f

	class CDXUIScrollBarMouseHandler;

	enum DX_UISCROLLBAR_SCROLL_STATUS
	{
		DX_UISCROLLBAR_SCROLL_NONE,
		DX_UISCROLLBAR_SCROLL_BACKWARD,
		DX_UISCROLLBAR_SCROLL_FOREWARD,
		DX_UISCROLLBAR_SCROLL_REPEAT_BACKWARD,
		DX_UISCROLLBAR_SCROLL_REPEAT_FOREWARD,
	};

	class CDXUIScrollBar : public IDXUIElement
	{
		friend class CDXUIScrollBarMouseHandler;

	private:
		XMFLOAT4						m_f4ActiveTint;
		XMFLOAT4						m_f4DisabledTint;
		DXRect							m_rectButtonBackward;
		DXRect							m_rectButtonForeward;
		DXRect							m_rectSlider;
		DXRect							m_rectStrip;
		DXRect							m_rectBackwardButtonTexture;
		DXRect							m_rectForewardButtonTexture;
		DXRect							m_rectSliderTexture;
		DX_UISCROLLBAR_SCROLL_STATUS	m_ScrollStatus;
		// The first displayed item.
		UINT							m_uiPosition;
		// The total number of items.
		UINT							m_uiTotalItems;
		// No. of items that can be displayed at once.
		UINT							m_uiPageSize;
		bool							m_bVertical;
		bool							m_bShowSlider;

	public:
		HRESULT						Create(float fX, float fY, float fLength, float fThickness, UINT uiNumItems, bool bVertical);

	public:
		virtual	void				RenderText()			  { };
		virtual void				Render();
		virtual	void				Update(float fDelta);
		virtual void				OnDimensionsUpdate();	
		virtual DX_UIELEMENT_TYPE	GetElementType()	const { return DX_UIELEMENT_TYPE_SCROLLBAR;	};

	public:
		UINT		GetPageSize()				const	{ return m_uiPageSize;			};
		UINT		GetTotalItems()				const	{ return m_uiTotalItems;		};
		UINT		GetFirstItemDisplayed()		const	{ return m_uiPosition;			};
		
		void SetPageSize(UINT uiSize)
		{
			m_uiPageSize = uiSize;
			UpdateSliderRect();
		}


		void SetTotalItems(UINT uiItems)
		{
			m_uiTotalItems = uiItems;
			UpdateSliderRect();
		}

	public:
		// Positive values move the slider down (or right). Negative in the opposite
		// direction.
		void	ScrollBy(int i)
		{
			int iNewPosition = (int)m_uiPosition + i;
			if (iNewPosition >= 0 && iNewPosition + m_uiPageSize < m_uiTotalItems)
			{
				m_uiPosition = (UINT)iNewPosition;
				UpdateSliderRect();
			}
		}

		// Specify the absolute value of the scroll bar.
		void ScrollTo(UINT ui)
		{
			if (ui >= 0 && ui < m_uiTotalItems)
			{
				if (ui + m_uiPageSize < m_uiTotalItems)
				{
					m_uiPosition = ui;
				}
				else
				{
					m_uiPosition = m_uiTotalItems - m_uiPageSize;
				}
				UpdateSliderRect();
			}
		}

		void	UpdateSliderRect()
		{
			if (m_uiPageSize >= m_uiTotalItems)
			{
				m_bShowSlider = false;
				SetActive(false);
			}
			else
			{
				float fStripLength;
				float fSliderLength = (float)m_uiPageSize / (float)m_uiTotalItems;
				if (m_bVertical)
				{
					fStripLength = m_rectStrip.fBottom - m_rectStrip.fTop;
					fSliderLength *= fStripLength;
					m_rectSlider.fTop = m_rectStrip.fTop + (float)m_uiPosition * (fStripLength - fSliderLength) / (float)(m_uiTotalItems - m_uiPageSize - 1);
					m_rectSlider.fBottom = m_rectSlider.fTop + fSliderLength;
				}
				else
				{
					fStripLength = m_rectStrip.fRight - m_rectStrip.fLeft;
					fSliderLength *= fStripLength;
					m_rectSlider.fLeft = m_rectStrip.fLeft + (float)m_uiPosition * (fStripLength - fSliderLength) / (float)(m_uiTotalItems - m_uiPageSize - 1);
					m_rectSlider.fRight = m_rectSlider.fLeft + fSliderLength;
				}

				SetActive(true);
			}
		}

	public:
		CDXUIScrollBar();
		~CDXUIScrollBar();
	};

} // DXFramework namespace end