#include "DXStdafx.h"
#include "DX3DDriver.h"
#include "DXResourcesInclude.h"
#include "DXResourceManagersInclude.h"
#include "DXUIManager.h"
#include "DXTextRenderer.h"
#include "DXGraphicsDriver.h"



namespace DXFramework
{ // DXFramework namespace begin

#define DX_GUI_SHADER_FILE				L"GUI.hlsl"
#define DX_DEFAULT_FONT_FILE			L"Ariel_0.dds"
#define DX_DEFAULT_FONT_GLYPHDATA_FILE	L"Ariel.fnt"
#define DX_DEFAULT_FONT_SIZE			32.0f

	CDXGraphicsDriver::CDXGraphicsDriver() :
												m_p3dDriver(nullptr),
												m_pTextRenderer(nullptr),
												m_pManHardwareBuffer(nullptr),
												m_pManInputLayout(nullptr),
												m_pManShader(nullptr),
												m_pManTexture(nullptr),
												m_pCBGUITransform(nullptr),
												m_pVB2DQuad(nullptr),
												m_pIB2DQuad(nullptr),
												m_pILPosTex(nullptr),
												m_pVSGUI(nullptr),
												m_pPSGUI(nullptr),
												m_pTex1x1White(nullptr),
												m_pTexDepthStencil(nullptr),
												m_pBSTransparent(nullptr),
												m_pDSNoDepth(nullptr),
												m_bStarted(false)
	{
		DX_DEBUG_OUTPUT0(L"ctor.");
	}

	
	CDXGraphicsDriver::~CDXGraphicsDriver()
	{
		DX_DEBUG_OUTPUT0(L"dtor.");
	}



