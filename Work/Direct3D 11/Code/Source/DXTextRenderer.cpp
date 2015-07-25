#include "DXStdafx.h"
#include "DXTexture.h"
#include "DX3DDriver.h"
#include "DXGraphicsDriver.h"
#include "DXBitmapFontFile.h"
#include "DXSprite.h"
#include "DXTextRenderer.h"

namespace DXFramework
{ // DXFramework namespace begin

	CDXTextRenderer::CDXTextRenderer()	:	m_p3dDriver(nullptr),
											m_pBitmapFontGlyphData(nullptr),
											m_pSprite(nullptr),
											m_pTexCurrentFont(nullptr),
											m_pTexDefaultFont(nullptr),
											m_pVS(nullptr),
											m_pPS(nullptr),
											m_pBSTransparent(nullptr),
											m_fStartX(0.0f),
											m_fStartY(0.0f),
											m_fFontSize(0.0f),
											m_fGlyphWidth(0.0f),
											m_fGlyphHeight(0.0f),
											m_fReciprocalBackBufferWidth(1.0f),
											m_fReciprocalBackBufferHeight(1.0f),
											m_fTextureWidth(512.0f),
											m_fTextureHeight(256.0f),
											m_bStateSaved(false)
	{
		//m_fTexErrorX = 1.0f / 1280.0f;
		//m_fTexErrorY = 1.0f / 1280.0f;
		m_f4FontColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		m_pSprite = new CDXSprite();
	}


	CDXTextRenderer::~CDXTextRenderer()
	{
		DX_SAFE_DELETE(m_pSprite);
	}


	HRESULT CDXTextRenderer::Startup(CDX3DDriver* p3DDriver, WCHAR* szDefaultFontTexture, WCHAR* szDefaultGlyphDataFile, float fDefaultSizeInPixel)
	{
		HRESULT hr = S_OK;

#define DX_ERROR_DEFAULTFONTTEXTURE_FAIL	L"ERROR!!! Unable to create default font texture."
#define DX_ERROR_DEFAULTFONTGLYPHFILE_FAIL	L"ERROR!!! Unable to create default font glyph data."
#define DX_ERROR_DEFAULTFONTVB_FAIL			L"ERROR!!! Unable to create default text vertex buffer."
#define DX_ERROR_SPRITE_FAIL				L"ERROR!!! Unable to create sprite."


		m_p3dDriver = p3DDriver;
		m_fFontSize = fDefaultSizeInPixel;

		CDXGraphicsDriver* pGraphics = CDXGraphicsDriver::GetInstance();


		///////////////////////// DEFAULT FONT TEXTURE ///////////////////////////////
		// Create a texture for the default font.
		DXTexture2DDesc descTex;
		ZeroMemory(&descTex, sizeof(descTex));
		descTex.bFileTexture = true;
		descTex.bindFlags = DX_BIND_SHADER_RESOURCE;
		descTex.gpuUsage = DX_GPU_READ_ONLY;
		wcscpy_s(descTex.textureData.szFileName, DX_MAX_FILE_NAME, szDefaultFontTexture);
		
		DX_V(pGraphics->CreateAndRecreateResource(DX_RESOURCE_TYPE_TEXTURE, &descTex, (IDXResource**)&m_pTexDefaultFont));
		if (FAILED(hr))
		{
			DX_DEBUG_OUTPUT0(DX_ERROR_DEFAULTFONTTEXTURE_FAIL);
			return hr;
		}

		DX_RESOURCE_SET_DEBUG_NAME(m_pTexDefaultFont, "DEFAULT FONT");
		m_pTexCurrentFont = m_pTexDefaultFont;
		///////////////////////////////////////////////////////////////////////////////


		m_pBitmapFontGlyphData = new CDXBitmapFontFile();
		DX_V(m_pBitmapFontGlyphData->LoadFont(szDefaultGlyphDataFile));
		if (FAILED(hr))
		{
			DX_DEBUG_OUTPUT0(DX_ERROR_DEFAULTFONTGLYPHFILE_FAIL);
			return hr;
		}
		m_pGlyphData = (DXBitmapFontGlyphData*)m_pBitmapFontGlyphData->GetGlyphData();


		//////////////////////////////// SPRITE ///////////////////////////////////////
		DX_V(m_pSprite->Create(L"TEXT RENDERER", pGraphics->GetQuadVertexShader(), DX_MAX_TEXT_VERTICES));
		if (FAILED(hr))
		{
			DX_DEBUG_OUTPUT0(DX_ERROR_SPRITE_FAIL);
			return hr;
		}
		m_pSprite->SetInputRectCoordMode(false);
		///////////////////////////////////////////////////////////////////////////////


		m_pBSTransparent = pGraphics->GetTransparentBlend();
		m_pVS = pGraphics->GetQuadVertexShader();
		m_pPS = pGraphics->GetQuadPixelShader();

		return hr;
	}


	void CDXTextRenderer::Shutdown()
	{
		DX_SAFE_RELEASE(m_pTexDefaultFont);
		DX_SAFE_DELETE(m_pBitmapFontGlyphData);
		if (m_pSprite)
		{
			m_pSprite->Destroy();
		}
	}


