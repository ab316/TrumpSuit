#include "DXStdafx.h"
#include "DXInputLayout.h"
#include "DXInputLayoutManager.h"


namespace DXFramework
{ // DXFramework namespace begin

	CDXInputLayoutManager::CDXInputLayoutManager()
	{
		DX_SET_UNKNOWN_NAME(this, L"INPUT LAYOUT MANAGER");
	}

	CDXInputLayoutManager::~CDXInputLayoutManager()
	{
	}


	HRESULT CDXInputLayoutManager::Create(void* pNull)
	{
		UNREFERENCED_PARAMETER(pNull);
		return S_OK;
	}


	DXResourceHandle CDXInputLayoutManager::CreateResource(void* pLayoutDesc, WCHAR* szCustomKey)
	{
		DXResourceKey key;
		DXResourceMapInsertResult insertResult;
		DXResourceHandle returnValue;
		DXInputLayoutDesc* pDesc;
		CDXInputLayout* pLayout;

		pDesc = (DXInputLayoutDesc*)pLayoutDesc;
		pLayout = nullptr;

		// Custom key is must.
		if (szCustomKey)
		{
			// Copy the key name.
			wcscpy_s(key.m_szName, DX_MAX_RESOURCE_KEY_NAME, szCustomKey);
			DX_DEBUG_OUTPUT1(L"WARNING!!! Custom key provided: %s, Not a good practice.", szCustomKey);
		}
		else
		{
			key.m_szName[0] = 0;
			if (pDesc->uiNumElements > 8)
			{
				DX_DEBUG_OUTPUT0(L"ERROR!!! Number of elements > 8 and custom key not provided. Automatic key preparation can't be done with more than 8 elements.");
				return returnValue;
			}

			for (int i=0; i<pDesc->uiNumElements; i++)
			{
				WCHAR szKeyNamePart[20];
				WCHAR szFormat[3];
				DXInputLayoutElement* pElement = &pDesc->pInputElements[i];

				switch (pElement->format)
				{
				case DX_FORMAT_R32G32B32A32_FLOAT:
					wcscpy_s(szFormat, 3, L"F4");
					break;

				case DX_FORMAT_R32G32B32A32_UINT:
					wcscpy_s(szFormat, 3, L"U4");
					break;

				case DX_FORMAT_R32G32B32_FLOAT:
					wcscpy_s(szFormat, 3, L"F3");
					break;

				case DX_FORMAT_R32G32B32_UINT:
					wcscpy_s(szFormat, 3, L"U3");
					break;

				case DX_FORMAT_R32G32_FLOAT:
					wcscpy_s(szFormat, 3, L"F2");
					break;

				case DX_FORMAT_R32G32_UINT:
					wcscpy_s(szFormat, 3, L"U2");
					break;

				case DX_FORMAT_R32_FLOAT:
					wcscpy_s(szFormat, 3, L"F1");
					break;

				case DX_FORMAT_R32_UINT:
					wcscpy_s(szFormat, 3, L"U1");
					break;
				}

				swprintf_s(szKeyNamePart, 20, L"(%.4s_%.2s_%u)", pElement->szName, szFormat, pElement->uiIndex);
				wcscat_s(key.m_szName, DX_MAX_RESOURCE_KEY_NAME, szKeyNamePart);
			}
		}

		// Attempt to insert the resource into the map.
		insertResult = InsertResource(key, pLayout);

		if (insertResult.second) // New element inserted?
		{
			HRESULT hr = S_OK;
			// Allocate memory for the new member.
			pLayout = new CDXInputLayout();
			// Create the resource.
			DX_V(pLayout->Create(pLayoutDesc));
			if (FAILED(hr)) // Failed?
			{
				DX_DEBUG_OUTPUT1(L"Failed, KEY: %s.", szCustomKey);
				DX_SAFE_DELETE(pLayout);
				return returnValue;
			}
			// Succeeded!
			insertResult.first->second = pLayout;
			AddMemoryUsage(pLayout->GetSize());
		}
		else // Resource already exists in the map.
		{
			pLayout = (CDXInputLayout*)insertResult.first->second;
		}

		pLayout->AddRef();
		returnValue.key = key;
		returnValue.pResource = pLayout;

		return returnValue;
	}

} // DXFramework namespace end