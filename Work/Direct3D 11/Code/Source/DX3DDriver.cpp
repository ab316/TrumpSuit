#include "DXStdafx.h"
#include "DXFileSystem.h"
#include "DX3DDriver.h"


namespace DXFramework
{ // DXFramework namespace begin

#define DX_MULTISAMPLE_COUNT		4
#define	DX_OUTPUT_FORMAT			DXGI_FORMAT_R8G8B8A8_UNORM


	CDX3DDriver::CDX3DDriver() :	m_uiVideoMemory(0),
									m_pAdapter(nullptr),
									m_pD3dDevice(nullptr),
									m_pImmediateContext(nullptr),
									m_pMainSwapChain(nullptr),
									m_pRenderTargetView(nullptr),
									m_pBlendStateDefault(nullptr),
									m_pRasterizerStateDefault(nullptr),
									m_pSamplerDefault(nullptr),
									m_pDepthStencilStateDefault(nullptr)
	{
	}


	CDX3DDriver::~CDX3DDriver()
	{
	}


	void CDX3DDriver::FindBestAdapter()
	{
		std::vector<IDXGIAdapter1*> vec_pAdapters;

		// A dxgi factory is used to create dxgi objects which are common
		// b/w different D3Ds. The '1' means we are using DXGI 1.1.
		IDXGIFactory1* pFactory = nullptr;
		CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)&pFactory);

		// Enumerate all the graphic adapters (video cards).
		for (UINT i=0; pFactory->EnumAdapters1(i, &m_pAdapter) != DXGI_ERROR_NOT_FOUND; i++)
		{
			vec_pAdapters.push_back(m_pAdapter);
		}

		if (vec_pAdapters.size() == 0)
		{
			DX_ERROR_MSGBOX( L"DX3DDriver: No video adapter found." );
			return;
		}

		// Get the first adapter.
		m_pAdapter = vec_pAdapters[0];
		DXGI_ADAPTER_DESC1 descAdapter0;
		m_pAdapter->GetDesc1(&descAdapter0);
		m_uiVideoMemory = descAdapter0.DedicatedVideoMemory + descAdapter0.SharedSystemMemory;

		// loop through the remaining adapters.
		for (int i=1; i<(int)vec_pAdapters.size(); i++)
		{
			DXGI_ADAPTER_DESC1 descLastAdapter;
			DXGI_ADAPTER_DESC1 descCurrentAdapter;

			IDXGIAdapter1* pCurrentAdapter = vec_pAdapters[i];

			m_pAdapter->GetDesc1(&descLastAdapter);
			pCurrentAdapter->GetDesc1(&descCurrentAdapter);

			// Get the total video memory of the adapters.
			SIZE_T iLastAdapterMem = descLastAdapter.DedicatedVideoMemory + descLastAdapter.SharedSystemMemory;
			SIZE_T iCurrentAdapterMem = descCurrentAdapter.DedicatedVideoMemory + descCurrentAdapter.SharedSystemMemory;

			// If new video memory is greater than last one. This is our choice.
			// Free the last adapter and set it to our new one.
			if (iCurrentAdapterMem > iLastAdapterMem)
			{
				m_uiVideoMemory = iCurrentAdapterMem;
				m_pAdapter->Release();
				m_pAdapter = pCurrentAdapter;
			}
		}

		DX_SAFE_RELEASE( pFactory );
	}


	void CDX3DDriver::GetAppropriateD3DUsage(DX_GPU_RESOURCE_USAGE inUsage, D3D11_USAGE* pUsage, UINT* pCpuAccess)
	{
		D3D11_USAGE usage;
		UINT uiCpuAccess = 0;

		switch (inUsage)
		{
		case DX_GPU_READ_ONLY:
			usage = D3D11_USAGE_IMMUTABLE;
			break;

		case DX_GPU_READ_WRITE_ONLY:
			usage = D3D11_USAGE_DEFAULT;
			break;

		case DX_GPU_READ_CPU_WRITE:
			usage = D3D11_USAGE_DYNAMIC;
			uiCpuAccess = D3D11_CPU_ACCESS_WRITE;
			break;

		case DX_CPU_READ_WRITE:
			usage = D3D11_USAGE_STAGING;
			uiCpuAccess = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
			break;

		default:
			usage = (D3D11_USAGE)-1;
		}

		if (pUsage)
		{
			*pUsage = usage;
		}
		if (pCpuAccess)
		{
			*pCpuAccess = uiCpuAccess;
		}
	}


	void CDX3DDriver::CreateDefaultPipelineStates()
	{
		CreateBlendState(m_pBlendStateDefault);
		DX_SET_D3D_DEBUG_NAME(m_pBlendStateDefault->m_pBlendState, "DX Default");

		CreateRasterizerState(m_pRasterizerStateDefault);
		DX_SET_D3D_DEBUG_NAME(m_pRasterizerStateDefault->m_pRasterState, "DX Default");

		CreateSamplerState(m_pSamplerDefault);
		DX_SET_D3D_DEBUG_NAME(m_pSamplerDefault->m_pSamplerState, "DX Default");

		CreateDepthStencilState(m_pDepthStencilStateDefault);
		DX_SET_D3D_DEBUG_NAME(m_pDepthStencilStateDefault->m_pDepthStencilState, "DX Default");
	}


	HRESULT CDX3DDriver::Startup(DXWindowState windowState)
	{
		HRESULT hr = S_OK;

		DX_PIX_EVENT_BEGIN(0xff0000ff, L"3DDriver Startup");

		m_hWnd = windowState.hWnd;
		m_PipeLineState.bWindowed = true;
		m_PipeLineState.uiWidth = windowState.uiWidth;
		m_PipeLineState.uiHeight = windowState.uiHeight;

		FindBestAdapter();

		// IDXGIOutput object represents a monitor. Only 1 monitor is supported.
		IDXGIOutput* pOutput;
		m_pAdapter->EnumOutputs(0, &pOutput);

		DXGI_MODE_DESC requiredMode;
		DXGI_MODE_DESC closestMode;
		// Our required display mode.
		requiredMode.Width = m_PipeLineState.uiWidth;
		requiredMode.Height = m_PipeLineState.uiHeight;
		requiredMode.RefreshRate.Numerator = 60;
		requiredMode.RefreshRate.Denominator = 1;
		requiredMode.Format = DX_OUTPUT_FORMAT;
		requiredMode.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		requiredMode.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;

		// Find the display mode supported by the video adapter closest to
		// our required one.
		DX_V_RETURN(pOutput->FindClosestMatchingMode(&requiredMode, &closestMode, nullptr));
		DX_SAFE_RELEASE(pOutput);

		// Create a swap chain description.
		DXGI_SWAP_CHAIN_DESC swapChainDesc;
		ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
		swapChainDesc.BufferCount = 1;
		swapChainDesc.BufferDesc = closestMode;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.OutputWindow = m_hWnd;
		swapChainDesc.Windowed = m_PipeLineState.bWindowed;
		// No multisampling.
		swapChainDesc.SampleDesc.Count = DX_MULTISAMPLE_COUNT;
		swapChainDesc.SampleDesc.Quality = 0;

		// Flags for our d3d11 device.
		UINT iFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef DX_DEBUG
		iFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		// Create the d3d11 device and the swap chain.
		DX_V(D3D11CreateDeviceAndSwapChain(m_pAdapter, D3D_DRIVER_TYPE_UNKNOWN, 0, iFlags, nullptr, 0,
			D3D11_SDK_VERSION, &swapChainDesc, &m_pMainSwapChain, &m_pD3dDevice, &m_featureLevel, &m_pImmediateContext));

		if (FAILED(hr))
		{
			DX_ERROR_MSGBOX(L"DX3DDriver: Startup(), unable to create D3D device: %s.", DXGetErrorDescription(hr));
			return hr;
		}


		DX_SET_D3D_DEBUG_NAME(m_pD3dDevice, DX_D3D_DEVICE_NAME);
		DX_SET_D3D_DEBUG_NAME(m_pMainSwapChain, DX_SWAPCHAIN_NAME);
		DX_SET_D3D_DEBUG_NAME(m_pImmediateContext, DX_IMMEDIATE_CONTEXT_NAME);

		m_pBlendStateDefault = new DXBlendState();
		m_pRasterizerStateDefault = new DXRasterizerState();
		m_pSamplerDefault = new DXSamplerState();
		m_pDepthStencilStateDefault = new DXDepthStencilState();

		CreateDefaultPipelineStates();
		ResetPipelineState();

		DX_PIX_EVENT_END;

		return hr;
	}


	void CDX3DDriver::Shutdown()
	{
		if (m_pMainSwapChain && !IsWindowed())
		{
			SetWindowedMode(true);
		}

		DX_SAFE_RELEASE(m_pAdapter);
		DX_SAFE_RELEASE(m_pRenderTargetView);
		DX_SAFE_RELEASE(m_pMainSwapChain);

		m_pImmediateContext->ClearState();
		m_pImmediateContext->Flush();
		DX_SAFE_RELEASE(m_pImmediateContext);

		// Release the blend and rasterizer states.
		for (DXBlendStateVectorIterator iter = m_vecpBlendStates.begin(); iter != m_vecpBlendStates.end(); iter++)
		{
			(*iter)->Release();
		}
		
		for (DXRasterStateVectorIterator iter = m_vecpRasterStates.begin(); iter != m_vecpRasterStates.end(); iter++)
		{
			(*iter)->Release();
		}

		for (DXSamplerStateVectorIterator iter = m_vecpSamplerStates.begin(); iter != m_vecpSamplerStates.end(); iter++)
		{
			(*iter)->Release();
		}

		for (DXDepthStencilStateVectorIterator iter = m_vecpDepthStencilStates.begin(); iter != m_vecpDepthStencilStates.end(); iter++)
		{
			(*iter)->Release();
		}

		// Clears the vectors.
		m_vecpBlendStates.clear();
		m_vecpRasterStates.clear();
		m_vecpSamplerStates.clear();

		DX_SAFE_RELEASE(m_pD3dDevice);

		DX_SAFE_DELETE(m_pBlendStateDefault);
		DX_SAFE_DELETE(m_pRasterizerStateDefault);
		DX_SAFE_DELETE(m_pSamplerDefault);
	}


	void CDX3DDriver::UpdateWindowState(DXWindowState pNewState)
	{
		m_PipeLineState.uiWidth = pNewState.uiWidth;
		m_PipeLineState.uiHeight = pNewState.uiHeight;
	}


	void CDX3DDriver::SetWindowedMode(bool bWindowed)
	{
		if (m_PipeLineState.bWindowed != bWindowed)
		{
			m_PipeLineState.bWindowed = bWindowed;
			m_pMainSwapChain->SetFullscreenState(!bWindowed, nullptr);
		}
	}


	bool CDX3DDriver::WindowStateSame(DXWindowState state)
	{
		if (m_PipeLineState.uiWidth != state.uiWidth || m_PipeLineState.uiHeight != state.uiHeight || m_PipeLineState.bWindowed != state.bWindowed)
		{
			return false;
		}
		else
		{
			return true;
		}
	}


	void CDX3DDriver::ResetPipelineState()
	{
		// Clear the device context state.
		m_pImmediateContext->ClearState();

		// Don't just ZeroMemory the entire struct because we want to
		// preserve the width, height and windowed fields.
		m_PipeLineState.pDepthStencil = nullptr;
		m_PipeLineState.pIndexBuffer = nullptr;
		m_PipeLineState.pInputLayout = nullptr;
		m_PipeLineState.pPixelShader = nullptr;
		m_PipeLineState.pVertexBuffer = nullptr;
		m_PipeLineState.pVertexShader = nullptr;
		m_PipeLineState.uiIndexBufferOffset = 0;
		m_PipeLineState.uiVertexBufferOffset = 0;
		m_PipeLineState.pRenderTarget = DX_DEFAULT_RENDER_TARGET;
		m_PipeLineState.pDepthStencil = nullptr;
		m_PipeLineState.primitveType = DX_PRIMITIVE_TYPE_TRIANGLE_LIST;
		
		ZeroMemory(m_PipeLineState.pPSTextures, sizeof(CDXTexture*) * DX_MAX_PIPELINE_TEXTURES_SLOTS);
		ZeroMemory(m_PipeLineState.pPSSamplers, sizeof(DXSamplerState*) * DX_MAX_PIPELINE_SAMPLERS_SLOTS);

		m_PipeLineState.pPSSamplers[0] = m_pSamplerDefault;
		m_PipeLineState.pBlendState = m_pBlendStateDefault;
		m_PipeLineState.pRasterState = m_pRasterizerStateDefault;
		m_PipeLineState.pDepthStencilState = m_pDepthStencilStateDefault;

		m_pImmediateContext->PSSetSamplers(0, 1, &m_pSamplerDefault->m_pSamplerState);
		m_pImmediateContext->OMSetBlendState(m_pBlendStateDefault->m_pBlendState, 0, 0xffffffff);
		m_pImmediateContext->RSSetState(m_pRasterizerStateDefault->m_pRasterState);
		m_pImmediateContext->OMSetDepthStencilState(m_pDepthStencilStateDefault->m_pDepthStencilState, 1);
		m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		
		m_pImmediateContext->OMSetRenderTargets(1, &m_pRenderTargetView, nullptr);

		ResetViewport();
	}


	HRESULT CDX3DDriver::CreateRenderTargetView()
	{
		HRESULT hr = S_OK;

		// reset our device context's render target.
		m_pImmediateContext->OMSetRenderTargets(0, nullptr, nullptr);

		// release the render target view if already present. This happens when the
		// window is resized.
		// Must release all outstanding references
		// to the swap chain's buffers.
		if (m_pRenderTargetView)
		{
			m_pRenderTargetView->Release();
		}

		// resize the buffers of our swap chain. This is necessary if the
		// window is resized. Because we want the buffers (back and front) to match
		// our window dimensions.
		DX_V_RETURN(m_pMainSwapChain->ResizeBuffers(1, m_PipeLineState.uiWidth, m_PipeLineState.uiHeight, DX_OUTPUT_FORMAT, 0));

		// Get the back buffer of the swap chain. It is simply a 2d texture resource.
		ID3D11Texture2D* pBackBuffer;
		m_pMainSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer);

		// create a render target view using the back buffer of the swap chain.
		// a render target view is a resource that can be written by the output-merger stage at the
		// end of a render pass.
		// A view is used to bind a resource to a pipeline stage.
		DX_V_RETURN(m_pD3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &m_pRenderTargetView));
		// release the reference to the back buffer. Is necessary whenever
		// we acquire a pointer to a COM object using a get function.
		DX_SET_D3D_DEBUG_NAME(pBackBuffer, DX_BACKBUFFER_NAME);
		pBackBuffer->Release();

		DX_SET_D3D_DEBUG_NAME(m_pRenderTargetView, DX_RENDERTARGET_NAME);

		return hr;
	}


	HRESULT CDX3DDriver::CreateTexture2D(DXTexture2DDesc* pDesc, ID3D11Texture2D** ppOutTex)
	{
		HRESULT hr = S_OK;

		// If it is a file texture then create it and acquire its description.
		if (pDesc->bFileTexture)
		{
			DX_V_RETURN(D3DX11CreateTextureFromMemory(m_pD3dDevice, pDesc->textureData.pData, pDesc->textureData.uiPitchOrSize,
				nullptr, nullptr, (ID3D11Resource**)ppOutTex, nullptr));
			
			D3D11_TEXTURE2D_DESC desc;
			(*ppOutTex)->GetDesc(&desc);
			pDesc->uiWidth = (UINT16)desc.Width;
			pDesc->uiHeight = (UINT16)desc.Height;
			pDesc->bindFlags = (UINT16)desc.BindFlags;
			pDesc->uiMipLevels = (UINT16)desc.MipLevels;
			pDesc->uiMultisampleLevel = (UINT16)desc.SampleDesc.Count;
			pDesc->textureFormat = (DX_FORMAT)desc.Format;
		}
		// Otherwise.
		else
		{
			D3D11_TEXTURE2D_DESC descTexture;
			// Fill in the texture description
			ZeroMemory(&descTexture, sizeof(descTexture));
			descTexture.ArraySize = 1;
			descTexture.BindFlags = (UINT)pDesc->bindFlags;
			descTexture.Format = (DXGI_FORMAT)pDesc->textureFormat;
			descTexture.Width = pDesc->uiWidth;
			descTexture.Height = pDesc->uiHeight;
			descTexture.MipLevels = pDesc->uiMipLevels;
			descTexture.SampleDesc.Count = pDesc->uiMultisampleLevel;
			GetAppropriateD3DUsage(pDesc->gpuUsage, &descTexture.Usage, &descTexture.CPUAccessFlags);

			// Initial texture data given. Only 1 mip level expected.
			if (pDesc->textureData.pData)
			{
				D3D11_SUBRESOURCE_DATA subData;
				subData.pSysMem = pDesc->textureData.pData;
				subData.SysMemPitch = (UINT)pDesc->textureData.uiPitchOrSize;

				DX_V_RETURN(m_pD3dDevice->CreateTexture2D(&descTexture, &subData, ppOutTex));
			}
			// No initial texture data given.
			else
			{
				DX_V_RETURN(m_pD3dDevice->CreateTexture2D(&descTexture, nullptr, ppOutTex));
			}
		}

		return hr;
	}


	HRESULT CDX3DDriver::CreateResourceViews(DXTexture2DDesc* pDescTex, ID3D11Texture2D* pTex, ID3D11ShaderResourceView** ppSRView, ID3D11RenderTargetView** ppRTView, ID3D11DepthStencilView** ppDSView)
	{
		HRESULT hr = S_OK;

		if ((pDescTex->bindFlags & DX_BIND_RENDER_TARGET) == DX_BIND_RENDER_TARGET)
		{
			D3D11_RENDER_TARGET_VIEW_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			desc.Texture2D.MipSlice = 0;
			desc.Format = (DXGI_FORMAT)pDescTex->textureFormat;
			desc.ViewDimension = (pDescTex->uiMultisampleLevel == 1) ? D3D11_RTV_DIMENSION_TEXTURE2D : D3D11_RTV_DIMENSION_TEXTURE2DMS;
			
			DX_V_RETURN(m_pD3dDevice->CreateRenderTargetView(pTex, &desc, ppRTView));
		}

		if ((pDescTex->bindFlags & DX_BIND_DEPTH_STENCIL) == DX_BIND_DEPTH_STENCIL)
		{
			D3D11_DEPTH_STENCIL_VIEW_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			desc.Texture2D.MipSlice = 0;
			desc.Format = (DXGI_FORMAT)pDescTex->textureFormat;
			desc.ViewDimension = (pDescTex->uiMultisampleLevel == 1) ? D3D11_DSV_DIMENSION_TEXTURE2D : D3D11_DSV_DIMENSION_TEXTURE2DMS;
			
			DX_V_RETURN(m_pD3dDevice->CreateDepthStencilView(pTex, &desc, ppDSView));
		}

		if ((pDescTex->bindFlags & DX_BIND_SHADER_RESOURCE) == DX_BIND_SHADER_RESOURCE)
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			desc.Texture2D.MipLevels = 1;
			desc.Texture2D.MostDetailedMip = 0;
			desc.Format = (DXGI_FORMAT)pDescTex->textureFormat;
			desc.ViewDimension = (pDescTex->uiMultisampleLevel == 1) ? D3D11_SRV_DIMENSION_TEXTURE2D : D3D11_SRV_DIMENSION_TEXTURE2DMS;
			
			DX_V_RETURN(m_pD3dDevice->CreateShaderResourceView(pTex, &desc, ppSRView));
		}

		return hr;
	}


	HRESULT CDX3DDriver::CreateBuffer(DXHardwareBufferDesc* pDesc, ID3D11Buffer** ppBuffer)
	{
		HRESULT hr = S_OK;

		D3D11_BUFFER_DESC descBuffer;
		// Prepare d3d buffer description.
		ZeroMemory(&descBuffer, sizeof(descBuffer));
		descBuffer.BindFlags = pDesc->bufferType;
		descBuffer.ByteWidth = pDesc->uiBufferSize;
		GetAppropriateD3DUsage(pDesc->gpuUsage, &descBuffer.Usage, &descBuffer.CPUAccessFlags);
		
		// Initial data provided?
		if (pDesc->pBufferData)
		{
			D3D11_SUBRESOURCE_DATA resourceData;
			resourceData.pSysMem = pDesc->pBufferData;

			DX_V_RETURN(m_pD3dDevice->CreateBuffer(&descBuffer, &resourceData, ppBuffer));
		}
		else
		{
			if ( (pDesc->bufferType != DX_HARDWAREBUFFER_TYPE_CONSTANTBUFFER) && (pDesc->gpuUsage == DX_GPU_READ_ONLY || pDesc->gpuUsage == DX_GPU_READ_WRITE_ONLY) )
			{
				DX_DEBUG_OUTPUT0(L"ERROR!!! No initial data provided and GPU usage has no CPU write access.");
				return E_INVALIDARG;
			}
			DX_V_RETURN(m_pD3dDevice->CreateBuffer(&descBuffer, nullptr, ppBuffer));
		}

		return hr;
	}


	HRESULT CDX3DDriver::CreateInputLayout(DXInputLayoutDesc* pDesc, ID3D11InputLayout** ppLayout)
	{
		HRESULT hr = S_OK;

		D3D11_INPUT_ELEMENT_DESC* aD3dInputDesc;
		D3D11_INPUT_ELEMENT_DESC* pD3dInputDesc;
		DXInputLayoutElement* pElement;

		aD3dInputDesc = new D3D11_INPUT_ELEMENT_DESC[pDesc->uiNumElements];
		pD3dInputDesc = aD3dInputDesc;
		pElement = pDesc->pInputElements;

		char sName[16][DX_MAX_IL_ELEMENT_NAME];
		// Fill the d3d11 input element description.
		for (int i=0; i<pDesc->uiNumElements; i++)
		{
			WideCharToMultiByte(CP_ACP, 0, pElement->szName, DX_MAX_IL_ELEMENT_NAME, sName[i], DX_MAX_IL_ELEMENT_NAME, 0, 0);
			pD3dInputDesc->SemanticName = sName[i];
			pD3dInputDesc->SemanticIndex = pElement->uiIndex;
			pD3dInputDesc->Format = (DXGI_FORMAT)pElement->format;
			pD3dInputDesc->InputSlot = pElement->uiInputSlot;
			pD3dInputDesc->InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			pD3dInputDesc->AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
			pD3dInputDesc->InstanceDataStepRate = 0;

			pD3dInputDesc++;
			pElement++;
		}

		CDXShader* pShader = pDesc->pVertexShader;
		if (pShader)
		{
			DX_V_RETURN(m_pD3dDevice->CreateInputLayout(aD3dInputDesc, pDesc->uiNumElements, pShader->GetShaderCode(), pShader->GetSize(), ppLayout));
		}
		else
		{
			DX_DEBUG_OUTPUT0(L"ERROR!!! no vertex shader in description.");
		}

		delete [] aD3dInputDesc;

		return hr;
	}


	HRESULT CDX3DDriver::CompileShader(DXShaderDesc* pDesc, ID3DBlob** ppOutBlob)
	{
		HRESULT	hr = S_OK;

		UINT flags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;
#ifdef DX_DEBUG
		flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
		flags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif

		// A Blob stores arbitrary data.
		ID3DBlob* pErrorBlob;

		char szEntryPoint[DX_MAX_SHADER_ENTRY+1];
		char szProfile[DX_MAX_SHADER_PROFILE+1];
		WideCharToMultiByte(CP_ACP, 0, pDesc->szEntryPoint, -1, szEntryPoint, DX_MAX_SHADER_ENTRY+1, 0, 0);
		WideCharToMultiByte(CP_ACP, 0, pDesc->szShaderLevel, -1, szProfile, DX_MAX_SHADER_PROFILE+1, 0, 0);
		
		IDXShaderIncludeHandler includeHandler;
		DX_V(D3DCompile(pDesc->pBuffer, pDesc->uiBufferSize, nullptr, nullptr, &includeHandler, szEntryPoint, szProfile, flags, 0, ppOutBlob, &pErrorBlob));
		//DX_V(D3DX11CompileFromMemory((LPCSTR)pDesc->pBuffer, pDesc->uiBufferSize, 0, 0, &includeHandler, szEntryPoint, szProfile, D3D10_SHADER_DEBUG|D3D10_SHADER_SKIP_OPTIMIZATION|D3D10_SHADER_PACK_MATRIX_ROW_MAJOR|D3D10_SHADER_ENABLE_STRICTNESS, 0, 0, ppOutBlob, &pErrorBlob, 0));
		// If unable to compile the shader.
		if (FAILED( hr ))
		{
			// If we have an error message returned.
			if (pErrorBlob)
			{
				// Output the error.
				DX_DEBUG_OUTPUT3(L"ERROR!!! FILE: %s, ENTRY: %s, PROFILE: %s", pDesc->szFileName, pDesc->szEntryPoint, pDesc->szShaderLevel);
				DX_DEBUG_OUTPUT("ERROR: %s\n", (char*)pErrorBlob->GetBufferPointer());
			}
			else
			{
				// Otherwise output the error described by the HRESULT hr returned value.
				DX_DEBUG_OUTPUT4(L"ERROR!!! FILE: %s, ENTRY: %s, PROFILE: %s, ERROR: %s", pDesc->szFileName, pDesc->szEntryPoint, pDesc->szShaderLevel, DXGetErrorDescription(hr));
			}
		}

		// Release the blob object so that it can be removed from the pipeline.
		if (pErrorBlob)
		{
			pErrorBlob->Release();
		}

		return hr;
	}


	HRESULT CDX3DDriver::CreateShader(DX_SHADER_TYPE type, ID3DBlob* pShaderData, void** pOutShader)
	{
		HRESULT hr = S_OK;

		switch (type)
		{
		case DX_SHADER_VERTEX_SHADER:
			DX_V_RETURN(m_pD3dDevice->CreateVertexShader(pShaderData->GetBufferPointer(), pShaderData->GetBufferSize(), nullptr, (ID3D11VertexShader**)pOutShader));
			break;

		case DX_SHADER_PIXEL_SHADER:
			DX_V_RETURN(m_pD3dDevice->CreatePixelShader(pShaderData->GetBufferPointer(), pShaderData->GetBufferSize(), nullptr, (ID3D11PixelShader**)pOutShader));
			break;
		}

		return hr;
	}


	HRESULT CDX3DDriver::CreateBlendState(DXBlendState* pState)
	{
		HRESULT hr = S_OK;

		D3D11_BLEND_DESC descBlend;
		D3D11_RENDER_TARGET_BLEND_DESC descRT;
		DXBlendDesc* pDesc = &pState->desc;

		descRT.BlendEnable = pDesc->bBlendEnable;
		descRT.BlendOp = (D3D11_BLEND_OP)pDesc->blendOp;
		descRT.BlendOpAlpha = (D3D11_BLEND_OP)pDesc->blendOpAlpha;
		descRT.DestBlend = (D3D11_BLEND)pDesc->destBlend;
		descRT.DestBlendAlpha = (D3D11_BLEND)pDesc->destBlendAlpha;
		descRT.SrcBlend = (D3D11_BLEND)pDesc->srcBlend;
		descRT.SrcBlendAlpha = (D3D11_BLEND)pDesc->srcBlendAlpha;
		descRT.RenderTargetWriteMask = pDesc->writeMask;
		
		descBlend.AlphaToCoverageEnable = pDesc->bAlphaToCoverage;
		descBlend.IndependentBlendEnable = FALSE;
		descBlend.RenderTarget[0] = descRT;

		DX_V(m_pD3dDevice->CreateBlendState(&descBlend, &pState->m_pBlendState));
		if (FAILED(hr))
		{
			DX_DEBUG_OUTPUT0(L"ERROR!!! Failed.");
			pState->m_pBlendState = nullptr;
			return hr;
		}
		m_vecpBlendStates.push_back(pState->m_pBlendState);

		return hr;
	}


	HRESULT CDX3DDriver::CreateRasterizerState(DXRasterizerState* pState)
	{
		HRESULT hr = S_OK;

		D3D11_RASTERIZER_DESC descRast;
		DXRasterizerDesc* pDesc = &pState->desc;
		descRast.AntialiasedLineEnable = FALSE;
		descRast.CullMode = (D3D11_CULL_MODE)pDesc->cullMode;
		descRast.DepthBias = pDesc->iDepthBias;
		descRast.DepthBiasClamp = pDesc->fDepthBiasClamp;
		descRast.DepthClipEnable = pDesc->bDepthClip;
		descRast.FillMode = (D3D11_FILL_MODE)pDesc->fillMode;
		descRast.FrontCounterClockwise = pDesc->bFrontCCW;
		descRast.MultisampleEnable = pDesc->bMultisample;
		descRast.ScissorEnable = false;
		descRast.SlopeScaledDepthBias = 0.0f;

		DX_V(m_pD3dDevice->CreateRasterizerState(&descRast, &pState->m_pRasterState));
		if (FAILED(hr))
		{
			DX_DEBUG_OUTPUT0(L"ERROR!!! Failed.");
			pState->m_pRasterState = nullptr;
			return hr;
		}
		m_vecpRasterStates.push_back(pState->m_pRasterState);

		return hr;
	}


	HRESULT CDX3DDriver::CreateSamplerState(DXSamplerState* pState)
	{
		HRESULT hr = S_OK;

		D3D11_SAMPLER_DESC descSample;
		DXSamplerDesc* pDesc = &pState->desc;
		descSample.Filter = (D3D11_FILTER)pDesc->filter;
		descSample.AddressU = (D3D11_TEXTURE_ADDRESS_MODE)pDesc->addressU;
		descSample.AddressV = (D3D11_TEXTURE_ADDRESS_MODE)pDesc->addressV;
		descSample.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		descSample.MaxAnisotropy = pDesc->uMaxAnisotropicity;
		descSample.ComparisonFunc = D3D11_COMPARISON_NEVER;
		descSample.MaxLOD = FLT_MAX;
		descSample.MinLOD = FLT_MIN;
		descSample.MipLODBias = 0.0f;
		
		memcpy_s(descSample.BorderColor, sizeof(FLOAT)*4, pDesc->fBorderColor, sizeof(float)*4);

		DX_V(m_pD3dDevice->CreateSamplerState(&descSample, &pState->m_pSamplerState));
		if (FAILED(hr))
		{
			DX_DEBUG_OUTPUT0(L"ERROR!!! Failed.");
			pState->m_pSamplerState = nullptr;
			return hr;
		}

		m_vecpSamplerStates.push_back(pState->m_pSamplerState);

		return hr;
	}


	HRESULT CDX3DDriver::CreateDepthStencilState(DXDepthStencilState* pState)
	{
		HRESULT hr = S_OK;

		D3D11_DEPTH_STENCIL_DESC descDS;
		DXDepthStencilDesc* pDesc = &pState->desc;
		descDS.DepthEnable = pDesc->bDepthEnable;
		descDS.DepthWriteMask = pDesc->bDepthWrite ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
		descDS.StencilEnable = pDesc->bStencilEnable;
		descDS.StencilReadMask = pDesc->uiStencilReadMask;
		descDS.StencilWriteMask = pDesc->uiStencilWriteMask;
		descDS.DepthFunc = (D3D11_COMPARISON_FUNC)pDesc->depthFunc;
		
		descDS.FrontFace.StencilFunc = (D3D11_COMPARISON_FUNC)pDesc->frontFace.stencilFunc;
		descDS.FrontFace.StencilPassOp = (D3D11_STENCIL_OP)pDesc->frontFace.opStencilPass;
		descDS.FrontFace.StencilFailOp = (D3D11_STENCIL_OP)pDesc->frontFace.opStencilFail;
		descDS.FrontFace.StencilDepthFailOp = (D3D11_STENCIL_OP)pDesc->frontFace.opDepthStencilFail;
		
		descDS.BackFace.StencilFunc = (D3D11_COMPARISON_FUNC)pDesc->backFace.stencilFunc;
		descDS.BackFace.StencilPassOp = (D3D11_STENCIL_OP)pDesc->backFace.opStencilPass;
		descDS.BackFace.StencilFailOp = (D3D11_STENCIL_OP)pDesc->backFace.opStencilFail;
		descDS.BackFace.StencilDepthFailOp = (D3D11_STENCIL_OP)pDesc->backFace.opDepthStencilFail;

		DX_V(m_pD3dDevice->CreateDepthStencilState(&descDS, &pState->m_pDepthStencilState));
		if (FAILED(hr))
		{
			DX_DEBUG_OUTPUT0(L"ERROR!!! Failed.");
			pState->m_pDepthStencilState = nullptr;
			return hr;
		}

		m_vecpDepthStencilStates.push_back(pState->m_pDepthStencilState);

		return hr;
	}

} // DXFramework namespace end