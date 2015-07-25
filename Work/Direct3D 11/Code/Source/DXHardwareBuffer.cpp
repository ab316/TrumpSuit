#include "DXStdafx.h"
#include "DX3DDriver.h"
#include "DXHardwareBuffer.h"

namespace DXFramework
{ // DXFramework namespace begin

	CDXHardwareBuffer::CDXHardwareBuffer() :	m_pD3dBuffer(nullptr)
	{
	}


	CDXHardwareBuffer::~CDXHardwareBuffer()
	{
	}


	HRESULT CDXHardwareBuffer::Create(void* pBufferDesc)
	{
		HRESULT hr = S_OK;

		// Copy the description data.
		DXHardwareBufferDesc* pDesc = (DXHardwareBufferDesc*)pBufferDesc;
		m_descBuffer.bufferType = pDesc->bufferType;
		m_descBuffer.uiBufferSize = pDesc->uiBufferSize;
		m_descBuffer.uiBufferStride = pDesc->uiBufferStride;
		m_descBuffer.gpuUsage = pDesc->gpuUsage;
		m_descBuffer.pBufferData = nullptr;

		if (m_descBuffer.bufferType != DX_HARDWAREBUFFER_TYPE_CONSTANTBUFFER)
		{
			size_t bufferSize = GetSize();
			if (!pDesc->pBufferData && (m_descBuffer.gpuUsage == DX_GPU_READ_ONLY || m_descBuffer.gpuUsage == DX_GPU_READ_WRITE_ONLY))
			{
				hr = E_INVALIDARG;
				DX_DEBUG_OUTPUT0(L"ERROR!!! No buffer data provided and GPU usage is not one of CPU_WRITE.");
				return hr;
			}
			m_descBuffer.pBufferData = DX_MALLOC(bufferSize);
			if (!m_descBuffer.pBufferData)
			{
				hr = E_OUTOFMEMORY;
				DX_DEBUG_OUTPUT0(L"ERROR!!! Out of memory.");
				return hr;
			}
			if (pDesc->pBufferData) // If initial data is provided.
			{
				// Copy the data into our heap memory.
				memcpy_s(m_descBuffer.pBufferData, bufferSize, pDesc->pBufferData, bufferSize);
			}
		}

		m_bCreated = true;

		return hr;
	}


	HRESULT CDXHardwareBuffer::Recreate()
	{
		HRESULT hr = S_OK;

		if (m_bDisposed && m_bCreated)
		{
			DX_V_RETURN(CDX3DDriver::GetInstance()->CreateBuffer(&m_descBuffer, &m_pD3dBuffer));
			m_bDisposed = false;
		}

		return hr;
	}


	void CDXHardwareBuffer::Destroy()
	{
		Dispose();
		DX_FREE(m_descBuffer.pBufferData);
		ZeroMemory(&m_descBuffer, sizeof(m_descBuffer));
		m_bCreated = false;
	}


	void CDXHardwareBuffer::Dispose()
	{
		DX_SAFE_RELEASE(m_pD3dBuffer);
		m_bDisposed = true;
	}


	void CDXHardwareBuffer::SetDebugName(char* szName)
	{
		WCHAR name[DX_MAX_NAME];
		MultiByteToWideChar(CP_ACP, 0, szName, -1, name, DX_MAX_NAME);
		try
		{
			SetName(name);
			DX_SET_D3D_DEBUG_NAME(m_pD3dBuffer, szName);
		}
		catch (...)
		{
			DX_DEBUG_OUTPUT1(L"Exception raised in %s.", name);
		}
	}
} // DXFramework namespace end