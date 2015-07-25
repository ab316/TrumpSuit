#pragma once

#include "DXUIElement.h"


namespace DXFramework
{ // DXFramework namespace begin

	class	CDXUIComboBoxMouseHandler;
	class	CDXUIScrollBar;

	// Needs no introduction.
	class CDXUIComboBox : public IDXUIElement
	{
		friend class CDXUIComboBoxMouseHandler;

	private:
		XMFLOAT4			m_f4TextColor;
		XMFLOAT4			m_f4MouseOverTint;
		XMFLOAT4			m_f4MouseDownTint;
		XMFLOAT4			m_f4NormalTint;
		XMFLOAT4			m_f4DisabledTint;
		DXRect				m_rectSelectionBox;
		DXRect				m_rectButton;
		CDXUIScrollBar*		m_pScrollBar;
		UINT				m_uiPageSize;
		float				m_fFontSize;
		float				m_fWidth;
		float				m_fHeight;

	public:
		HRESULT				Create(float fUpperLeftX, float fUpperLeftY, float Height, float fFontSize=18.0f);
		
	public:
		virtual void		Render();
		virtual void		RenderText();
		virtual	void		Update(float fElapsedTime);
		virtual void		OnDimensionsUpdate() { };

	public:
		void	SetTextColor(XMFLOAT4 color)			{ m_f4TextColor = color;		};
		void	SetNormalTint(XMFLOAT4 color)			{ m_f4NormalTint = color;		};
		void	SetMouseOverTint(XMFLOAT4 color)		{ m_f4MouseOverTint = color;	};
		void	SetMouseDownTint(XMFLOAT4 color)		{ m_f4MouseDownTint = color;	};
		void	SetDisabledTint(XMFLOAT4 color)			{ m_f4DisabledTint = color;		};


	public:
		virtual DX_UIELEMENT_TYPE	GetElementType()	const	{ return DX_UIELEMENT_TYPE_COMBOBOX;	};

	public:
		CDXUIComboBox();
		~CDXUIComboBox();
	};

} // DXFramework namespace end