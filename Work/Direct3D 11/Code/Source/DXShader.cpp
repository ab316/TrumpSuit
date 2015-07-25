#include "DXStdafx.h"
#include "DXShader.h"
#include "DX3DDriver.h"
#include "DXFileSystem.h"


namespace DXFramework
{ // DXFramework namespace begin

	CDXShader::CDXShader() :	m_pShader(nullptr),
								m_pShaderBlob(nullptr)
	{
		ZeroMemory(&m_desc, sizeof(m_desc));
	}


	CDXShader::~CDXShader()
	{
	}


	HRESULT CDXShader::Create(void* pDesc)
	{
		HRESULT hr = S_OK;

		DXShaderDesc* pInShaderDesc = (DXShaderDesc*)pDesc;
		m_desc.type = pInShaderDesc->type;

		wcscpy_s(m_desc.szFileName, DX_MAX_FILE_NAME, pInShaderDesc->szFileName);
		wcscpy_s(m_desc.szEntryPoint, DX_MAX_SHADER_ENTRY, pInShaderDesc->szEntryPoint);
		wcscpy_s(m_desc.szShaderLevel, DX_MAX_SHADER_PROFILE, pInShaderDesc->szShaderLevel);

		CDXFileSystem* pFileSystem = CDXFileSystem::GetInstance();
		
		DX_V(pFileSystem->ReadFile(m_desc.szFileName, &m_desc.pBuffer, &m_desc.uiBufferSize));
		if (FAILED(hr))
		{
			DX_DEBUG_OUTPUT1(L"ERROR!!! Failed, unable to read file: %s.", m_desc.szFileName);
			Destroy();
			return hr;
		}
		pFileSystem->DecreaseFileUsage(m_desc.szFileName);

		CDX3DDriver* p3dDriver = CDX3DDriver::GetInstance();
		DX_V(p3dDriver->CompileShader(&m_desc, &m_pShaderBlob));
		if (FAILED(hr))
		{
			DX_DEBUG_OUTPUT3(L"ERROR!!! Failed, unable to compile shader FILE: %s, ENTRY: %S, PROFILE: %s.", m_desc.szFileName, m_desc.szEntryPoint, m_desc.szShaderLevel);
			Destroy();
			return hr;
		}

		m_desc.uiBufferSize = m_pShaderBlob->GetBufferSize();

		m_bCreated = true;

		return hr;
	}


	HRESULT CDXShader::Recreate()
	{
		HRESULT hr = S_OK;

		if (m_bDisposed && m_bCreated)
		{
			CDX3DDriver* p3dDriver = CDX3DDriver::GetInstance();
			DX_V(p3dDriver->CreateShader(m_desc.type, m_pShaderBlob, &m_pShader));
			if (FAILED(hr))
			{
				DX_DEBUG_OUTPUT3(L"ERROR!!! Failed, unable to compile shader FILE: %s, ENTRY: %S, PROFILE: %s.", m_desc.szFileName, m_desc.szEntryPoint, m_desc.szShaderLevel);
				return hr;
			}

			m_bDisposed = false;
		}

		return hr;
	}


	void CDXShader::Destroy()
	{
		Dispose();
		DX_SAFE_RELEASE(m_pShaderBlob);
		ZeroMemory(&m_desc, sizeof(m_desc));
		m_bCreated = false;
	}


	void CDXShader::Dispose()
	{
		IUnknown* p = (IUnknown*)m_pShader;
		DX_SAFE_RELEASE(p);

		m_bDisposed = true;
	}


	void CDXShader::SetDebugName(char* szName)
	{
		WCHAR name[DX_MAX_NAME];
		MultiByteToWideChar(CP_ACP, 0, szName, -1, name, DX_MAX_NAME);
		try
		{
			SetName(name);
			ID3D11DeviceChild* p = (ID3D11DeviceChild*)m_pShader;
			DX_SET_D3D_DEBUG_NAME(p, szName);
		}
		catch (...)
		{
			DX_DEBUG_OUTPUT1(L"Exception raised in %s.", name);
		}
	}


	HRESULT _stdcall IDXShaderIncludeHandler::Open(D3D10_INCLUDE_TYPE includeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID* ppData, UINT* pBytes)
	{
		UNREFERENCED_PARAMETER(pParentData);
		UNREFERENCED_PARAMETER(includeType);

		HRESULT hr = S_OK;

		CDXFileSystem* pFileSystem = CDXFileSystem::GetInstance();

		MultiByteToWideChar(CP_ACP, 0, pFileName, -1, m_szFile, DX_MAX_FILE_PATH);

		void* pBufferData = nullptr;
		DX_V_RETURN(pFileSystem->ReadFile(m_szFile, &pBufferData, (size_t*)pBytes));
		*ppData = pBufferData;

		return hr;
	}


	HRESULT _stdcall IDXShaderIncludeHandler:: Close(LPCVOID pData)
	{
		UNREFERENCED_PARAMETER(pData);
		CDXFileSystem::GetInstance()->DecreaseFileUsage(m_szFile);
		return S_OK;
	}

} // DXFramework namespace end