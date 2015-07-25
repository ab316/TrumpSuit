#pragma once

#include "DXUIElement.h"


namespace DXFramework
{ // DXFramework namespace begin

	class CDXUIRadioButtonMouseHandler;
	class CDXUIRadioButton;

	typedef std::map<UINT, CDXUIRadioButton*>	DXRadioButtonMap;
	typedef DXRadioButtonMap::iterator			DXRadioButtonMapIterator;

	// Only one radio button can be active in a radio button group.
	struct DXRadioButtonGroup
	{
		UINT				uiActiveId;
		DXRadioButtonMap	m_mapRadioButtons;
	};


	class CDXUIRadioButton : public IDXUIElement
	{
		friend class CDXUIRadioButtonMouseHandler;

	private:
		DXRect					m_rectTextureCheck;
		XMFLOAT4				m_f4CheckColor;
		XMFLOAT4				m_f4NormalTint;
		XMFLOAT4				m_f4MouseOverTint;
		XMFLOAT4				m_f4DisabledTint;
		// The radio button group to which this radio button belongs.
		DXRadioButtonGroup*		m_pRadioButtonGroup;
		// Each radio button in a group must have a unique radio id.
		UINT					m_uiRadioId;
		// Checked status of the button.
		bool					m_bChecked;
		// For animation.
		bool					m_bShowCheck;

	public:
		HRESULT		Create(DXRadioButtonGroup* pGroup, float fCentreX, float fCentreY, float fRadius);

	public:
		bool		GetChecked()			const		{ return m_bChecked;			};

	public:
		void		SetChecked()
		{
			m_pRadioButtonGroup->uiActiveId = m_uiRadioId;
			m_bChecked = true;
		}
		void		SetNormalTint(XMFLOAT4 color)		{ m_f4NormalTint = color;		};
		void		SetMouseOverTint(XMFLOAT4 color)	{ m_f4MouseOverTint = color;	};
		void		SetDisabledTint(XMFLOAT4 color)		{ m_f4DisabledTint = color;		};

	public:
		virtual	void				RenderText()			  { };
		virtual void				Render();
		virtual	void				Update(float fDelta);
		virtual void				OnDimensionsUpdate()	  { };	
		virtual DX_UIELEMENT_TYPE	GetElementType()	const { return DX_UIELEMENT_TYPE_RADIOBUTTON;	};

	public:
		CDXUIRadioButton();
		~CDXUIRadioButton();
	};

} // DXFramework namespace end