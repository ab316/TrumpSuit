#include "DXStdafx.h"
#include "DXHardwareBuffer.h"
#include "DXHardwareBufferManager.h"


namespace DXFramework
{ // DXFramework namespace begin

	CDXHardwareBufferManager::CDXHardwareBufferManager()
	{
		DX_SET_UNKNOWN_NAME(this, L"HARDWARE BUFFER MANAGER");
	}


	CDXHardwareBufferManager::~CDXHardwareBufferManager()
	{
	}


	HRESULT CDXHardwareBufferManager::Create(void* pNull)
	{
		UNREFERENCED_PARAMETER(pNull);
		return S_OK;
	}


	DXResourceHandle CDXHardwareBufferManager::CreateResource(void* pBufferDesc, WCHAR* szCustomKey)
	{
		DXResourceKey key;
		DXResourceMapInsertResult insertResult;
		DXResourceHandle returnValue;
		DXHardwareBufferDesc* pDesc;
		CDXHardwareBuffer* pHardwareBuffer;

		pDesc = (DXHardwareBufferDesc*)pBufferDesc;
		pHardwareBuffer = nullptr;

		// Custom key is must.
		if (!szCustomKey)
		{
			DX_DEBUG_OUTPUT0(L"ERROR!!! Custom key must be provided.");
			return returnValue;
		}


		DX_HARDWAREBUFFER_TYPE type = pDesc->bufferType;
		WCHAR szType[3];
		WCHAR szKey[DX_MAX_RESOURCE_KEY_NAME];

		switch (type)
		{
		case DX_HARDWAREBUFFER_TYPE_CONSTANTBUFFER:
			wcscpy_s(szType, L"CB");
			break;

		case DX_HARDWAREBUFFER_TYPE_INDEXBUFFER:
			wcscpy_s(szType, L"IB");
			break;

		case DX_HARDWAREBUFFER_TYPE_VERTEXBUFFER:
			wcscpy_s(szType, L"VB");
			break;

		default:
			wcscpy_s(szType, L"??");
			break;
		}

		wsprintf(szKey, L"%s_%s", szType, szCustomKey);

		// Try to insert the resource into the map.
		key = DXResourceKey(szKey);
		insertResult = InsertResource(key, pHardwareBuffer);
		if (insertResult.second) // New element inserted.
		{
			HRESULT hr = S_OK;
			// Allocate memory for new resource and create it.
			pHardwareBuffer = new CDXHardwareBuffer();
			DX_V(pHardwareBuffer->Create(pBufferDesc));
			if (FAILED(hr))
			{
				DX_DEBUG_OUTPUT1(L"ERROR!!! Failed on a resource, KEY: %s.", szCustomKey);
				DX_SAFE_DELETE(pHardwareBuffer);
				return returnValue;
			}

			// Update the pointer of the resource in the map.
			insertResult.first->second = pHardwareBuffer;
			// Increase our memory usage.
			AddMemoryUsage(pHardwareBuffer->GetSize());
		}
		else // Existing found.
		{
			pHardwareBuffer = (CDXHardwareBuffer*)insertResult.first->second;
		}

		pHardwareBuffer->AddRef();
		returnValue.pResource = pHardwareBuffer;
		returnValue.key = key;

		return returnValue;
	}

} // DXFramework namespace end