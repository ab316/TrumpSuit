#include "DXStdafx.h"
#include "DXHardwareBuffer.h"
#include "DXInputLayout.h"
#include "DXGraphicsDriver.h"
#include "DX3DDriver.h"
#include "DXSprite.h"


namespace DXFramework
{ // DXFramework namespace begin

	CDXSprite::CDXSprite()	:	m_pILTemp(nullptr),
								m_pVBTemp(nullptr),
								m_pVBSprites(nullptr),
								m_p3dDriver(nullptr),
								m_pILSprites(nullptr),
								m_pMappedVB(nullptr),
								m_uiMaxVertices(0),
								m_uiVerticesInBatch(0),
								m_uiVBOffsetTemp(0),
								m_fReciprocalBackBufferWidth(0),
								m_fReciprocalBackBufferHeight(0),
								m_bRendering(false),
								m_bVBMapped(false),
								m_bConvertToClipSpace(true)
	{
		m_pMappedVB = new DXMappedResource();
	}


	CDXSprite::~CDXSprite()
	{
		DX_SAFE_DELETE(m_pMappedVB);
	}


	HRESULT CDXSprite::Create(WCHAR* szName, CDXShader* pSignatureVS, UINT uiNumVertices)
	{
		HRESULT hr = S_OK;

		if (!pSignatureVS || !szName || !uiNumVertices)
		{
			hr = E_INVALIDARG;
			return hr;
		}


		SetName(szName);

		CDXGraphicsDriver* pGraphics = CDXGraphicsDriver::GetInstance();


		////////////////////////////// INPUT LAYOUT /////////////////////////////////////
		DXInputLayoutDesc descIL;
		DXInputLayoutElement elements[3] =
		{
			{ DX_FORMAT_R32G32B32A32_FLOAT, L"COLOR", 0, 0 },
			{ DX_FORMAT_R32G32B32_FLOAT, L"POSITION", 0, 0 },
			{ DX_FORMAT_R32G32_FLOAT, L"TEXCOORD", 0, 0 },
		};

		descIL.pInputElements = elements;
		descIL.uiNumElements = 3;
		descIL.pVertexShader = pSignatureVS;

		DX_V(pGraphics->CreateAndRecreateResource(DX_RESOURCE_TYPE_INPUT_LAYOUT, &descIL, (IDXResource**)&m_pILSprites));
		if (FAILED(hr))
		{
			DX_DEBUG_OUTPUT0(L"ERROR!!! Unable to create Input Layout.");
			return hr;
		}
		DX_RESOURCE_SET_DEBUG_NAME(m_pILSprites, "Sprite IL");
		//////////////////////////////////////////////////////////////////////////////////



		/////////////////////////////// VERTEX BUFFER ////////////////////////////////////
		DXHardwareBufferDesc descBuffer;
		descBuffer.bufferType = DX_HARDWAREBUFFER_TYPE_VERTEXBUFFER;
		descBuffer.gpuUsage = DX_GPU_READ_CPU_WRITE;
		descBuffer.pBufferData = nullptr;
		descBuffer.uiBufferSize = sizeof(DXSpriteVertex) * uiNumVertices;
		descBuffer.uiBufferStride = sizeof(DXSpriteVertex);
		DX_V(pGraphics->CreateAndRecreateResource(DX_RESOURCE_TYPE_HARDWARE_BUFFER, &descBuffer, (IDXResource**)&m_pVBSprites, szName));
		if (FAILED(hr))
		{
			DX_DEBUG_OUTPUT1(L"ERROR!!! Failed to create vertex buffer. %s.", szName);
			return hr;
		}

#ifdef DX_DEBUG
		char name[DX_MAX_NAME+1];
		WideCharToMultiByte(CP_ACP, 0, szName, -1, name, DX_MAX_NAME+1, 0, 0);
		DX_RESOURCE_SET_DEBUG_NAME(m_pVBSprites, name);
#endif
		//////////////////////////////////////////////////////////////////////////////////

		// Record the number of vertices the buffer can hold.
		m_uiMaxVertices = uiNumVertices;

		m_p3dDriver = CDX3DDriver::GetInstance();

		return hr;
	}


	void CDXSprite::Destroy()
	{
		DX_SAFE_RELEASE(m_pILSprites);
		DX_SAFE_RELEASE(m_pVBSprites);
	}


	void CDXSprite::OnBackBufferResize(float fWidth, float fHeight)
	{
		m_fReciprocalBackBufferWidth = 1.0f / fWidth;
		m_fReciprocalBackBufferHeight = 1.0f / fHeight;
	}


