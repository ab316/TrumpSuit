#pragma once

#include "DXUIElement.h"


namespace DXFramework
{ // DXFramework namespace begin

	class CDXUIStatic : public IDXUIElement
	{
	private:
		UINT						m_uiTextLength;
		UINT						m_uiMaxLength;
		float						m_fFontSize;
		WCHAR*						m_szText;

	public:
		HRESULT						Create(LPCWSTR szText, float fX, float fY, float fFontSize);
		void						SetText(LPCWSTR szText);

	public:
		void						SetFontSize(float fSize)		{ m_fFontSize = fSize;	};

	public:
		virtual	void				RenderText();
		virtual void				Render()				  { };
		virtual	void				Update(float fDelta)	  { UNREFERENCED_PARAMETER(fDelta);		};
		virtual void				OnDimensionsUpdate()	  { };	
		virtual DX_UIELEMENT_TYPE	GetElementType()	const { return DX_UIELEMENT_TYPE_STATIC;	};

	public:
		virtual UINT		GetTypeId()		const	{ return DX_UIELEMENT_TYPE_ID;		};
		virtual LPCWSTR		GetTypeName()	const	{ return DX_UIELEMENT_TYPE_NAME;	};

	public:
		CDXUIStatic();
		~CDXUIStatic();
	};

} // DXFramework namespace end