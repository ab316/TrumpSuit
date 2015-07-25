#include "DXStdafx.h"
#include "DXInputLayout.h"
#include "DX3DDriver.h"

namespace DXFramework
{ // DXFramework namespace begin

	CDXInputLayout::CDXInputLayout() :	m_pInputLayout(nullptr)
	{
	}


	CDXInputLayout::~CDXInputLayout()
	{
	}


	HRESULT CDXInputLayout::Create(void* pVoidDesc)
	{
		HRESULT hr = S_OK;

		DXInputLayoutDesc* pDesc = (DXInputLayoutDesc*)pVoidDesc;
		m_descInput.uiNumElements = pDesc->uiNumElements;
		m_descInput.pVertexShader = pDesc->pVertexShader;

		// Allocate memory for the input layout elements.
		size_t sizeMemory = GetSize();
		m_descInput.pInputElements = (DXInputLayoutElement*)DX_MALLOC(sizeMemory);
		if (!m_descInput.pInputElements)
		{
			DX_DEBUG_OUTPUT0(L"ERROR!!! Out of memory.");
			hr = E_OUTOFMEMORY;
			return hr;
		}

		// Copy the data.
		memcpy_s(m_descInput.pInputElements, sizeMemory, pDesc->pInputElements, sizeMemory);

		m_bCreated = true;

		return hr;
	}


	HRESULT CDXInputLayout::Recreate()
	{
		HRESULT hr = S_OK;

		if (m_bDisposed && m_bCreated)
		{
			DX_V_RETURN(CDX3DDriver::GetInstance()->CreateInputLayout(&m_descInput, &m_pInputLayout));
			m_bDisposed = false;
		}

		return hr;
	}


	void CDXInputLayout::Destroy()
	{
		Dispose();
		DX_FREE(m_descInput.pInputElements);
		m_bCreated = false;
	}


	void CDXInputLayout::Dispose()
	{
		DX_SAFE_RELEASE(m_pInputLayout);
		m_bDisposed = true;
	}


	void CDXInputLayout::SetDebugName(char* szName)
	{
		WCHAR name[DX_MAX_NAME];
		MultiByteToWideChar(CP_ACP, 0, szName, -1, name, DX_MAX_NAME);
		try
		{
			SetName(name);
			DX_SET_D3D_DEBUG_NAME(m_pInputLayout, szName);
		}
		catch (...)
		{
			DX_DEBUG_OUTPUT1(L"Exception raised in %s.", name);
		}
	}

} // DXFramework namespace end