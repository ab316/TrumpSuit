#include "DXStdafx.h"
#include "DXTexture.h"
#include "DXTextureManager.h"
#include "DX3DDriver.h"
#include "DX2DDriver.h"

namespace DXFramework
{ // DXFramework namespace begin


	#define	DX_OUTPUT_FORMAT		DXGI_FORMAT_R8G8B8A8_UNORM


	CDX2DDriver::CDX2DDriver() :	m_pD3d11Device(0),
									m_pD3d10Device(0),
									m_pKeyedMutex10(0),
									m_pKeyedMutex11(0),
									m_pSharedTexture(0),
									m_pD2dFactory(0),
									m_pD2dRenderTarget(0),
									m_pD2dSolidBrush(0),
									m_pDWriteFactory(0),
									m_pSharedTextureView(0)
	{
	}


	CDX2DDriver::~CDX2DDriver()
	{
	}


	void CDX2DDriver::ResetRenderingState()
	{
		m_State.fTextPosX = 0.0f;
		m_State.fTextPosY = 0.0f;
		m_State.fStrokeWidth = 1.0f;
		m_State.fTextVerticalSpacing = 1.0f;
		m_State.bLineByLineText = false;
		SetBrush(DX_2D_BRUSH_TYPE_SOLID);
		SetFont(m_pFontDefault);
		SetSolidColor(1.0f, 1.0f, 1.0f, 1.0f);
	}


	HRESULT	CDX2DDriver::Startup(IDXGIAdapter1* pAdapter, ID3D11Device* pD3D11Device)
	{
		HRESULT hr = S_OK;

		m_pD3d11Device = pD3D11Device;

		// Create the D3D10.1 device.
		UINT uFlags = D3D10_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef DX_DEBUG
		uFlags |= D3D10_CREATE_DEVICE_DEBUG;
#endif
		DX_V_RETURN(D3D10CreateDevice1(pAdapter, D3D10_DRIVER_TYPE_HARDWARE, 0, uFlags, D3D10_FEATURE_LEVEL_10_1, D3D10_1_SDK_VERSION, &m_pD3d10Device));

		// Just to silent the D3D10.1 debug warning. We won't be rendering with D3D10.1
		m_pD3d10Device->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_POINTLIST);



