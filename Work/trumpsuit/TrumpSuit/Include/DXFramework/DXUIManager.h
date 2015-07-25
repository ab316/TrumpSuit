#pragma once

#include "DXUnknown.h"


namespace DXFramework
{ // DXFramework namespace begin

	// 6 vertices per quad. Meaning (DX_MAX_UI_SPRITES_VERTICES / 6) sprites can be
	// rendered in a single batch.
	#define DX_MAX_UI_SPRITES_VERTICES	60

	class	IDXUIElement;
	class	CDXUIButton;
	class	CDX3DDriver;
	class   CDXTexture;
	class   CDXShader;
	class   CDXSprite;
	struct	DXRect;
	struct  DXRadioButtonGroup;


#define DX_INVALID_UIELEMENT_ID ((UINT)-1)

	struct DXUIElement
	{
		UINT			uiID;
		IDXUIElement*	pElement;
	};


	enum DX_UI_MESSAGE
	{
		DX_UI_MESSAGE_LEFT_MOUSE_CLICK,
		DX_UI_MESSAGE_KEY_PRESSED
	};


	typedef std::vector<DXUIElement*>				DXUIElementVector;
	typedef std::map<UINT, DXRadioButtonGroup*>		DXUIRadioButtonGroupMap;

	typedef DXUIElementVector::iterator				DXUIElementVectorIterator;
	typedef DXUIRadioButtonGroupMap::iterator		DXUIRadioButtonGroupMapIterator;

	typedef	bool DXOnUIMessage(UINT uiControlID, DX_UI_MESSAGE msg);
	
	// DXUIManager managers UI controls, processing messages to be passed to them
	// updating and rendering them.
	class CDXUIManager : public IDXUnknown
	{
	private:
		DXUIElementVector			m_vecpUIElements;
		DXUIRadioButtonGroupMap		m_mapRadioButtonGroups;
		DXUIElementVectorIterator	m_iterpUIElementsBegin;
		DXUIElementVectorIterator	m_iterpUIElementsEnd;
		CDX3DDriver*				m_p3dDriver;
		CDXSprite*					m_pSprite;
		CDXTexture*					m_pTexUIControls;
		CDXShader*					m_pVSGUI;
		CDXShader*					m_pPSGUI;
		IDXUIElement*				m_pKeyboardFocussedElement;
		DXOnUIMessage*				m_pCallbackOnUIMessage;
		float						m_fBackBufferWidth;
		float						m_fBackBufferHeight;
		bool						m_bIsRendering;

	public:
		float	GetBackBufferWidth()		const	{ return m_fBackBufferWidth;	};
		float	GetBackBufferHeight()		const	{ return m_fBackBufferHeight;	};


		void SetKeyboardFocus(IDXUIElement* pElement)
		{
			m_pKeyboardFocussedElement = pElement;
		}

		// Function signature is:	bool (UINT uiControl, DX_UI_MESSAGE msg)
		void SetCallbackOnUIMessage(DXOnUIMessage* pProc)
		{
			m_pCallbackOnUIMessage = pProc;
		}

		bool CallUIMessageCallback(IDXUIElement* pElement, DX_UI_MESSAGE msg)
		{
			if (m_pCallbackOnUIMessage)
			{
				UINT id = GetElementID(pElement);
				if (id != DX_INVALID_UIELEMENT_ID)
				{
					return m_pCallbackOnUIMessage(id, msg);
				}
				else
				{
					return false;
				}
			}
			else
			{
				return false;
			}
		}

	private:
		void	UpdateVectorIterators()
		{
			m_iterpUIElementsBegin = m_vecpUIElements.begin();
			m_iterpUIElementsEnd = m_vecpUIElements.end();
		}


	public:
		HRESULT			CreateResources();
		void			DestroyResources();
		void			OnBackBufferResize(float fWidth, float fHeight);
		bool			HasElement(IDXUIElement* pElement);
		bool			HasElement(UINT id);
		IDXUIElement*	GetElement(UINT id);
		UINT			GetElementID(IDXUIElement* pElement);
		// All values are in pixels.
		bool			AddButton(UINT id, WCHAR* szCaption, float fX, float fY, float fWidth, float fHeight);
		bool			AddEditBox(UINT id, UINT maxLength, float fUpperLeftX, float fUpperLeftY, float fWidth, float fHeight);
		bool			AddStatic(UINT id, WCHAR* szText, float fX, float fY, float fFontSize=18.0f);
		bool			AddCheckBox(UINT id, float fX, float fY, float fSide=25.0f, bool bChecked=false);
		bool			AddRadioButton(UINT id, UINT group, float fCentreX, float fCentreY, float fRadius=10.0f);
		bool			AddScrollBar(UINT id, float fX, float fY, float fLength, UINT uiNumUnits=0, bool bVertical=true, float fThickness=20.0f);

	private:
		DXUIElement*	AddElement(UINT id);
		void			RemoveElement(DXUIElement* pElement);

	public:
		IDXUIElement*	GetElementAtPoint(DXPoint point, bool bActiveOnly=true, bool bVisibleOnly=true);
		void			Update(float fDelta);
		void			Render();
		// Sorts the UI elements according to their z order. Required for proper rendering.
		// Call this method when z order of an element is changed or after finished adding
		// elements. Z order matters only when one element overlaps another.
		void			SortElements();
		// Processes the window messages and calls the appropriate keyboard/mouse handler
		// of appropriate ACTIVE and VISIBLE element (if any).
		// Returns true is the window message handler should not process the message further.
		bool			ProcessMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	public:
		static bool		InCorrectZOrder(DXUIElement* pFirst, DXUIElement* pSecond);

	public:
		static CDXUIManager*	GetInstance()
		{
			static CDXUIManager manager;
			return &manager;
		}

	public:
		virtual UINT		GetTypeId()		const	{ return DX_UIMANAGER_TYPE_ID;		};
		virtual LPCWSTR		GetTypeName()	const	{ return DX_UIMANAGER_TYPE_NAME;	};

	private:
		CDXUIManager();

	public:
		~CDXUIManager();
	};

} // DXFramework namespace end