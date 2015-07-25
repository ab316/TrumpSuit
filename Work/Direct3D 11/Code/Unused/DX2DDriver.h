#pragma once

#include "DXUnknown.h"

namespace DXFramework
{ // DXFramework namespace begin

	class CDXTexture;


	enum DX_2D_BRUSH_TYPE
	{
		DX_2D_BRUSH_TYPE_SOLID,
		DX_2D_BRUSH_TYPE_BITMAP,
	};

	enum DX_FONT_WEIGHT
	{
		DX_FONT_WEIGHT_THIN				=	100,
		DX_FONT_WEIGHT_EXTRA_LIGHT		=	200,
		DX_FONT_WEIGHT_LIGHT			=	300,
		DX_FONT_WEIGHT_NORMAL			=	400,
		DX_FONT_WEIGHT_MEDIUM			=	500,
		DX_FONT_WEIGHT_SEMI_BOLD		=	600,
		DX_FONT_WEIGHT_BOLD				=	700,
		DX_FONT_WEIGHT_EXTRA_BOLD		=	800,
		DX_FONT_WEIGHT_BLACK			=	900,
		DX_FONT_WEIGHT_EXTRA_BLACK		=	950,
	};


	enum DX_FONT_STYLE
	{
		DX_FONT_STYLE_NORMAL	=	DWRITE_FONT_STYLE_NORMAL,
		DX_FONT_STYLE_ITALIC	=	DWRITE_FONT_STYLE_ITALIC,
	};


	enum DX_FONT_STRETCH
	{
		DX_FONT_STRETCH_EXTRA_CONDENSED		=	2,
		DX_FONT_STRETCH_CONDENSED,
		DX_FONT_STRETCH_SEMI_CONDENSED,
		DX_FONT_STRETCH_NORMAL,
		DX_FONT_STRETCH_SEMI_EXPANDED,
		DX_FONT_STRETCH_EXPANDED,
		DX_FONT_STRETCH_EXTRA_EXPANDED,
	};


	enum DX_TEXT_ALIGNMENT
	{
		DX_TEXT_ALIGNMENT_LEADING	=	0,
		DX_TEXT_ALIGNMENT_TRAILING,
		DX_TEXT_ALIGNMENT_CENTER,
	};


	enum DX_PARAGRAPH_ALIGNMENT
	{
		DX_PARAGRAPH_ALIGNMENT_NEAR	=	0,
		DX_PARAGRAPH_ALIGNMENT_FAR,
		DX_PARAGRAPH_ALIGNMENT_CENTER,
	};


	// Represent a font for drawing text on the screen. Once the font is
	// created, only its text and paragraph alignment can be changed via
	// respective methods. If other fields are changed, then CreateFont()
	// method of CDX2Driver must be called again to update. It is not
	// recommended, therefore, to change the fields frequently. Instead
	// multiple fonts should be created.
	struct DXFont
	{
		float					fSize;
		DX_FONT_WEIGHT			weight;
		DX_FONT_STYLE			style;
		DX_FONT_STRETCH			stretch;
		DX_TEXT_ALIGNMENT		textAlignment;
		DX_PARAGRAPH_ALIGNMENT	paragraphAlignment;
		WCHAR					szName[DX_MAX_NAME];
		// Internal usage.
		IDWriteTextFormat*		pTextFormat;


		DXFont(LPCWSTR szFontName=L"Ariel", float fSizeInPixels=18.0f, DX_FONT_WEIGHT fontWeight=DX_FONT_WEIGHT_SEMI_BOLD, DX_FONT_STYLE fontStyle=DX_FONT_STYLE_NORMAL, DX_FONT_STRETCH fontStretch=DX_FONT_STRETCH_NORMAL)
		{
			pTextFormat = 0;
			SetName(szFontName);
			fSize = fSizeInPixels;
			weight = fontWeight;
			style = fontStyle;
			stretch = fontStretch;
		}

		~DXFont()
		{
		}

		void SetName(LPCWSTR szNewName)
		{
			wcscpy_s(szName, DX_MAX_NAME, szNewName);
		}

		// Must be called after CreateFont() method succeeds.
		void SetTextAlignment(DX_TEXT_ALIGNMENT alignment)
		{
			pTextFormat->SetTextAlignment((DWRITE_TEXT_ALIGNMENT)alignment);
		}

		// Must be called after CreateFont() method succeeds.
		void SetParagraphAlignment(DX_PARAGRAPH_ALIGNMENT alignment)
		{
			pTextFormat->SetParagraphAlignment((DWRITE_PARAGRAPH_ALIGNMENT)alignment);
		}
	};


	// A fixed text cannot be changed once it is created (to change the text, it needs
	// to be created again) but it is more efficient to render. Use DXFixedText when
	// you need to render the same text multiple times (or for multiple frames) because
	// the RenderText() method which takes any string needs to build the text glyphs every
	// time it is called. Except the string, the other parameters can be changed without
	// recreation, using the SetXXX functions(); These must only be used after creation.
	struct DXFixedText
	{
		UINT				uiStringLength;
		float				fWidth;
		float				fHeight;
		// Internal usage.
		IDWriteTextLayout*	pTextLayout;
		WCHAR*				szText;


