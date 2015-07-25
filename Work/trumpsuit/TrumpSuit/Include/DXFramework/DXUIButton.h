#pragma once

#include "DXUIElement.h"


namespace DXFramework
{ // DXFramework namespace begin

	class	CDXUIButtonMouseHandler;


	// Needs no introduction.
	class CDXUIButton : public IDXUIElement
	{
		friend class CDXUIButtonMouseHandler;

	private:
		XMFLOAT4			m_f4TextColor;
		XMFLOAT4			m_f4MouseOverTint;
		XMFLOAT4			m_f4MouseDownTint;
		XMFLOAT4			m_f4NormalTint;
		XMFLOAT4			m_f4DisabledTint;
		DXRect				m_rectButton;
		float				m_fFontSize;
		float				m_fWidth;
		float				m_fHeight;
		float				m_fCaptionX;
		float				m_fCaptionY;
		WCHAR				m_szCaption[DX_MAX_NAME];

	public:
		HRESULT				Create(WCHAR* szCaption, float fUpperLeftX, float fUpperLeftY, float fWidth, float fHeight);
		
	public:
		virtual void		Render();
		virtual void		RenderText();
		virtual	void		Update(float fElapsedTime);
		virtual void		OnDimensionsUpdate() { };

	public:
		void	SetTextColor(XMFLOAT4 color)			{ m_f4TextColor = color;		};

		void	SetCaption(WCHAR* szCaption)
		{
			wcscpy_s(m_szCaption, DX_MAX_NAME, szCaption);
		}

		void	SetNormalTint(XMFLOAT4 color)			{ m_f4NormalTint = color;		};
		void	SetMouseOverTint(XMFLOAT4 color)		{ m_f4MouseOverTint = color;	};
		void	SetMouseDownTint(XMFLOAT4 color)		{ m_f4MouseDownTint = color;	};
		void	SetDisabledTint(XMFLOAT4 color)			{ m_f4DisabledTint = color;		};


		LPCWSTR	GetCaption()	const	{ return m_szCaption;	};

	public:
		virtual DX_UIELEMENT_TYPE	GetElementType()	const	{ return DX_UIELEMENT_TYPE_BUTTON;	};

	public:
		CDXUIButton();
		~CDXUIButton();
	};

} // DXFramework namespace end