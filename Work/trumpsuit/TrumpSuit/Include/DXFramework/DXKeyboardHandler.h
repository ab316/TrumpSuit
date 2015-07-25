#pragma once

namespace DXFramework
{ // DXFramework namespace begin

	class CDXUIEditBox;

	class IDXKeyboardHandler
	{
	public:
		bool	m_bSendWCHARMessages;

	public:
		// For comparison of virtual key against alphabets, Always compare
		// with capital case letters. Small case alphabets will not work.
		virtual	bool	OnKeyDown(UINT virtualKeyCode)	=	0;
		virtual bool	OnKeyUp(UINT virtualKeyCode)	=	0;
		// The WCHAR functions can be used if only printable characters are required.
		virtual bool	OnKey(WCHAR szKey)
		{
			UNREFERENCED_PARAMETER(szKey);
			return false;
		};

		IDXKeyboardHandler()	:	m_bSendWCHARMessages(false)
		{
		}
	};


	class CDXUIEditBoxKeyboardHandler : public IDXKeyboardHandler
	{
	private:
		CDXUIEditBox*	m_pControl;
		// Maximum characters the edit box can hold.
		UINT*			puiMaxLength;
		// The current number of characters in the edit box.
		UINT*			puiTextLength;
		// The current absolute caret position in the text.
		UINT*			puiCaretPos;
		// The index of the first character in the text that is being displayed
		// in the edit box.
		UINT*			puiFirstPos;
		// The index of the last character in the text that is being displayed
		// in the edit box.
		UINT*			puiLastPos;
		// The maximum no. of characters the edit box can display.
		UINT*			puiMaxCharsAtOnce;

	public:
		virtual	bool	OnKeyDown(UINT virtualKeyCode);
		virtual bool	OnKeyUp(UINT virtualKeyCode);
		virtual bool	OnKey(WCHAR szKey);


	public:
		void	UpdateText();
		// Moves the caret position foreward by one (if possible) and performs appropriate actions
		// so that correct part of text is displayed in the editbox.
		void	StepCaretForeward(bool bDisplaceStart=true)
		{
			// If the caret is not at the end of the text. Then we can move foreward.
			if (*puiCaretPos < *puiTextLength)
			{
				// Increase the caret position.
				++(*puiCaretPos);
				// If the number of characters being displayed are more than we can display.
				// The = sign is necessary because all the character b/w first and last INCLUSIVE
				// are visible. So the number of characters visible at any time are:
				// (last - first) + 1. Instead of adding 1 on the LHS, we add = sign in the
				// inequality.
				if ((*puiLastPos - *puiFirstPos) >= *puiMaxCharsAtOnce)
				{
					// Displace the first or last visible character position as demanded
					// by the client to bring the no. of characters visible in the limit.
					// We can go with just moving the pointer by one because the variables
					// are updated with each key press.
					if (bDisplaceStart)
					{
						++(*puiFirstPos);
					}
					else
					{
						--(*puiLastPos);
					}
				}
				// If the caret goes beyond the last char position visible, we need
				// to move the visible region ahead by 1.
				if ((*puiCaretPos > *puiLastPos+1))
				{
					++(*puiFirstPos);
					++(*puiLastPos);
				}

				UpdateText();
			}
		}


		// Moves the caret position back by 1 if possible.
		void	StepCaretBackward()
		{
			// If the caret is ahead of the start zero position.
			if (*puiCaretPos > 0)
			{
				// Move the caret back by one.
				--(*puiCaretPos);
				// The last char position is not changed here because it is not necessary
				// that it be changed with each call to this method. If it needs to be changed
				// it should be done before call to this method.
				// Last char position can't be less than or equal (if the caret is not at 0) to
				// the first char pos.
				if (*puiLastPos <= *puiFirstPos)
				{
					// If it is then we decrement it as apropriate.
					*puiFirstPos -= (*puiFirstPos >= *puiMaxCharsAtOnce) ? *puiMaxCharsAtOnce : *puiFirstPos;
				}
				// If the caret goes 
				if (*puiCaretPos < *puiFirstPos)
				{
					*puiFirstPos -= (*puiFirstPos >= *puiMaxCharsAtOnce) ? *puiMaxCharsAtOnce : *puiFirstPos;
				}
				UINT uiDiff1 = *puiTextLength - *puiFirstPos;
				*puiLastPos = *puiFirstPos + min(*puiMaxCharsAtOnce-1, uiDiff1);

				UpdateText();
			}
		}

	public:
		CDXUIEditBoxKeyboardHandler(CDXUIEditBox* pControl);
		~CDXUIEditBoxKeyboardHandler();
	};

} // DXFramework namespace end