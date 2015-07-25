#include "stdafx.h"
#include "DX3DDriver.h"
#include "DXGraphicsDriver.h"
#include "DXHardwareBuffer.h"
#include "DXInputLayout.h"
#include "DXTexture.h"
#include "DXShader.h"
#include "CardMesh.h"

using namespace DXFramework;


CCardMesh::CCardMesh()	:	m_pIndexBuffer(nullptr),
							m_pVertexBuffer(nullptr),
							m_pInputLayout(nullptr),
							m_pVertexShader(nullptr),
							m_pPixelShader(nullptr),
							m_bBackSide(false),
							m_uiTextureId(0)
{
	m_ConstantBuffer.f4Color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
}


CCardMesh::~CCardMesh()
{
}


HRESULT CCardMesh::Create()
{
	HRESULT hr = S_OK;

	CDXGraphicsDriver* pGraphics = CDXGraphicsDriver::GetInstance();

	DXShaderDesc descShader;
	descShader.type = DX_SHADER_VERTEX_SHADER;
	wcscpy_s(descShader.szEntryPoint, DX_MAX_SHADER_ENTRY, L"main");
	wcscpy_s(descShader.szFileName, DX_MAX_FILE_NAME, L"CardVS.hlsl");
	wcscpy_s(descShader.szShaderLevel, DX_MAX_SHADER_PROFILE, L"vs_4_1");
	
	DX_V_RETURN(pGraphics->CreateAndRecreateResource(DX_RESOURCE_TYPE_SHADER, &descShader, (IDXResource**)&m_pVertexShader));
	
	descShader.type = DX_SHADER_PIXEL_SHADER;
	wcscpy_s(descShader.szFileName, DX_MAX_FILE_NAME, L"CardPS.hlsl");
	wcscpy_s(descShader.szShaderLevel, DX_MAX_SHADER_PROFILE, L"ps_4_1");
	DX_V_RETURN(pGraphics->CreateAndRecreateResource(DX_RESOURCE_TYPE_SHADER, &descShader, (IDXResource**)&m_pPixelShader));


	DXInputLayoutDesc inputDesc;
	DXInputLayoutElement elements[] = {
		{ DX_FORMAT_R32G32B32_FLOAT, L"POSITION", 0, 0 },
		{ DX_FORMAT_R32G32B32_FLOAT, L"NORMAL", 0, 0 },
		{ DX_FORMAT_R32G32_FLOAT, L"TEXCOORD", 0, 0 },
	};

	inputDesc.pInputElements = elements;
	inputDesc.uiNumElements = 3;
	inputDesc.pVertexShader = m_pVertexShader;
	DX_V_RETURN(pGraphics->CreateAndRecreateResource(DX_RESOURCE_TYPE_INPUT_LAYOUT, &inputDesc, (IDXResource**)&m_pInputLayout));
	

	CardVertex vertices[] = {
		{ XMFLOAT3(-0.50f, -0.70f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(0.50f, -0.70f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(0.50f, 0.70f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(-0.50f, 0.70f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) }
	};

	UINT16 indices[] = {2, 1, 0, 0, 3, 2 };


	DXHardwareBufferDesc desc;
	desc.bufferType = DX_HARDWAREBUFFER_TYPE_VERTEXBUFFER;
	desc.gpuUsage = DX_GPU_READ_ONLY;
	desc.pBufferData = vertices;
	desc.uiBufferSize = sizeof(vertices);
	desc.uiBufferStride = sizeof(CardVertex);
	DX_V_RETURN(pGraphics->CreateAndRecreateResource(DX_RESOURCE_TYPE_HARDWARE_BUFFER, &desc, (IDXResource**)&m_pVertexBuffer, L"Card"));

	desc.bufferType = DX_HARDWAREBUFFER_TYPE_INDEXBUFFER;
	desc.pBufferData = indices;
	desc.uiBufferSize = sizeof(indices);
	DX_V_RETURN(pGraphics->CreateAndRecreateResource(DX_RESOURCE_TYPE_HARDWARE_BUFFER, &desc, (IDXResource**)&m_pIndexBuffer, L"Card"));


	desc.bufferType = DX_HARDWAREBUFFER_TYPE_CONSTANTBUFFER;
	desc.gpuUsage = DX_GPU_READ_WRITE_ONLY;
	desc.uiBufferSize = sizeof(CardConstantBuffer);
	DX_V_RETURN(pGraphics->CreateAndRecreateResource(DX_RESOURCE_TYPE_HARDWARE_BUFFER, &desc, (IDXResource**)&m_pCBTransforms, L"Card"));


	// Textures.

	// Order matters here.
	WCHAR szSuite[4][2] = { L"s", L"c", L"d", L"h" };

	DXTexture2DDesc descTex;
	descTex.bFileTexture = true;
	descTex.bindFlags = DX_BIND_SHADER_RESOURCE;
	descTex.gpuUsage = DX_GPU_READ_ONLY;

	for (int suit=0; suit<4; suit++)
	{
		for (int i=0; i<13; i++)
		{
			WCHAR szNum[3];
			wsprintf(szNum, L"%u", i+2);
			wsprintf(descTex.textureData.szFileName, L"%s%s.png", szSuite[suit], szNum);

			UINT id = (suit * 13) + i;
			DX_V_RETURN(pGraphics->CreateAndRecreateResource(DX_RESOURCE_TYPE_TEXTURE, &descTex, (IDXResource**)&m_pTextures[id]));
		}
	}

	wcscpy_s(descTex.textureData.szFileName, L"CardBack.png");
	DX_V_RETURN(pGraphics->CreateAndRecreateResource(DX_RESOURCE_TYPE_TEXTURE, &descTex, (IDXResource**)&m_pTextures[52]));

	return hr;
}


void CCardMesh::Destroy()
{
	for (int i=0; i<53; ++i)
	{
		DX_SAFE_RELEASE(m_pTextures[i]);
	}

	DX_SAFE_RELEASE(m_pInputLayout);
	DX_SAFE_RELEASE(m_pIndexBuffer);
	DX_SAFE_RELEASE(m_pVertexBuffer);
	DX_SAFE_RELEASE(m_pVertexShader);
	DX_SAFE_RELEASE(m_pCBTransforms);
	DX_SAFE_RELEASE(m_pPixelShader);
}


void CCardMesh::SetHighlight(bool b)
{
	if (b)
	{
		m_ConstantBuffer.f4Color = XMFLOAT4(1.0f, 0.3f, 1.0f, 1.0f);
	}
	else
	{
		m_ConstantBuffer.f4Color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	}
}


void CCardMesh::UpdateTransform()
{
	CDXGraphicsDriver::GetInstance()->Get3DDriver()->UpdateConstantBuffer(m_pCBTransforms, &m_ConstantBuffer);
}


void CCardMesh::Render()
{
	static CDX3DDriver* p3d = CDXGraphicsDriver::GetInstance()->Get3DDriver();

	p3d->SetBlendState(0, 0);
	p3d->SetDepthStencilState(0, 0);
	p3d->SetRasterizerState(0);
	p3d->SetOutputs(DX_DEFAULT_RENDER_TARGET, DX_DEFAULT_RENDER_TARGET);
	p3d->SetVertexBuffer(m_pVertexBuffer);
	p3d->SetIndexBuffer(m_pIndexBuffer);
	p3d->SetInputLayout(m_pInputLayout);
	p3d->SetVertexShader(m_pVertexShader);
	p3d->SetPixelShader(m_pPixelShader);
	p3d->SetVSConstantBuffer(0, m_pCBTransforms);
	if (m_bBackSide)
	{
		p3d->SetPSTextures(0, 1, &m_pTextures[52]);
	}
	else
	{
		p3d->SetPSTextures(0, 1, &m_pTextures[m_uiTextureId]);
	}
	p3d->DrawIndexed(6);
}


void CCardMesh::SetTexture(CARD_SUIT suit, CARD_VALUE value)
{
	m_uiTextureId = ((UINT)suit * 13) + (UINT)value;
}