	void CDXTextRenderer::OnBackBufferResize(float fBackBufferWidth, float fBackBufferHeight)
	{
		m_fReciprocalBackBufferWidth = 1.0f / fBackBufferWidth;
		m_fReciprocalBackBufferHeight = 1.0f / fBackBufferHeight;

		m_pSprite->OnBackBufferResize(fBackBufferWidth, fBackBufferHeight);
		
		// Just to update the glyph dimensions.
		SetFontSize(m_fFontSize);
	}


	void CDXTextRenderer::BeginText(float fStartX, float fStartY, bool bInPixels, bool bSaveState)
	{
		static CDXGraphicsDriver* pGraphics = CDXGraphicsDriver::GetInstance();

		m_bStateSaved = bSaveState;
		if (bSaveState)
		{
			m_pTexTemp = m_p3dDriver->GetPSTexture(0);
			m_pBSTemp = m_p3dDriver->GetBlendState();
			m_pVSTemp = m_p3dDriver->GetVertexShader();
			m_pPSTemp = m_p3dDriver->GetPixelShader();
		}


		m_p3dDriver->SetPSTextures(0, 1, &m_pTexCurrentFont);
		m_p3dDriver->SetBlendState(m_pBSTransparent, nullptr);
		m_p3dDriver->SetVertexShader(m_pVS);
		m_p3dDriver->SetPixelShader(m_pPS);
		if (bInPixels)
		{
			NormalizedScreenSpaceToProjectionSpace(fStartX*m_fReciprocalBackBufferWidth, fStartY*m_fReciprocalBackBufferHeight, &m_fStartX, &m_fStartY);
		}
		else
		{
			NormalizedScreenSpaceToProjectionSpace(fStartX, fStartY, &m_fStartX, &m_fStartY);
		}

		m_pSprite->BeginBatchRendering();
	}


	void CDXTextRenderer::EndText()
	{
		m_pSprite->EndBatchRendering();
		if (m_bStateSaved)
		{
			m_p3dDriver->SetBlendState(m_pBSTemp, nullptr);
			m_p3dDriver->SetPSTextures(0, 1, &m_pTexTemp);
			m_p3dDriver->SetVertexShader(m_pVSTemp);
			m_p3dDriver->SetPixelShader(m_pPSTemp);

			m_bStateSaved = false;
		}
	}


	void CDXTextRenderer::RenderTextProjectionSpace(LPCWSTR szText, float fX, float fY, float fDepth)
	{

		int iNumVertices = 0;
		int iStringLength = (int)wcslen(szText);
		float fStartX = fX;
		float fStartY = fY;
		int i;

		int iRenderableChars = 0;
		for (i=0; i<iStringLength; i++)
		{
			if (szText[i] > ' ' && szText[i] <= '~')
			{
				++iRenderableChars;
			}
		}

		for (i=0; i<iStringLength; i++)
		{
			// If the number of vertices are going to be more than the vertex buffer's
			// size, then we break the loop and render what has already been filled.
			if (iNumVertices+6 > DX_MAX_TEXT_VERTICES)
			{
				break;
			}

			if (szText[i] == '\n')
			{
				fStartX = fX;
				fStartY -= m_fGlyphHeight;
				continue;
			}

			DXBitmapFontGlyphData* pGlyph = &m_pGlyphData[szText[i]];

			float xFactor = m_fTextureWidth * m_fGlyphWidth / 48.0f;
			float yFactor = m_fTextureHeight * m_fGlyphHeight / 48.0f;
			float offsetX = pGlyph->fOffsetX * xFactor;
			float offsetY = pGlyph->fOffsetY * yFactor;
			float width = (pGlyph->rectTex.fRight - pGlyph->rectTex.fLeft) * xFactor;
			float height = (pGlyph->rectTex.fBottom - pGlyph->rectTex.fTop) * yFactor;

			float fX1 = fStartX + offsetX;
			float fX2 = fX1 + width;
			float fY1 = fStartY - offsetY;
			float fY2 = fY1 - height;

			DXRect rect(fX1, fY1, fX2, fY2);
			DXRect rectTex(pGlyph->rectTex.fLeft, pGlyph->rectTex.fTop, pGlyph->rectTex.fRight, pGlyph->rectTex.fBottom);
			
			m_pSprite->BatchRenderSprite(&rect, &rectTex, &m_f4FontColor, fDepth);
			iNumVertices += 6;
			fStartX += pGlyph->fAdvanceX * xFactor;
		}

		// If there are still more characters to process, then pass the remaining
		// to a recursive call to this method.
		if (iNumVertices < iRenderableChars * 6)
		{
			RenderTextProjectionSpace( &(szText[i]), fStartX, fStartY, fDepth);
		}
	}


	void CDXTextRenderer::GetTextMetrics(LPCWSTR szText, float fFontSize, UINT uiInLength, float* pfOffsetsInPixels)
	{
		float fStartX = 0.0f;

		float xFactor = m_fTextureWidth * 2 * fFontSize * m_fReciprocalBackBufferWidth / 48.0f;

		for (UINT i=0; i<uiInLength; i++)
		{
			DXBitmapFontGlyphData* pGlyph = &m_pGlyphData[szText[i]];

			float offsetX = pGlyph->fOffsetX * xFactor;
			float width = (pGlyph->rectTex.fRight - pGlyph->rectTex.fLeft) * xFactor;

			fStartX += pGlyph->fAdvanceX * xFactor;
			pfOffsetsInPixels[i] = fStartX;
		}
	}

} // DXFramework namespace end