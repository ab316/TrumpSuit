#pragma once


namespace DXFramework
{ // DXFramework namespace begin

	void CDX2DDriver::SetBrush(DX_2D_BRUSH_TYPE type)
	{
		switch (type)
		{
		case DX_2D_BRUSH_TYPE_SOLID:
			m_State.pCurrentBrush = m_pD2dSolidBrush;
		}
	}

	void CDX2DDriver::SetSolidColor(float fColor[4])
	{
		m_State.solidBrushColor = D2D1::ColorF(fColor[0], fColor[1], fColor[2], fColor[3]);
		m_pD2dSolidBrush->SetColor(m_State.solidBrushColor);
	}


	void CDX2DDriver::SetSolidColor(float r, float g, float b, float a)
	{
		m_State.solidBrushColor = D2D1::ColorF(r, g, b, a);
		m_pD2dSolidBrush->SetColor(m_State.solidBrushColor);
	}


	void CDX2DDriver::SetStrokeWidth(float fWidth)
	{
		m_State.fStrokeWidth = fWidth;
	}


	void CDX2DDriver::SetFont(DXFont* pFont)
	{
		m_State.pFont = pFont;
	}


	///<summary>
	//Should be called before rendering text. RenderText() calls b/w BeginTextRendering()
	//and EndTextRendering() calls place the texts line by line.
	//</summary>
	///<param name="fStartX">The x coord in pixels of upper left corner of the text.</param>
	///<param name="fStartY">The y coord in pixels of upper left corner of the text.</param>
	///<param name="fVerticalSpacing">The vertical text spacing multiplier. 1.0 gives normal spacing.</param>
	void CDX2DDriver::BeginTextRendering(float fStartX, float fStartY, float fVerticalSpacing)
	{
		m_State.fTextPosX = fStartX;
		m_State.fTextPosY = fStartY;
		m_State.fTextVerticalSpacing = fVerticalSpacing;
		m_State.bLineByLineText = true;
	}


	///<summary>
	//Should be called after finished rendering line by line text.
	//</summary>
	void CDX2DDriver::EndTextRendering()
	{
		m_State.bLineByLineText = false;
	}

	///<summary>Renders text on the 2D render target.</summary>
	///<param name="szText">The text to render</param>
	///<param name="fWidth">The width of the text region in pixels for word wrapping.</param>
	///<param name="fXPos">Used only when outside Begin/End-TextRendering() block. The x coord of upper left corner of text.</param>
	///<param name="fYPos">Used only when outside Begin/End-TextRendering() block. The y coord of upper left corner of text.</param>
	void CDX2DDriver::RenderText(LPCWSTR szText, float fXPos, float fYPos, float fWidth)
	{
		D2D1_RECT_F layoutRect;

		if (!m_State.bRendering)
		{
			DX_DEBUG_OUTPUT("DX2DDriver: Rendering operation called without calling begin rendering first.");
			return;
		}

		float fRightX;
		if (m_State.bLineByLineText)
		{
			fRightX = m_State.fTextPosX + fWidth;
			layoutRect = D2D1::RectF(m_State.fTextPosX, m_State.fTextPosY, fRightX, m_State.fHeight);
			m_State.fTextPosY += m_State.pFont->fSize * m_State.fTextVerticalSpacing;
		}
		else
		{
			fRightX = fXPos + fWidth;
			layoutRect = D2D1::RectF(fXPos, fYPos, fRightX, m_State.fHeight);
		}

		m_pD2dRenderTarget->DrawTextW(szText, wcslen(szText), m_State.pFont->pTextFormat, layoutRect, m_State.pCurrentBrush, D2D1_DRAW_TEXT_OPTIONS_CLIP);
	}


	///<summary>Drawing a fixed text is more efficient than arbitrary text using RenderText().</summary>
	void CDX2DDriver::RenderFixedText(DXFixedText* pText, float fXPos, float fYPos)
	{
		m_pD2dRenderTarget->DrawTextLayout(D2D1::Point2F(fXPos, fYPos), pText->pTextLayout, m_State.pCurrentBrush);
	}


	void CDX2DDriver::RenderRectangle(float fX, float fY, float fWidth, float fHeight, bool bFilled)
	{
		D2D1_RECT_F rect = D2D1::RectF(fX, fY, fX+fWidth, fY+fHeight);
		if (bFilled)
		{
			m_pD2dRenderTarget->FillRectangle(rect, m_State.pCurrentBrush);
		}
		else
		{
			m_pD2dRenderTarget->DrawRectangle(rect, m_State.pCurrentBrush, m_State.fStrokeWidth);
		}
	}


	void CDX2DDriver::RenderRoundedRectangle(float fX, float fY, float fWidth, float fHeight, float fXRadius, float fYRadius, bool bFilled)
	{
		D2D1_RECT_F rect = D2D1::RectF(fX, fY, fX+fWidth, fY+fHeight);
		D2D1_ROUNDED_RECT roundedRect = D2D1::RoundedRect(rect, fXRadius, fYRadius);
		if (bFilled)
		{
			m_pD2dRenderTarget->FillRoundedRectangle(roundedRect, m_State.pCurrentBrush);
		}
		else
		{
			m_pD2dRenderTarget->DrawRoundedRectangle(roundedRect, m_State.pCurrentBrush, m_State.fStrokeWidth);
		}
	}

} // DXFramework namespace end