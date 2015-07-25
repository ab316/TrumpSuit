#pragma once

#include "DXUnknown.h"


namespace DXFramework
{ // DXFramework namespace begin

#define DX_UIELEMENT_DEFAULT_ZORDER		10

	enum DX_UIELEMENT_TYPE
	{
		DX_UIELEMENT_TYPE_BUTTON,
		DX_UIELEMENT_TYPE_EDITBOX,
		DX_UIELEMENT_TYPE_STATIC,
		DX_UIELEMENT_TYPE_CHECKBOX,
		DX_UIELEMENT_TYPE_RADIOBUTTON,
		DX_UIELEMENT_TYPE_SCROLLBAR,
		DX_UIELEMENT_TYPE_COMBOBOX,
	};


	enum DX_UIELEMENT_STATE
	{
		DX_UIELEMENT_STATE_NORMAL,
		// This should not be set directly. Use SetActive() instead.
		DX_UIELEMENT_STATE_DISABLED,
		DX_UIELEMENT_STATE_MOUSEOVER,
		DX_UIELEMENT_STATE_PRESSED,
	};


	class IDXMouseHandler;
	class IDXKeyboardHandler;
	class CDXUIManager;
	class CDXSprite;
 
	// A UI element base class. All user interface, HUD objects derive from this class.
	// Abstract class. Can't be instantiated. Collection of UIElements is managed by
	// CDXUIManager class.
	DX_ALIGN
	class IDXUIElement : public IDXUnknown
	{
		friend class CDXUIManager;

	protected:
		XMFLOAT4			m_f4Color;
		DXRect				m_rect;
		DXRect				m_rectTexture;
		CDXUIManager*		m_pUIManager;
		CDXSprite*			m_pSprite;
		IDXMouseHandler*	m_pMouseHandler;
		IDXKeyboardHandler*	m_pKeyboardHandler;
		DX_UIELEMENT_STATE	m_State;
		// Only active elements receive messages.
		bool				m_bActive;
		// Only visible elements receive messages.
		bool				m_bVisible;
		bool				m_bMouseFocusable;
		bool				m_bKeyboardFocusable;
		// Should be set to false in elements which don't want to render any text. It
		// saves the 3d driver some cpu cycles.
		bool				m_bRenderText;
		// Whether the element has input focus.
		bool				m_bFocussed;
		// 0 is the foremost layer, 255 is the backmost.
		char				m_cZOrder;
		// This class is 16 byte aligned. Its size must a multiple of 16 otherwise it will
		// generate compiler warning.
#if !defined(DX_X64)
		char				padding[8];
#endif

	public:
		virtual void				Render()									=	0;
		virtual	void				RenderText()					{ };
		// Should include functionality to update stuff required for rendering.
		virtual	void				Update(float fDelta)						=	0;
		virtual DX_UIELEMENT_TYPE	GetElementType()		const				=	0;
		// Should include functionality that updates any data that depends on the rect
		// of the element.
		virtual void				OnDimensionsUpdate()						=	0;

	public:
		IDXMouseHandler*	GetMouseHandler()		const	{ return m_pMouseHandler;		};
		IDXKeyboardHandler*	GetKeyboardHandler()	const	{ return m_pKeyboardHandler;	};
		char				GetZOrder()				const	{ return m_cZOrder;				};
		DX_UIELEMENT_STATE	GetState()				const	{ return m_State;				};
		const DXRect*		GetRect()				const	{ return &m_rect;				};
		bool				IsVisible()				const	{ return m_bVisible;			};
		bool				IsMouseFocusable()		const	{ return m_bMouseFocusable;		};
		bool				IsKeyboardFocusable()	const	{ return m_bKeyboardFocusable;	};
		bool				IsFocussed()			const	{ return m_bFocussed;			};
		bool				IsActive()				const	{ return m_bActive;				};
		
		void	SetVisible(bool bVisble)					{ m_bVisible = bVisble;		};
		// This should not be used to position the element after its creation.
		void	SetRect(DXRect newRect)						{ m_rect = newRect;			};
		void	SetTextureRect(DXRect rect)					{ m_rectTexture = rect;		};
		void	SetMouseFocusable(bool b)					{ m_bMouseFocusable = b;	};
		void	SetKeyboardFocusable(bool b)				{ m_bKeyboardFocusable = b;	};
		void	SetFocussed(bool b)							{ m_bFocussed = b;			};
		// Don't use 0 z order. Keep above 10. An offset to DX_UIELEMENT_DEFAULT_ZORDER
		// is recommended instead of using absolute values.
		void	SetZOrder(char c)							{ m_cZOrder = c;			};
		void	SetColor(XMFLOAT4 color)					{ m_f4Color = color;		};
		// Only active element's state can be changed.
		void	SetState(DX_UIELEMENT_STATE state)
		{ 
			if (m_bActive)
			{
				m_State = state;
			}
		}

		void	SetActive(bool b)
		{
			if (b)
			{
				m_bActive = b;
				SetState(DX_UIELEMENT_STATE_NORMAL);
			}
			else
			{
				SetState(DX_UIELEMENT_STATE_DISABLED);
				m_bActive = b;
			}
		};

		// Give the coords of upper left corner.
		void Move(float fNewX, float fNewY)
		{
			float dx = fNewX - m_rect.fLeft;
			float dy = fNewY - m_rect.fTop;
			m_rect.fLeft += dx;
			m_rect.fTop += dy;
			m_rect.fRight += dx;
			m_rect.fBottom += dy;
			OnDimensionsUpdate();
		}

	public:
		virtual UINT		GetTypeId()		const	{ return DX_UIELEMENT_TYPE_ID;		};
		virtual LPCWSTR		GetTypeName()	const	{ return DX_UIELEMENT_TYPE_NAME;	};

	public:
		IDXUIElement();
		virtual ~IDXUIElement();
	};

} // DXFramework namespace end