	HRESULT CDXGraphicsDriver::Startup(HWND hWnd)
	{
		HRESULT hr = S_OK;

#define DX_ERROR_3DDRIVER_FAIL		L"ERROR!!! Unable to create 3D driver."
#define DX_ERROR_1X1TEXTURE_FAIL	L"ERROR!!! Unable to create 1x1 White texture."
#define DX_ERROR_TEXTRENDERER_FAIL	L"ERROR!!! Unable start text renderer."

		if (m_bStarted)
		{
			hr = E_ILLEGAL_METHOD_CALL;
			DX_DEBUG_OUTPUT0(L"Already started.");
			return hr;
		}
		
		m_WindowState.hWnd = hWnd;
		m_WindowState.bWindowed = true;
		UpdateWindowDimensions();

		// Get the instance of the singleton 3d driver. This is the first time
		// GetInstance() method is called so it forces the creation of the driver.
		m_p3dDriver = CDX3DDriver::GetInstance();
		// Do the same for the text renderer.
		m_pTextRenderer = CDXTextRenderer::GetInstance();

		// Create the resource managers.
		m_pManHardwareBuffer = CDXHardwareBufferManager::GetInstance();
		m_pManInputLayout = CDXInputLayoutManager::GetInstance();
		m_pManShader = CDXShaderManager::GetInstance();
		m_pManTexture = CDXTextureManager::GetInstance();

		// Call the Create() method of the managers, though, it does nothing presently.
		m_pManHardwareBuffer->Create();
		m_pManInputLayout->Create();
		m_pManShader->Create();
		m_pManTexture->Create();

		// Start the 3d driver,
		DX_V(m_p3dDriver->Startup(m_WindowState));
		if (FAILED(hr))
		{
			DX_DEBUG_OUTPUT0(DX_ERROR_3DDRIVER_FAIL);
			return hr;
		}


		m_bStarted = true;


		//////////////////////// 1 X 1 WHITE TEXTURE //////////////////////////////////
		// Create a default texture with only 1 pixel of color (255, 255, 255, 255).
		UINT32 uiWhiteColor = 0xffffffff;
		DXTexture2DDesc descTex;
		descTex.bFileTexture = false;
		descTex.bindFlags = DX_BIND_SHADER_RESOURCE;
		descTex.textureFormat = DX_FORMAT_R8G8B8A8_UNORM;
		descTex.gpuUsage = DX_GPU_READ_ONLY;
		descTex.uiWidth = 1;
		descTex.uiHeight = 1;
		descTex.uiMultisampleLevel = 1;
		descTex.uiMipLevels = 1;
		descTex.textureData.uiPitchOrSize = sizeof(UINT32);
		descTex.textureData.pData = &uiWhiteColor;

		DX_V(CreateAndRecreateResource(DX_RESOURCE_TYPE_TEXTURE, &descTex, (IDXResource**)&m_pTex1x1White, L"1X1 WHITE"));
		if (FAILED(hr))
		{
			DX_DEBUG_OUTPUT0(DX_ERROR_1X1TEXTURE_FAIL);
			return hr;
		}
		DX_RESOURCE_SET_DEBUG_NAME(m_pTex1x1White, "1X1 WHITE");
		///////////////////////////////////////////////////////////////////////////////


		/////////////////////////// TRANSPARENT BLEND STATE ///////////////////////////
		// Create a transparent blend state.
		m_pBSTransparent = new DXBlendState();
		m_pBSTransparent->desc.bBlendEnable = true;
		m_pBSTransparent->desc.blendOp = DX_BLEND_OP_ADD;
		m_pBSTransparent->desc.srcBlend = DX_BLEND_SRC_ALPHA;
		m_pBSTransparent->desc.destBlend = DX_BLEND_INV_SRC_ALPHA;
		m_p3dDriver->CreateBlendState(m_pBSTransparent);
		DX_SET_D3D_DEBUG_NAME(m_pBSTransparent->m_pBlendState, "TRANPARENT");
		///////////////////////////////////////////////////////////////////////////////


		///////////////////////// NO DEPTH DEPTH STENCIL STATE ////////////////////////
		m_pDSNoDepth = new DXDepthStencilState();
		m_pDSNoDepth->desc.bDepthEnable = false;
		m_p3dDriver->CreateDepthStencilState(m_pDSNoDepth);
		DX_SET_D3D_DEBUG_NAME(m_pDSNoDepth->m_pDepthStencilState, "NO DEPTH");
		///////////////////////////////////////////////////////////////////////////////


		/////////////////////////// FULL SCREEN QUAD CALL /////////////////////////////
		// Create the vertex buffer, index buffer, vertex shader, pixel shader
		// and input layout required for rendering a full screen quad on the screen.
		DX_V(CreateFullScreenQuadResources());
		if (FAILED(hr))
		{
			DX_ERROR_MSGBOX(L"DXGraphicsDriver: Startup(), unable create to fullscreen quad resources.");
			return hr;
		}


		///////////////////////////// TEXT RENDERER ///////////////////////////////////
		// The text rendered depends upon the transparent blend state and the 2d quad VS.
		DX_V(m_pTextRenderer->Startup(m_p3dDriver, DX_DEFAULT_FONT_FILE, DX_DEFAULT_FONT_GLYPHDATA_FILE, DX_DEFAULT_FONT_SIZE));
		if (FAILED(hr))
		{
			DX_DEBUG_OUTPUT0(DX_ERROR_TEXTRENDERER_FAIL);
			return hr;
		}
		float fBBWidth = (float)m_p3dDriver->GetBackbufferWidth();
		float fBBHeight = (float)m_p3dDriver->GetBackbufferHeight();
		m_pTextRenderer->OnBackBufferResize(fBBWidth, fBBHeight);
		///////////////////////////////////////////////////////////////////////////////

		CDXUIManager* pUIMan = CDXUIManager::GetInstance();
		DX_V_RETURN(pUIMan->CreateResources());
		pUIMan->OnBackBufferResize(fBBWidth, fBBHeight);

		return hr;
	}


	void CDXGraphicsDriver::Shutdown()
	{
		if (!m_bStarted)
		{
			DX_DEBUG_OUTPUT0(L"Called when not started.");
			return;
		}

		CDXUIManager::GetInstance()->DestroyResources();

		DX_SAFE_RELEASE(m_pCBGUITransform);
		DX_SAFE_RELEASE(m_pVB2DQuad);
		DX_SAFE_RELEASE(m_pIB2DQuad);
		DX_SAFE_RELEASE(m_pTex1x1White);
		DX_SAFE_RELEASE(m_pVSGUI);
		DX_SAFE_RELEASE(m_pPSGUI);
		DX_SAFE_RELEASE(m_pILPosTex);
		DX_SAFE_RELEASE(m_pTexDepthStencil);

		// Text render creates resources, therefore it must shutdown before the resource
		// managers.
		m_pTextRenderer->Shutdown();

		m_pManHardwareBuffer->Destroy();
		m_pManInputLayout->Destroy();
		m_pManShader->Destroy();
		m_pManTexture->Destroy();

		m_p3dDriver->Shutdown();

		DX_SAFE_DELETE(m_pBSTransparent);
		DX_SAFE_DELETE(m_pDSNoDepth);

		m_bStarted = false;
	}