		DXFixedText() :	uiStringLength(0),
						pTextLayout(0),
						fWidth(1000),
						fHeight(1000),
						szText(0)
		{
		}

		~DXFixedText()
		{
			if (szText)
			{
				delete [] szText;
			}
		}

		// CreateFixedText() method of CDX2DDriver needs to be called to update the text.
		void SetString(LPCWSTR szNewString)
		{
			size_t uiNewLength = wcslen(szNewString);
			if (uiStringLength < uiNewLength)
			{
				if (szText)
				{
					DX_SAFE_DELETE(szText);
				}
				uiStringLength = uiNewLength;
				szText = new WCHAR[uiStringLength+1];
			}
			wcscpy_s(szText, uiStringLength+1, szNewString);
		}

		// Sets the new width of the layout box.
		void SetWidth(float fNewWidth)
		{
			fWidth = fNewWidth;
			if (pTextLayout)
			{
				pTextLayout->SetMaxWidth(fNewWidth);
			}
		}

		// Sets the new height of the layout box.
		void SetHeight(float fNewHeight)
		{
			fHeight = fNewHeight;
			if (pTextLayout)
			{
				pTextLayout->SetMaxWidth(fNewHeight);
			}
		}


		///<param name="startPos">The first character to which to apply the change.</param>
		///<param name="length">The number of characters after startPos to affect. Default value affects the entire text."</param>
		void SetFontSize(float fSize, UINT32 startPos=0, UINT32 length=-1)
		{
			DWRITE_TEXT_RANGE range = { startPos, (length == (UINT)-1) ? uiStringLength : length };
			pTextLayout->SetFontSize(fSize, range);
		}


		///<param name="startPos">The first character to which to apply the change.</param>
		///<param name="length">The number of characters after startPos to affect. Default value affects the entire text."</param>
		void SetFontStyle(DX_FONT_STYLE style, UINT32 startPos=0, UINT32 length=-1)
		{
			DWRITE_TEXT_RANGE range = { startPos, (length == (UINT)-1) ? uiStringLength : length };
			pTextLayout->SetFontStyle((DWRITE_FONT_STYLE)style, range);
		}

		///<param name="startPos">The first character to which to apply the change.</param>
		///<param name="length">The number of characters after startPos to affect. Default value affects the entire text."</param>
		void SetFontWeight(DX_FONT_WEIGHT weight, UINT32 startPos=0, UINT32 length=-1)
		{
			DWRITE_TEXT_RANGE range = { startPos, (length == (UINT)-1) ? uiStringLength : length };
			pTextLayout->SetFontWeight((DWRITE_FONT_WEIGHT)weight, range);
		}

		///<param name="startPos">The first character to which to apply the change.</param>
		///<param name="length">The number of characters after startPos to affect. Default value affects the entire text."</param>
		void SetFontStretch(DX_FONT_STRETCH stretch, UINT32 startPos=0, UINT32 length=-1)
		{
			DWRITE_TEXT_RANGE range = { startPos, (length == (UINT)-1) ? uiStringLength : length };
			pTextLayout->SetFontStretch((DWRITE_FONT_STRETCH)stretch, range);
		}

		///<param name="startPos">The first character to which to apply the change.</param>
		///<param name="length">The number of characters after startPos to affect. Default value affects the entire text."</param>
		void SetFontName(LPCWSTR szFont, UINT32 startPos=0, UINT32 length=-1)
		{
			DWRITE_TEXT_RANGE range = { startPos, (length == (UINT)-1) ? uiStringLength : length };
			pTextLayout->SetFontFamilyName(szFont, range);
		}
	};


	// Represent a bitmap for a bitmap brush.
	struct DXBitmap
	{
		WCHAR					szFileName[DX_MAX_FILE_PATH];
		// Internal usage.
		CDXTexture*				pTexture;
		// Internal usage.
		ID2D1Bitmap*			pBitmap;

		DXBitmap()	:	pTexture(0), pBitmap(0)
		{
		}
	};


	struct DX2DState
	{
		D2D1_COLOR_F		solidBrushColor;
		ID2D1Brush*			pCurrentBrush;
		DXFont*				pFont;
		float				fTextPosX;
		float				fTextPosY;
		float				fTextVerticalSpacing;
		float				fWidth;
		float				fHeight;
		float				fStrokeWidth;
		bool				bStarted;
		bool				bRendering;
		bool				bLineByLineText;
	};


