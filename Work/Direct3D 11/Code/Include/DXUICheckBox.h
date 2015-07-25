#pragma once

#include "DXUIElement.h"


namespace DXFramework
{ // DXFramework namespace begin

	class CDXUICheckBoxMouseHandler;

	class CDXUICheckBox : public IDXUIElement
	{
		friend class CDXUICheckBoxMouseHandler;

	private:
		DXRect		m_rectTextureCheck;
		XMFLOAT4	m_f4CheckColor;
		XMFLOAT4	m_f4NormalTint;
		XMFLOAT4	m_f4MouseOverTint;
		XMFLOAT4	m_f4DisabledTint;
		bool		m_bChecked;
		bool		m_bShowCheck;

	public:
		HRESULT		Create(float fX, float fY, float fSide, bool bChecked);

	public:
		bool		GetChecked()			const		{ return m_bChecked;			};

	public:
		void		SetChecked(bool b)					{ m_bChecked = b;				};
		void		SetNormalTint(XMFLOAT4 color)		{ m_f4NormalTint = color;		};
		void		SetMouseOverTint(XMFLOAT4 color)	{ m_f4MouseOverTint = color;	};
		void		SetDisabledTint(XMFLOAT4 color)		{ m_f4DisabledTint = color;		};

	public:
		virtual	void				RenderText()			  { };
		virtual void				Render();
		virtual	void				Update(float fDelta);
		virtual void				OnDimensionsUpdate()	  { };	
		virtual DX_UIELEMENT_TYPE	GetElementType()	const { return DX_UIELEMENT_TYPE_CHECKBOX;	};

	public:
		CDXUICheckBox();
		~CDXUICheckBox();
	};

} // DXFramework namespace end