	HRESULT CDXGraphicsDriver::CreateWindowDependentResources()
	{
		HRESULT hr = S_OK;

#define DX_ERROR_RENDERTARGET_FAIL L"\nDXGraphicsDriver: CreateWindowDependentResources(), failed to create render target view.\n"
#define DX_ERROR_DEPTHSTENCIL_FAIL L"\nDXGraphicsDriver: CreateWindowDependentResources(), failed to create depth stencil.\n"


		if (!m_bStarted)
		{
			DX_ERROR_MSGBOX(L"DXGraphicsDriver: CreateWindowDependentResources() called when not started.");
			hr = E_ILLEGAL_METHOD_CALL;
			return hr;
		}

		DX_V(m_p3dDriver->CreateRenderTargetView());
		if (FAILED(hr))
		{
			DX_DEBUG_OUTPUT0(DX_ERROR_RENDERTARGET_FAIL);
			return hr;
		}


		// Create a depth stencil texture.
		DXTexture2DDesc depthDesc;
		ZeroMemory(&depthDesc, sizeof(depthDesc));

		depthDesc.bFileTexture = false;
		depthDesc.bindFlags = DX_BIND_DEPTH_STENCIL;
		depthDesc.gpuUsage = DX_GPU_READ_WRITE_ONLY;
		depthDesc.textureFormat = DX_FORMAT_D24_UNORM_S8_UINT;
		depthDesc.uiMipLevels = 1;
		depthDesc.uiMultisampleLevel = 4;
		depthDesc.uiWidth = (UINT16)m_WindowState.uiWidth;
		depthDesc.uiHeight = (UINT16)m_WindowState.uiHeight;
		
		CDXTexture* pDepthStencil = (CDXTexture*)m_pManTexture->GetResource(L"DepthStencil");
		if (pDepthStencil)
		{
			pDepthStencil->Release();
			m_pManTexture->DestroyResource(pDepthStencil);
		}
		DX_V(CreateAndRecreateResource(DX_RESOURCE_TYPE_TEXTURE, &depthDesc, (IDXResource**)&m_pTexDepthStencil, L"DepthStencil"));
		if (FAILED(hr))
		{
			DX_DEBUG_OUTPUT0(DX_ERROR_DEPTHSTENCIL_FAIL);
			return hr;
		}
		DX_RESOURCE_SET_DEBUG_NAME(m_pTexDepthStencil, "DepthStencil");

		return hr;
	}


	void CDXGraphicsDriver::UpdateWindowDimensions()
	{
		RECT dimensions;
		GetClientRect(m_WindowState.hWnd, &dimensions);
		m_WindowState.uiWidth = dimensions.right - dimensions.left;
		m_WindowState.uiHeight = dimensions.bottom - dimensions.top;
	}


	HRESULT CDXGraphicsDriver::HandleWindowResize()
	{
		HRESULT hr = S_OK;

		UpdateWindowDimensions();
		m_WindowState.bWindowed = m_p3dDriver->IsWindowed();

		// If the new state is the same as the existing state of the
		// 3d driver, no need to work.
		if (m_p3dDriver->WindowStateSame(m_WindowState))
		{
			return hr;
		}

		// Otherwise, we update the window state of the 3d driver and recreate
		// the resources that depend on the windows size.
		m_p3dDriver->UpdateWindowState(m_WindowState);
		DX_V(CreateWindowDependentResources());
		if (FAILED(hr))
		{
			DX_ERROR_MSGBOX(L"CDXGraphicsDriver: HandleWindowResize() failed.");
			return hr;
		}

		CDXUIManager::GetInstance()->OnBackBufferResize((float)m_WindowState.uiWidth, (float)m_WindowState.uiHeight);
		m_pTextRenderer->OnBackBufferResize((float)m_WindowState.uiWidth, (float)m_WindowState.uiHeight);

		// Reset the pipeline state.
		m_p3dDriver->ResetPipelineState();

		return hr;
	}


	HRESULT CDXGraphicsDriver::CreateFullScreenQuadResources()
	{
		HRESULT hr = S_OK;

		// A quad that covers the entire screen.
		DXScreenVertex vertices[] =
		{
			{ XMFLOAT3(-1.0f, -1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) },
			{ XMFLOAT3(-1.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) },
			{ XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) },
			{ XMFLOAT3(1.0f, -1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) }
		};


		// Indices that define two triangles.
		WORD indices[] =
		{
			0, 1, 2,
			0, 2, 3
		};


