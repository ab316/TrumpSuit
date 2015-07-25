#include "DXStdafx.h"
#include "DXTexture.h"
#include "DXTextureManager.h"

namespace DXFramework
{ // DXFramework namespace begin

	CDXTextureManager::CDXTextureManager()
	{
		DX_SET_UNKNOWN_NAME(this, L"TEXTURE MANAGER");
	}


	CDXTextureManager::~CDXTextureManager()
	{
	}


	HRESULT CDXTextureManager::Create(void* pNull)
	{
		HRESULT hr = S_OK;

		UNREFERENCED_PARAMETER(pNull);

		return hr;
	}


	DXResourceHandle CDXTextureManager::CreateResource(void* pTex2DDesc, WCHAR* szCustomKey)
	{
		// Local storage for the processed key.
		DXResourceKey key;
		// Result of the attempt to insert the resource with the specified key
		// into the resource map.
		DXResourceMapInsertResult insertResult;
		// Output of the method. Sets an invalid return value first.
		DXResourceHandle returnValue;
		// Pointer to the texture.
		CDXTexture* pTexture = nullptr;
		//  The user supplied texture description.
		DXTexture2DDesc* pDesc = (DXTexture2DDesc*)pTex2DDesc;


		// First we process the key for the resource.

		// If it is a file texture.
		if (pDesc->bFileTexture)
		{
			// If a custom key is provided. Well and good.
			if (szCustomKey)
			{
				wcscpy_s(key.m_szName, DX_MAX_RESOURCE_KEY_NAME, szCustomKey);
			}
			else
			{
				// Else use the file name as the key. NOTE: The file
				// name and extension must not be more than (DX_MAX_FILE_NAME - 1)
				WCHAR* p = wcsrchr(pDesc->textureData.szFileName, '\\');
				if (p)
				{
					++p;
					wcscpy_s(key.m_szName, DX_MAX_RESOURCE_KEY_NAME, p);
				}
				else
				{
					wcscpy_s(key.m_szName, DX_MAX_RESOURCE_KEY_NAME, pDesc->textureData.szFileName);
				}
				// Remove the extension as well.
				p = wcsrchr(key.m_szName, '.');
				if (p)
				{
					*p = 0;
				}
			}
		}
		// Not a file texture?
		else
		{
			// Then a custom key must have been provided.
			if (szCustomKey)
			{
				// Yes.
				// Copy the key provided to our local var.
				wcscpy_s(key.m_szName, DX_MAX_RESOURCE_KEY_NAME, szCustomKey);
			}
			else // No
			{
				// Return a typical invalid return value.
				DX_DEBUG_OUTPUT0(L"Non file textures must be supplied with a custom key.");
				return returnValue;
			}
		}

		// Finished processing the key. Now to handle the actual
		// insertion of resource in the map and its creation on the heap.

		// Try to insert the resource into the map.
		// insertResult <iterator, bool>
		insertResult = InsertResource(key.m_szName, pTexture);

		// If a new element was inserted into the map.
		if (insertResult.second)
		{
			HRESULT hr = S_OK;
			// Allocate memory for the new texture on the heap.
			pTexture = new CDXTexture();
			// Create it.
			DX_V(pTexture->Create((void*)pDesc));
			if (FAILED(hr))
			{
				DX_DEBUG_OUTPUT1(L"Failed on a resource, KEY: %s.", key.m_szName);
				DX_SAFE_DELETE(pTexture);
				return returnValue;
			}

			// Update the return value and the pointer to the new texture memory in the map. 
			insertResult.first->second = pTexture;
			// Increase the memory usage.
			AddMemoryUsage(pTexture->GetSize());
		}
		else // Existing element was returned.
		{
			pTexture = (CDXTexture*)insertResult.first->second;
		}

		pTexture->AddRef();
		returnValue.key = key;
		returnValue.pResource = pTexture;
		// returnValue : <DXResourceKey, IDXResource*>
		return returnValue;
	}

} // DXFramework namespace end