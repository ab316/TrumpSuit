#pragma once

#include "DXUnknown.h"

namespace DXFramework
{ // DXFramework namespace begin

#define DX_MAX_TEXT_VERTICES	500

	class  CDX3DDriver;
	class  CDXTexture;
	class  CDXShader;
	class  CDXHardwareBuffer;
	class  CDXSprite;
	class  CDXBitmapFontFile;

	struct DXBlendState;
	struct DXBitmapFontGlyphData;

	// Singleton class for text rendering. The font rendering system is in crude condition
	// for now.
	// Depends upon:
	//	DX3DDriver
	//	DXGraphicsDriver
	//	DXTexture
	//	DXSpite
	class CDXTextRenderer : public IDXUnknown
	{
	public:
		XMFLOAT4				m_f4FontColor;
		CDX3DDriver*			m_p3dDriver;
		CDXBitmapFontFile*		m_pBitmapFontGlyphData;
		DXBitmapFontGlyphData*	m_pGlyphData;
		CDXSprite*				m_pSprite;
		DXBlendState*			m_pBSTransparent;
		CDXTexture*				m_pTexDefaultFont;
		CDXTexture*				m_pTexCurrentFont;
		CDXShader*				m_pVS;
		CDXShader*				m_pPS;
		// Temporary storage of pipeline state.
		DXBlendState*			m_pBSTemp;
		CDXTexture*				m_pTexTemp;
		CDXShader*				m_pVSTemp;
		CDXShader*				m_pPSTemp;
		UINT					m_uiVBOffsetTemp;
		float					m_fTextureWidth;
		float					m_fTextureHeight;
		float					m_fReciprocalBackBufferWidth;
		float					m_fReciprocalBackBufferHeight;
		// Starting point of text in screen space.
		float					m_fStartX;
		// Starting point of text in screen space.
		float					m_fStartY;
		// The font size in pixels.
		float					m_fFontSize;
		// The glyph width in screen space.
		float					m_fGlyphWidth;
		// The glyph height in screen space.
		float					m_fGlyphHeight;
		// Whether the state of the pipeline was saved by BeginText() so that
		// it can be restored by EndText().
		bool					m_bStateSaved;


	public:
		CDXTexture*		GetDefaultFontTexture()	const	{ return m_pTexDefaultFont;	};
		CDXTexture*		GetCurrentFontTexture()	const	{ return m_pTexCurrentFont;	};

		// Public system tasks.
	public:
		// Startup routine.
		HRESULT			Startup(CDX3DDriver* p3DDriver, WCHAR* szDefaultFontTexture, WCHAR* szDefaultGlyphDataFile, float fDefaultSizeInPixel=32.0f);
		void			Shutdown();
		// Should be called after startup and every time the back buffer is resized.
		void			OnBackBufferResize(float fBackBufferWidth, float fBackBufferHeight);

		// Public user tasks.
	public:
		// Fills the float array with the horizontal offset of right corner of each character from 
		// the the left corner of the first character. The float array must be atleast uiInLength
		// elements long. The filled offsets are correct for the backbuffer size of the text renderer.
		void		GetTextMetrics(LPCWSTR szText, float fFontSize, UINT uiInLength, float* pfOffsetsInPixels);
		// Coords in projection space range from -1 to 1.
		void		RenderTextProjectionSpace(LPCWSTR szText, float fX, float fY, float fDepth=0);
		///<summary>
		// Updates the pipeline state (Textures, Blend state, shaders, constant buffer
		// primitive type, Input layout) required to render text.
		// NOTE: Vertex Buffer is set by RenderText() methods.
		// Calls to RenderText(LPCWSTR) method must be enclosed within BeginText() and EndText()
		// method calls. Other RenderText method calls can be called outside BeginText() block
		// But the required pipeline states (Texture, blend state, input layout, shaders)
		// must be set prior to the call.
		///</summary>
		///<param name="fStartX">X coord of upper left corner.</param>
		///<param name="fStartY">Y coord of upper left corner.</param>
		///<param name="bInPixels">If true, the coords are interpreted as in pixels (screen space), otherwise in normalized screen space.</param>
		///<param name="bSaveState">If true, the pipeline states which will be changed by this call will be stored.</param>
		void		BeginText(float fStartX, float fStartY, bool bInPixels, bool bSaveState=false);
		///<summary>If BeginText() was called previously with bSaveState = true, call to this function will restore the saved state.</summary>
		void		EndText();

	public:
		///<summary>Successive calls to this method in a BeginText() EndText() code block
		// renders text in successive lines.</summary>
		void	RenderText(LPCWSTR szText, float fDepth=0)
		{
			RenderTextProjectionSpace(szText, m_fStartX, m_fStartY, fDepth);
			m_fStartY -= m_fGlyphHeight;
		}

		///<summary>Coords are either in normalized screen space or in screen space
		// depending on the value of bInPixels.</summary>
		void	RenderText(LPCWSTR szText, float fX, float fY, float fDepth=0, bool bInPixels=false)
		{
			float x;
			float y;
			if (bInPixels)
			{
				fX *= m_fReciprocalBackBufferWidth;
				fY *= m_fReciprocalBackBufferHeight;
			}
			NormalizedScreenSpaceToProjectionSpace(fX, fY, &x, &y);
			RenderTextProjectionSpace(szText, x, y, fDepth);
		}

		// Generates the text vertices so that the text is centered at the origin. Update
		// the GUI constant buffer as needed to transform the text before calling this method.
		// Won't work with muliline strings.
		void	RenderTextCentered(LPCWSTR szText, float fDepth=0)
		{
			float x = (wcslen(szText) * (m_fGlyphWidth)) * -0.5f;
			float y = m_fGlyphHeight / 2.0f;
			RenderTextProjectionSpace(szText, x, y, fDepth);
		}

		// Sets the text's font size in pixels.
		void	SetFontSize(float fSizeInPixels)
		{
			m_fFontSize = fSizeInPixels;
			m_fGlyphWidth = 2 * m_fFontSize * m_fReciprocalBackBufferWidth;
			m_fGlyphHeight = 2* m_fFontSize * m_fReciprocalBackBufferHeight;
		}

		// Converts given coords in normalize screen space ( (0, 0) to (1, 1) ) to projection
		// or clip space ( (-1, 1) to (1, 1) ).
		void	NormalizedScreenSpaceToProjectionSpace(float fInX, float fInY, float* pOutProjectionX, float* pOutProjectionY)
		{
			*pOutProjectionX = (2.0f * fInX) - 1.0f;
			*pOutProjectionY = 1.0f - (2.0f * fInY);
		}

		void	SetFontColor(XMFLOAT4 color)			{ m_f4FontColor = color;	};
		void	SetVertexShader(CDXShader* pShader)		{ m_pVS = pShader;			};
		void	SetPixelShader(CDXShader* pShader)		{ m_pPS = pShader;			};

	public:
		static CDXTextRenderer*	GetInstance()
		{
			static CDXTextRenderer renderer;
			return &renderer;
		}

	public:
		virtual UINT		GetTypeId()		const		{ return DX_TEXT_RENDERER_TYPE_ID;	};
		virtual LPCWSTR		GetTypeName()	const		{ return DX_TEXTRENDERER_TYPE_NAME;	};

	private:
		CDXTextRenderer();

	public:
		~CDXTextRenderer();
	};

} // DXFramework namespace end