#pragma once

#include "DXUIElement.h"

namespace DXFramework
{ // DXFramework namespace begin

	class CDXUIEditBoxKeyboardHandler;
	class CDXUIEditBoxMouseHandler;

	// A.k.a text box.
	class CDXUIEditBox : public IDXUIElement
	{
		friend class CDXUIEditBoxKeyboardHandler;
		friend class CDXUIEditBoxMouseHandler;

	private:
		XMFLOAT4			m_f4NormalTint;
		XMFLOAT4			m_f4DisabledTint;
		XMFLOAT4			m_f4TextColor;
		XMFLOAT4			m_f4CaretColor;
		// The text region of the box.
		DXRect				m_rectText;
		DXRect				m_rectCaret;
		DXRect				m_rectCaretTexture;
		// The text.
		WCHAR*				m_szText;
		// Data contained in this buffer is drawn. This buffer is also used for
		// temporary manipulation.
		WCHAR*				m_szTemp;
		// In pixels.
		float				m_fFontSize;
		// In pixels.
		float				m_fCaretWidth;
		float				m_fCaretBlinkTimer;
		// For rendering correct position of caret, we require the offset of right corner
		// of each character from the left corner of first character.
		float*				m_pfTextOffsets;
		// The current text length,
		UINT				m_uiTextLength;
		// The maximum characters we can hold.
		UINT				m_uiMaxLength;
		// The current position of the caret in the text.
		UINT				m_uiCaretPos;
		// The index of the first character in the text that can be seen in the box.
		UINT				m_uiFirstCharPosSeen;
		// The index of the last character in the text that can be seen in the box.
		UINT				m_uiLastCharPosSeen;
		// The maximum number of characters that can be seen in the box at once.
		UINT				m_uiMaxCharsCanBeSeen;
		float				m_fWidth;
		float				m_fHeight;
		bool				m_bBlinkCaret;
		bool				m_bShowCaret;
		bool				m_bCaretState;

	public:
		HRESULT		Create(UINT maxLength, float fUpperLeftX, float fUpperLeftY, float fWidth, float fHeight);
		
		// Inherited from IDXUIElement.
	public:
		virtual	void		OnDimensionsUpdate();
		virtual	void		Render();
		virtual	void		RenderText();
		virtual	void		Update(float fElapsedTime);

	private:
		bool		UpdateMaxCharsSeen();

	public:
		LPCWSTR		GetText() const			{ return m_szText;	};

	public:
		void	SetTextColor(XMFLOAT4 color)			{ m_f4TextColor = color;	};
		void	SetNormalTint(XMFLOAT4 color)			{ m_f4NormalTint = color;	};
		void	SetDisabled(XMFLOAT4 color)				{ m_f4DisabledTint = color;	};

		void	SetFontSize(float fSize)
		{
			m_fFontSize = fSize;
			OnDimensionsUpdate();
		}

		void	SetCaretWidth(float fWidthPixels)
		{
			m_fCaretWidth = fWidthPixels;
		}

		void	ShowCaret(bool bShowCaret)
		{
			m_bShowCaret = bShowCaret;
			m_bCaretState = bShowCaret;
		}

		void	BlinkCaret(bool bBlinkCaret)
		{
			m_bBlinkCaret = bBlinkCaret;
		}

	public:
		virtual DX_UIELEMENT_TYPE	GetElementType()	const	{ return DX_UIELEMENT_TYPE_BUTTON;	};
	
	public:
		CDXUIEditBox();
		~CDXUIEditBox();
	};

} // DXFramework namespace end