		// A handle for the created resources.
		DXResourceHandle resourceHandle;
		////////////// INDEX BUFFER /////////////////////
		DXHardwareBufferDesc descBuffer;
		descBuffer.bufferType = DX_HARDWAREBUFFER_TYPE_INDEXBUFFER;
		descBuffer.gpuUsage = DX_GPU_READ_ONLY;
		descBuffer.uiBufferStride = sizeof(WORD);
		descBuffer.uiBufferSize = sizeof(WORD) * 6;
		descBuffer.pBufferData = indices;
		resourceHandle = m_pManHardwareBuffer->CreateResource(&descBuffer, L"Screen Quad");
		m_pIB2DQuad = (CDXHardwareBuffer*)resourceHandle.pResource;
		DX_V_RETURN(m_pIB2DQuad->Recreate());
		DX_RESOURCE_SET_DEBUG_NAME(m_pIB2DQuad, "Screen Quad");


		///////////// VERTEX BUFFER ///////////////////
		descBuffer.bufferType = DX_HARDWAREBUFFER_TYPE_VERTEXBUFFER;
		descBuffer.uiBufferStride = sizeof(DXScreenVertex);
		descBuffer.uiBufferSize = sizeof(DXScreenVertex) * 4;
		descBuffer.pBufferData = vertices;
		m_pVB2DQuad = (CDXHardwareBuffer*)m_pManHardwareBuffer->CreateResource(&descBuffer, L"Screen Quad").pResource;
		DX_V_RETURN(m_pVB2DQuad->Recreate());
		DX_RESOURCE_SET_DEBUG_NAME(m_pVB2DQuad, "Screen Quad");


		//////////////// CONSTANT BUFFER //////////////
		descBuffer.bufferType = DX_HARDWAREBUFFER_TYPE_CONSTANTBUFFER;
		descBuffer.uiBufferSize = sizeof(XMFLOAT4X4A);
		descBuffer.gpuUsage = DX_GPU_READ_WRITE_ONLY;
		resourceHandle = CreateResource(DX_RESOURCE_TYPE_HARDWARE_BUFFER, &descBuffer, L"GUI Transformation");
		if (resourceHandle == DX_INVALID_RESOURCE_HANDLE)
		{
			DX_DEBUG_OUTPUT0(L"ERROR!!! Failed to create GUI constant buffer.");
			return E_FAIL;
		}
		m_pCBGUITransform = (CDXHardwareBuffer*)resourceHandle.pResource;
		DX_V(RecreateResource(&resourceHandle));
		if (FAILED(hr))
		{
			DX_DEBUG_OUTPUT0(L"ERROR!!! Failed to create GUI constant buffer.");
			return hr;
		}


		/////////////// VERTEX AND PIXEL SHADERS ///////////////////
		DXShaderDesc descVS = { L"GUI.hlsl", L"VS", L"vs_4_0", DX_SHADER_VERTEX_SHADER, 0, nullptr };
		DXShaderDesc descPS = { L"GUI.hlsl", L"PS", L"ps_4_0", DX_SHADER_PIXEL_SHADER, 0, nullptr };

		resourceHandle = m_pManShader->CreateResource(&descVS);
		m_pVSGUI = (CDXShader*)resourceHandle.pResource;
		if (resourceHandle == DX_INVALID_RESOURCE_HANDLE)
		{
			DX_ERROR_MSGBOX(L"Failed to created GUI vertex shader.");
			return E_FAIL;
		}

		DX_V(RecreateResource(&resourceHandle));
		if (FAILED(hr))
		{
			DX_ERROR_MSGBOX(L"Failed to created GUI vertex shader.");
			return hr;
		}

		resourceHandle = CreateResource(DX_RESOURCE_TYPE_SHADER, &descPS);
		m_pPSGUI = (CDXShader*)resourceHandle.pResource;
		DX_V(RecreateResource(&resourceHandle));
		if (FAILED(hr))
		{
			DX_ERROR_MSGBOX(L"Failed to created GUI vertex shader.");
			return hr;
		}
		
		DX_RESOURCE_SET_DEBUG_NAME(m_pVSGUI, "GUI");
		DX_RESOURCE_SET_DEBUG_NAME(m_pPSGUI, "GUI");
		