	// Our 2D rendering system. It uses Direct2D to do this. As
	// D2D doesn't directly works with D3D11 (thanks to Microsoft). We
	// have to use a D3D10.1 device. A D3D11 texture is shared with the
	// D3D10.1 device. The D2D writes to the shared texture.
	// An application can only use the rendering related methods
	// of this class.
	class CDX2DDriver : public IDXUnknown
	{
		typedef std::vector<DXFont*>		DXFontVector;
		typedef std::vector<DXFixedText*>	DXFixedTextVector;
		typedef std::vector<DXBitmap*>		DXBitmapVector;
		typedef DXFontVector::iterator		DXFontVectorIterator;
		typedef DXFixedTextVector::iterator	DXFixedTextVectorIterator;
		typedef DXBitmapVector::iterator	DXBitmapVectorIterator;

	private:
		DXFontVector				m_vecpFonts;
		DXFixedTextVector			m_vecpFixedTexts;
		DXBitmapVector				m_vecpBitmaps;
		DX2DState					m_State;
		// The D3D11 device.
		ID3D11Device*				m_pD3d11Device;
		// The D310.1 device.
		ID3D10Device1*				m_pD3d10Device;
		// A keyed mutex allows exclusive access to a shared resource
		// that is used by multiple devices.
		IDXGIKeyedMutex*			m_pKeyedMutex11;
		IDXGIKeyedMutex*			m_pKeyedMutex10;
		// The D2D factory. For creating D2D resources.
		ID2D1Factory*				m_pD2dFactory;
		// The D2D render target on which it will render. The
		// buffer of this render target is our shared texture resource.
		ID2D1RenderTarget*			m_pD2dRenderTarget;
		// A solid color brush.
		ID2D1SolidColorBrush*		m_pD2dSolidBrush;
		// The shared texture. It is owned by the D3D11 device but
		// it will also be accessible to the D3D10.1 device. D2D
		// will render onto this texture through th D3D10.1 device.
		ID3D11Texture2D*			m_pSharedTexture;
		// Resource view for the shared texture. Resource views are used
		// to map resources to different stages of the rendering pipeline.
		ID3D11ShaderResourceView*	m_pSharedTextureView;
		// DirectWrite for text rendering. D2D is the tools.
		// DirectWrite is an artist that guides the "tools" to
		// paint text.
		IDWriteFactory*				m_pDWriteFactory;
		DXFont*						m_pFontDefault;

		// Public system tasks.
	public:
		// Creates the device independents resources.
		HRESULT			Startup(IDXGIAdapter1* pAdapter, ID3D11Device* pD3D11Device);
		// Create the device dependent resources which must be recreated when the window is resized.
		// The rendering state is lost by this call.
		HRESULT			CreateResources(UINT uiWidth, UINT uiHeight);
		// Create a font for for text rendering.
		HRESULT			CreateFont(DXFont* pFont);
		// A fixed text renders more efficiently than the arbitrary text rendered
		// by RenderText() function.
		HRESULT			CreateFixedText(DXFixedText* pFixedText, DXFont* pFont);
		HRESULT			CreateBitmap(DXBitmap* pBitmap);
		void			Shutdown();
		// Do not call. Internal usage only by the graphics driver.
		void			BeginRendering();
		// Do not call. Internal usage only by the graphics driver.
		void			EndRendering();
		void			ReleaseResources();
		// Does not changes the started, and rendering flags.
		void			ResetRenderingState();

		// Internal usage by the graphics driver.
	public:
		ID3D11Texture2D*	GetSharedTexture() const
		{
			return m_pSharedTexture;
		}

		ID3D11ShaderResourceView*	GetSharedTextureView() const
		{
			return m_pSharedTextureView;
		}

		// Public tasks.
	public:
		inline	DXFont*	GetDefaultFont() const { return m_pFontDefault;	};
		inline void		SetBrush(DX_2D_BRUSH_TYPE type);
		inline void		SetSolidColor(float fColor[4]);
		inline void		SetSolidColor(float r, float g, float b, float a=1.0f);
		inline void		SetStrokeWidth(float fWidth);
		inline void		SetFont(DXFont* pFont);
		inline void		BeginTextRendering(float fStartX, float fStartY, float fVerticalSpacing=1.0f);
		inline void		EndTextRendering();
		inline void		RenderText(LPCWSTR szText, float fXPos=0.0f, float fYPos=0.0f, float fWidth=1000.0f);
		inline void		RenderFixedText(DXFixedText* pText, float fXPos=0.0f, float fYPos=0.0f);
		inline void		RenderRectangle(float fX, float fY, float fWidth, float fHeight, bool bFilled=false);
		inline void		RenderRoundedRectangle(float fX, float fY, float fWidth, float fHeight, float fXRadius, float fYRadius, bool bFilled=false);


	public:
		static CDX2DDriver* GetInstance()
		{
			static CDX2DDriver driver;
			return &driver;
		}

	public:
		virtual UINT		GetTypeId()		const	{ return DX_2DDRIVER_TYPE_ID;	};
		virtual LPCWSTR		GetTypeName()	const	{ return DX_2DDRIVER_TYPE_NAME;	};

	private:
		CDX2DDriver();

	public:
		~CDX2DDriver();
	};

} // DXFramework namespace end


#include "DX2DDriverInline.h"