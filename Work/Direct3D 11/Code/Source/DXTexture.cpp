#include "DXStdafx.h"
#include "DXTexture.h"
#include "DXFileSystem.h"
#include "DX3DDriver.h"


namespace DXFramework
{ // DXFramework namespace begin

	CDXTexture::CDXTexture() :		m_pTexture2d(nullptr),
									m_pShaderResourceView(nullptr),
									m_pRenderTargetView(nullptr),
									m_pDepthStencilView(nullptr)
	{
	}


	CDXTexture::~CDXTexture()
	{
	}


	HRESULT CDXTexture::Create(void* pDesc)
	{
		HRESULT hr = S_OK;

		DXTexture2DDesc* pDescTexture = (DXTexture2DDesc*)pDesc;

		// Each member is individually set because the file name member
		// is a pointer and equating the 2 structures would change it and, hence,
		// we would lose the pointer to the WCHAR array of our structure.
		m_descTexture.bFileTexture = pDescTexture->bFileTexture;
		m_descTexture.bindFlags = pDescTexture->bindFlags;
		m_descTexture.uiWidth = pDescTexture->uiWidth;
		m_descTexture.uiHeight = pDescTexture->uiHeight;
		m_descTexture.uiMipLevels = pDescTexture->uiMipLevels;
		m_descTexture.textureFormat = pDescTexture->textureFormat;
		m_descTexture.uiMultisampleLevel = pDescTexture->uiMultisampleLevel;
		m_descTexture.gpuUsage = pDescTexture->gpuUsage;
		m_descTexture.textureData.uiPitchOrSize = pDescTexture->textureData.uiPitchOrSize;
		m_descTexture.textureData.pData = pDescTexture->textureData.pData;
		m_descTexture.textureData.szFileName[0] = 0;

		// If it is a file texture, search for the file and puts its path in the member description
		// and read the file into the heap memory. For a file texture the texture memory is
		// managed by the file system.
		if (m_descTexture.bFileTexture)
		{
			WCHAR szFilePath[DX_MAX_FILE_PATH];
			CDXFileSystem* pFileSystem = CDXFileSystem::GetInstance();
			bool bFound = pFileSystem->FindFile(pDescTexture->textureData.szFileName, szFilePath);
			if (!bFound)
			{
				DX_DEBUG_OUTPUT1(L"ERROR!!! File not found: %s.", pDescTexture->textureData.szFileName);
				hr = E_FAIL;
				return hr;
			}
			wcscpy_s(m_descTexture.textureData.szFileName, DX_MAX_FILE_NAME, pFileSystem->GetFileName(szFilePath));
			pFileSystem->ReadFile(m_descTexture.textureData.szFileName, &m_descTexture.textureData.pData, &m_descTexture.textureData.uiPitchOrSize);
		}
		else
		{
			// If we are supplied with initial texture data. Then memory is allocated
			// here to store the texture data.
			if (m_descTexture.textureData.pData)
			{
				// Acquire the size of texture data.
				size_t sizeMemory = GetSize();
				// Allocate memory for the data on the heap.
				m_descTexture.textureData.pData = DX_MALLOC(sizeMemory);
				if (!m_descTexture.textureData.pData)
				{
					hr = E_OUTOFMEMORY;
					DX_DEBUG_OUTPUT0(L"ERROR!!! Out of memory.");
					return hr;
				}
				// Copy the data.
				memcpy_s(m_descTexture.textureData.pData, sizeMemory, pDescTexture->textureData.pData, sizeMemory);
			}
		}

		m_bCreated = true;

		return hr;
	}


	HRESULT CDXTexture::Recreate()
	{
		HRESULT hr = S_OK;

		if (m_bDisposed && m_bCreated)
		{
			CDX3DDriver* p3dDriver = CDX3DDriver::GetInstance();
			DX_V_RETURN(p3dDriver->CreateTexture2D(&m_descTexture, &m_pTexture2d));
			DX_V_RETURN(p3dDriver->CreateResourceViews(&m_descTexture, m_pTexture2d, &m_pShaderResourceView, &m_pRenderTargetView, &m_pDepthStencilView));

			m_bDisposed = false;
		}

		return hr;
	}


	void CDXTexture::Destroy()
	{
		Dispose();
		// For a file texture. The file data is managed by the file system.
		if (m_descTexture.bFileTexture)
		{
			CDXFileSystem* pFileSystem = CDXFileSystem::GetInstance();
			pFileSystem->DecreaseFileUsage(m_descTexture.textureData.szFileName);
		}
		else
		{
			DX_FREE(m_descTexture.textureData.pData);
		}
		ZeroMemory(&m_descTexture, sizeof(m_descTexture));
		m_bCreated = false;
	}


	void CDXTexture::Dispose()
	{
		DX_SAFE_RELEASE(m_pTexture2d);
		DX_SAFE_RELEASE(m_pDepthStencilView);
		DX_SAFE_RELEASE(m_pRenderTargetView);
		DX_SAFE_RELEASE(m_pShaderResourceView);
		m_bDisposed = true;
	}


	size_t CDXTexture::GetSize() const
	{
		if (m_descTexture.bFileTexture)
		{
			return m_descTexture.textureData.uiPitchOrSize;
		}
		else
		{
			size_t pixelSize = DX_GET_FORMAT_SIZE(m_descTexture.textureFormat);
			// Simple size calculation. Not taking MIP maps into account.
			return (size_t)(m_descTexture.uiWidth * m_descTexture.uiHeight * pixelSize);
		}
	}

	
	void CDXTexture::SetDebugName(char* szName)
	{
		WCHAR name[DX_MAX_FILE_NAME];
		MultiByteToWideChar(CP_ACP, 0, szName, -1, name, DX_MAX_FILE_NAME);
		try
		{
			SetName(name);
			DX_SET_D3D_DEBUG_NAME(m_pTexture2d, szName);

			if (m_pDepthStencilView)
			{
				DX_SET_D3D_DEBUG_NAME(m_pDepthStencilView, szName);
			}
			if (m_pRenderTargetView)
			{
				DX_SET_D3D_DEBUG_NAME(m_pRenderTargetView, szName);
			}
			if (m_pShaderResourceView)
			{
				DX_SET_D3D_DEBUG_NAME(m_pShaderResourceView, szName);
			}
		}
		catch (...)
		{
			DX_DEBUG_OUTPUT1(L"Exception raised in %s.", name);
		}
	}

} // DXFramework namespace end