		return hr;
	}


	void CDXGraphicsDriver::BeginRendering()
	{
		m_p3dDriver->SetOutputs(DX_DEFAULT_RENDER_TARGET, m_pTexDepthStencil);
	}


	void CDXGraphicsDriver::EndRendering()
	{
		static CDXUIManager* pUIMan = CDXUIManager::GetInstance();
		pUIMan->Render();
		//RenderGUIQuad();
		m_p3dDriver->Present();
	}


	void CDXGraphicsDriver::Clear(bool bRenderTarget, float fColorRGBA[4])
	{
		if (bRenderTarget)
		{
			m_p3dDriver->ClearRenderTarget(fColorRGBA);
		}

		// We won't be dealing with stencil for now.
		m_p3dDriver->ClearDepthStencil(1.0f, 0, DX_CLEAR_DEPTH|DX_CLEAR_STENCIL);
	}


	DXResourceHandle CDXGraphicsDriver::CreateResource(DX_RESOURCE_TYPE type, void* pDesc, WCHAR* szCustomKey)
	{
		switch (type)
		{
		case DX_RESOURCE_TYPE_TEXTURE:
			return m_pManTexture->CreateResource(pDesc, szCustomKey);
			break;

		case DX_RESOURCE_TYPE_HARDWARE_BUFFER:
			return m_pManHardwareBuffer->CreateResource(pDesc, szCustomKey);
			break;

		case DX_RESOURCE_TYPE_INPUT_LAYOUT:
			return m_pManInputLayout->CreateResource(pDesc, szCustomKey);
			break;

		case DX_RESOURCE_TYPE_SHADER:
			return m_pManShader->CreateResource(pDesc, szCustomKey);
			break;

		default:
			return DX_INVALID_RESOURCE_HANDLE;
			break;
		}
	}


	HRESULT CDXGraphicsDriver::RecreateResource(DXResourceHandle* pHandle)
	{
		switch (pHandle->pResource->GetResourceType())
		{
		case DX_RESOURCE_TYPE_TEXTURE:
			return m_pManTexture->RecreateResource(*pHandle);
			break;

		case DX_RESOURCE_TYPE_HARDWARE_BUFFER:
			return m_pManHardwareBuffer->RecreateResource(*pHandle);
			break;

		case DX_RESOURCE_TYPE_INPUT_LAYOUT:
			return m_pManInputLayout->RecreateResource(*pHandle);
			break;

		case DX_RESOURCE_TYPE_SHADER:
			return m_pManShader->RecreateResource(*pHandle);
			break;

		default:
			return E_INVALIDARG;
			break;
		}
	}


	HRESULT CDXGraphicsDriver::CreateAndRecreateResource(DX_RESOURCE_TYPE type, void* pDesc, IDXResource** ppOutResource, WCHAR* szCustomKey)
	{
		HRESULT hr = S_OK;
		DXResourceHandle handle;

		try
		{
			*ppOutResource = nullptr;

			switch (type)
			{
			case DX_RESOURCE_TYPE_TEXTURE:
				handle = m_pManTexture->CreateResource(pDesc, szCustomKey);
				break;

			case DX_RESOURCE_TYPE_HARDWARE_BUFFER:
				handle = m_pManHardwareBuffer->CreateResource(pDesc, szCustomKey);
				break;

			case DX_RESOURCE_TYPE_INPUT_LAYOUT:
				handle = m_pManInputLayout->CreateResource(pDesc, szCustomKey);
				break;

			case DX_RESOURCE_TYPE_SHADER:
				handle = m_pManShader->CreateResource(pDesc, szCustomKey);
				break;

			default:
				handle = DX_INVALID_RESOURCE_HANDLE;
				break;
			}

			if (handle == DX_INVALID_RESOURCE_HANDLE)
			{
				DX_DEBUG_OUTPUT2(L"ERROR!!! Failed. TYPE: %s, KEY: %s.", GetResourceTypeName(type), (szCustomKey) ? szCustomKey : L"UNSPECIFIED");
				return E_FAIL;
			}

			if (FAILED(RecreateResource(&handle)))
			{
				DX_DEBUG_OUTPUT2(L"ERROR!!! Failed. TYPE: %s, KEY: %s.", GetResourceTypeName(type), (szCustomKey) ? szCustomKey : L"UNSPECIFIED");
				return hr;
			}

			*ppOutResource = handle.pResource;
		}
		catch (...)
		{
			DX_DEBUG_OUTPUT2(L"\nERROR!!!: Exception raised!, Resource TYPE: %s, KEY: %s\n", GetResourceTypeName(type), (szCustomKey) ? szCustomKey : L"UNSPECIFIED");
		}

		return hr;
	}

} // DXFramework namespace end