		// Create the Direct2D factory.
		D2D1_FACTORY_OPTIONS options;
#ifdef DX_DEBUG
		options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#else
		options.debugLevel = D2D1_DEBUG_LEVEL_NONE;
#endif
		// Single threaded because we won't be rendering from multiple threads.
		D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, options, &m_pD2dFactory);

		// Create the direct Write factory.
		DX_V_RETURN(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), (IUnknown**)&m_pDWriteFactory));
		
		// Create a default font.
		m_pFontDefault = new DXFont();
		CreateFont(m_pFontDefault);

		m_State.bStarted = true;
		
		return hr;
	}


	HRESULT	CDX2DDriver::CreateResources(UINT uiWidth, UINT uiHeight)
	{
		HRESULT hr = S_OK;
		
		// Release the device dependent resource if already exists. This
		// happens when this function this called during a window resize event.
		ReleaseResources();

		m_State.fWidth = (float)uiWidth;
		m_State.fHeight = (float)uiHeight;

		///////////////////// SHARED TEXTURE ///////////////////////////

		// The description of the shared texture that will be used
		// by D3D11, and D2D through D3D10.1
		D3D11_TEXTURE2D_DESC sharedTextureDesc;
		ZeroMemory(&sharedTextureDesc, sizeof(sharedTextureDesc));
		sharedTextureDesc.Width = uiWidth;
		sharedTextureDesc.Height = uiHeight;
		// We must use the same render target format for the outputs.
		sharedTextureDesc.Format = DX_OUTPUT_FORMAT;
		sharedTextureDesc.MipLevels = 1;
		sharedTextureDesc.ArraySize = 1;
		// No anti-aliasing.
		sharedTextureDesc.SampleDesc.Count = 1;
		sharedTextureDesc.Usage = D3D11_USAGE_DEFAULT;
		// This texture will be used as a render target (it will be rendered to)
		// and will be also be input to a shader.
		sharedTextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		// We want shared access of this texture b/w 2 devices.
		sharedTextureDesc.MiscFlags = D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX;


		// Create the texture and set its name.
		DX_V_RETURN(m_pD3d11Device->CreateTexture2D(&sharedTextureDesc, 0, &m_pSharedTexture));
		DX_SET_D3D_DEBUG_NAME(m_pSharedTexture, "DX D2D D3D10_1 Shared");
		
		// Creates a shader resource view for the shared texture. This view
		// will be used as input to the pixel shader.
		DX_V_RETURN(m_pD3d11Device->CreateShaderResourceView(m_pSharedTexture, 0, &m_pSharedTextureView));
		DX_SET_D3D_DEBUG_NAME(m_pSharedTextureView, "DX D2D D3D10_1 Shared Texture");

		// Acquire the keyed mutex for the texture for the d3d11 device.
		// A keyed mutex is used to acquire exclusive access to the resource.
		DX_V_RETURN(m_pSharedTexture->QueryInterface(__uuidof(IDXGIKeyedMutex), (void**)&m_pKeyedMutex11));


		IDXGIResource* pSharedResource10;
		HANDLE hSharedHandle;
		// The D3D10.1 device can't access the d3d11 texture directly. So we acquire
		// the common IDXGIResource interface from the texture.
		DX_V_RETURN(m_pSharedTexture->QueryInterface(__uuidof(IDXGIResource), (void**)&pSharedResource10));
		// No we need a shared handle to this resource so that d3d10.1 can access it.
		DX_V_RETURN(pSharedResource10->GetSharedHandle(&hSharedHandle));
		// Release the reference to the shared resource. We're done with it.
		pSharedResource10->Release();

		IDXGISurface1* pSharedSurface10;
		// OpenSharedResource gives the d3d10.1 device access to a resource created
		// on a different d3d device (d3d11 device in this case). We are acquiring
		// the shared surface interface because the d2d create render target function
		// requires an IDXGISurface1 interface.
		DX_V_RETURN(m_pD3d10Device->OpenSharedResource(hSharedHandle, __uuidof(IDXGISurface1), (void**)&pSharedSurface10));
		
		// No we acquire the keyed mutex for the resource that will be used
		// by the d3d10 device.
		DX_V_RETURN(pSharedSurface10->QueryInterface(__uuidof(IDXGIKeyedMutex), (void**)&m_pKeyedMutex10));

		//////////////////////////////////////////////////////////////////



		///////////////////////// D2D RENDER TARGET ///////////////////////
		// Describe the direct2D render target.
		D2D1_RENDER_TARGET_PROPERTIES renderTargetProps;
		ZeroMemory(&renderTargetProps, sizeof(renderTargetProps));
		// Hardware rendering.
		renderTargetProps.type = D2D1_RENDER_TARGET_TYPE_DEFAULT;
		// Identify required pixel format autmatically.
		// Pre-multiplied alpha means that the pixel's color values are already
		// adjusted to the alpha.
		renderTargetProps.pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED);

		// Create a d2d render target using the shared surface.
		DX_V_RETURN(m_pD2dFactory->CreateDxgiSurfaceRenderTarget(pSharedSurface10, renderTargetProps, &m_pD2dRenderTarget));

		// Release the reference to the shared surface.
		pSharedSurface10->Release();
		/////////////////////////////////////////////////////////////

		
		////////////////////////// D2D OBJECTS /////////////////////////////
		// Create an opaque solid white brush. We can change the color any time.
		DX_V_RETURN(m_pD2dRenderTarget->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f), &m_pD2dSolidBrush));
		/////////////////////////////////////////////////////////////////
		
		ResetRenderingState();
		m_pFontDefault->SetTextAlignment(DX_TEXT_ALIGNMENT_LEADING);
		m_pFontDefault->SetParagraphAlignment(DX_PARAGRAPH_ALIGNMENT_NEAR);

		for (DXBitmapVectorIterator iter=m_vecpBitmaps.begin(); iter!=m_vecpBitmaps.end(); iter++)
		{
			CreateBitmap(*iter);
		}

		return hr;
	}


	HRESULT CDX2DDriver::CreateFont(DXFont* pFont)
	{
		HRESULT hr = S_OK;

		bool bNew = true;
		if (pFont->pTextFormat)
		{
			pFont->pTextFormat->Release();
			bNew = false;
		}

		DX_V(m_pDWriteFactory->CreateTextFormat(pFont->szName, 0, (DWRITE_FONT_WEIGHT)pFont->weight, (DWRITE_FONT_STYLE)pFont->style, (DWRITE_FONT_STRETCH)pFont->stretch, pFont->fSize, L"en-us", &pFont->pTextFormat));
		if (FAILED(hr))
		{
			pFont->pTextFormat = 0;
			DX_DEBUG_OUTPUT(L"CDX2DDriver: CreateFont() failed.");
		}
		if (bNew)
		{
			m_vecpFonts.push_back(pFont);
		}

		return hr;
	}


	HRESULT CDX2DDriver::CreateFixedText(DXFixedText* pFixedText, DXFont* pFont)
	{
		HRESULT hr = S_OK;

		DX_V(m_pDWriteFactory->CreateTextLayout(pFixedText->szText, pFixedText->uiStringLength, pFont->pTextFormat, pFixedText->fWidth, pFixedText->fHeight, &pFixedText->pTextLayout));
		if (FAILED(hr))
		{
			DX_DEBUG_OUTPUT(L"CDX2DDriver: CreateFixedText(), Failed to create fixed text: %s", pFixedText->szText);
		}

		return hr;
	}


	HRESULT CDX2DDriver::CreateBitmap(DXBitmap* pBitmap)
	{
		HRESULT hr = S_OK;

		bool bNewBitmap = true;

		CDX3DDriver* p3DDriver = CDX3DDriver::GetInstance();
		DXTexture2DDesc descTexture;

		// If the texture does not exists.
		if (!pBitmap->pTexture)
		{
			ZeroMemory(&descTexture, sizeof(descTexture));
			descTexture.bFileTexture = true;
			descTexture.gpuUsage = DX_CPU_READ_WRITE;
			wcscpy_s(descTexture.textureData.szFileName, DX_MAX_FILE_NAME, pBitmap->szFileName);
			
			CDXTextureManager* pTextureManager = CDXTextureManager::GetInstance();
			// Create the file texture.
			pTextureManager->CreateResource(&descTexture).pResource;
			DX_V_RETURN(pBitmap->pTexture->Recreate());
		}

		// If the bitmap already exists, free it.
		if (pBitmap->pBitmap)
		{
			pBitmap->pBitmap->Release();
			bNewBitmap = false;
		}
		
		// Get the texture's description.
		descTexture = *((DXTexture2DDesc*)pBitmap->pTexture->GetDesc());
		// Only one format is supported.
		if (descTexture.textureFormat != DX_FORMAT_B8G8R8A8_UNORM)
		{
			DX_DEBUG_OUTPUT(L"DXBitmap: Create(), Unsupported texture format. Only B8G8R8A8_UNORM supported.");
			hr = E_INVALIDARG;
		}

		// Get the texture's size and format.
		D2D1_SIZE_U size = D2D1::SizeU(descTexture.uiWidth, descTexture.uiHeight);
		D2D1_BITMAP_PROPERTIES properties;
		properties.pixelFormat = D2D1::PixelFormat((DXGI_FORMAT)descTexture.textureFormat, D2D1_ALPHA_MODE_PREMULTIPLIED);
		m_pD2dRenderTarget->GetDpi(&properties.dpiX, &properties.dpiY);
		
		// Create a bitmap using the texture data.
		DXMappedResource mappedTexture;
		p3DDriver->Map(pBitmap->pTexture, DX_MAP_READ, &mappedTexture);
		DX_V(m_pD2dRenderTarget->CreateBitmap(size, properties, &pBitmap->pBitmap));
		if (FAILED(hr))
		{
			DX_DEBUG_OUTPUT("DX2DDriver: CreateBitmap() failed.");
			p3DDriver->Unmap(pBitmap->pTexture);
			return hr;
		}

		p3DDriver->Unmap(pBitmap->pTexture);

		if (bNewBitmap)
		{
			m_vecpBitmaps.push_back(pBitmap);
		}

		return hr;
	}


	void CDX2DDriver::BeginRendering()
	{
		// D3D11 releases the texture using 0 key.
		m_pKeyedMutex11->ReleaseSync(0);
		// D3D10.1 acquires access to texture using 0 key.
		// Access can only be acquired with the key with which the
		// resource was previously restored with.
		m_pKeyedMutex10->AcquireSync(0, 1);
		m_pD2dRenderTarget->BeginDraw();
		m_pD2dRenderTarget->Clear(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f));
		m_State.bRendering = true;
	}

	
	void CDX2DDriver::EndRendering()
	{
		m_pD2dRenderTarget->EndDraw();
		m_pKeyedMutex10->ReleaseSync(1);
		m_pKeyedMutex11->AcquireSync(1, 1);
		m_State.bRendering = false;
	}


	void CDX2DDriver::ReleaseResources()
	{
		DX_SAFE_RELEASE(m_pKeyedMutex11);
		DX_SAFE_RELEASE(m_pKeyedMutex10);
		DX_SAFE_RELEASE(m_pSharedTextureView);
		DX_SAFE_RELEASE(m_pSharedTexture);
		DX_SAFE_RELEASE(m_pD2dSolidBrush);
		DX_SAFE_RELEASE(m_pD2dRenderTarget);
	}


	void CDX2DDriver::Shutdown()
	{
		ReleaseResources();
		
		for (DXFontVectorIterator iter=m_vecpFonts.begin(); iter!=m_vecpFonts.end(); iter++)
		{
			(*iter)->pTextFormat->Release();
		}
		m_vecpFonts.clear();


		for (DXFixedTextVectorIterator iter=m_vecpFixedTexts.begin(); iter!=m_vecpFixedTexts.end(); iter++)
		{
			(*iter)->pTextLayout->Release();
		}
		m_vecpFixedTexts.clear();


		for (DXBitmapVectorIterator iter=m_vecpBitmaps.begin(); iter!=m_vecpBitmaps.end(); iter++)
		{
			DX_SAFE_RELEASE((*iter)->pBitmap);
			DX_SAFE_RELEASE((*iter)->pTexture);
		}


		DX_SAFE_DELETE(m_pFontDefault);

		DX_SAFE_RELEASE(m_pDWriteFactory);
		DX_SAFE_RELEASE(m_pD2dFactory);
		DX_SAFE_RELEASE(m_pD3d10Device);

		m_State.bStarted = false;
	}

} // DXFramework namespace end