	void CDXSprite::BeginBatchRendering()
	{
		if (!m_bRendering)
		{
			m_uiVerticesInBatch = 0;
			m_pILTemp = m_p3dDriver->GetInputLayout();
			m_pVBTemp = m_p3dDriver->GetVertexBuffer();
			m_uiVBOffsetTemp = m_p3dDriver->GetVertexBufferOffset();

			m_p3dDriver->SetInputLayout(m_pILSprites);
			m_p3dDriver->Map(m_pVBSprites, DX_MAP_WRITE, m_pMappedVB);
			m_bRendering = true;
			m_bVBMapped = true;
		}
	}


	void CDXSprite::EndBatchRendering()
	{
		if (m_bRendering)
		{
			Flush();
			if (m_pILTemp)
			{
				m_p3dDriver->SetInputLayout(m_pILTemp);
			}
			if (m_pILTemp)
			{
				m_p3dDriver->SetVertexBuffer(m_pVBTemp, m_uiVBOffsetTemp);
			}
			m_bRendering = false;
		}
	}


	void CDXSprite::Flush()
	{
		if (m_bRendering && m_bVBMapped)
		{
			m_p3dDriver->Unmap(m_pVBSprites);
			m_bVBMapped = false;
			if (m_uiVerticesInBatch)
			{
				m_p3dDriver->SetVertexBuffer(m_pVBSprites);
				m_p3dDriver->Draw(m_uiVerticesInBatch, 0);
				m_uiVerticesInBatch = 0;
			}
		}
	}


	void CDXSprite::BatchRenderSprite(DXRect* pRect, DXRect* pRectTexture, XMFLOAT4* pColor, float fDepth)
	{
		if (m_uiVerticesInBatch + 6 >= m_uiMaxVertices)
		{
			Flush();
			m_p3dDriver->Map(m_pVBSprites, DX_MAP_WRITE, m_pMappedVB);
			m_bVBMapped = true;
		}
		if (!m_bVBMapped)
		{
			m_p3dDriver->Map(m_pVBSprites, DX_MAP_WRITE, m_pMappedVB);
			m_bVBMapped = true;
		}

		DXSpriteVertex* pVertices = (DXSpriteVertex*)m_pMappedVB->pData;

		float fX1 = pRect->fLeft;
		float fX2 = pRect->fRight;
		float fY1 = pRect->fTop;
		float fY2 = pRect->fBottom;

		// If the coords are given in screen space, then we need to convert them to clip
		// (projection) space.
		if (m_bConvertToClipSpace)
		{
			fX1 = 2.0f * (fX1 * m_fReciprocalBackBufferWidth) - 1;
			fX2 = 2.0f * (fX2 * m_fReciprocalBackBufferWidth) - 1;
			fY1 = 1.0f - 2.0f * (fY1 * m_fReciprocalBackBufferHeight);
			fY2 = 1.0f - 2.0f * (fY2 * m_fReciprocalBackBufferHeight);
		}

		float fTX1 = pRectTexture->fLeft;
		float fTX2 = pRectTexture->fRight;
		float fTY1 = pRectTexture->fTop;
		float fTY2 = pRectTexture->fBottom;

		int v = m_uiVerticesInBatch;

		pVertices[v].f3Pos = XMFLOAT3(fX1, fY1, fDepth);
		pVertices[v].f2Tex = XMFLOAT2(fTX1, fTY1);
		pVertices[v].f4Color = *pColor;

		pVertices[v+1].f3Pos = XMFLOAT3(fX2, fY2, fDepth);
		pVertices[v+1].f2Tex = XMFLOAT2(fTX2, fTY2);
		pVertices[v+1].f4Color = *pColor;

		pVertices[v+2].f3Pos = XMFLOAT3(fX1, fY2, fDepth);
		pVertices[v+2].f2Tex = XMFLOAT2(fTX1, fTY2);
		pVertices[v+2].f4Color = *pColor;

		pVertices[v+3].f3Pos = XMFLOAT3(fX1, fY1, fDepth);
		pVertices[v+3].f2Tex = XMFLOAT2(fTX1, fTY1);
		pVertices[v+3].f4Color = *pColor;

		pVertices[v+4].f3Pos = XMFLOAT3(fX2, fY1, fDepth);
		pVertices[v+4].f2Tex = XMFLOAT2(fTX2, fTY1);
		pVertices[v+4].f4Color = *pColor;

		pVertices[v+5].f3Pos = XMFLOAT3(fX2, fY2, fDepth);
		pVertices[v+5].f2Tex = XMFLOAT2(fTX2, fTY2);
		pVertices[v+5].f4Color = *pColor;

		m_uiVerticesInBatch += 6;
	}

} // DXFramework namespace end