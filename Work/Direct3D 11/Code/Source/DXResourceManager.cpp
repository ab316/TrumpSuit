#include "DXStdafx.h"
#include "DXResource.h"
#include "DXTexture.h"
#include "DXResourceManager.h"

namespace DXFramework
{ // DXFramework namespace begin

	IDXResourceManager::IDXResourceManager() :	m_uiCurrentMemoryUsage(0)
	{
	}


	IDXResourceManager::~IDXResourceManager()
	{
		Destroy();
	}


	HRESULT IDXResourceManager::RecreateResource(DXResourceKey key)
	{
		IDXResource* pResource = GetResource(key);
		if (!pResource)
		{
			DX_DEBUG_OUTPUT2(L"\nWARNING!!!: DXResourceManager NAME: %s. Called on an invalid resource. Key: %s.", GetName(), key.m_szName);
			return E_FAIL;
		}
		if (pResource->IsCreated() && pResource->IsDisposed())
		{
			if (pResource->ms_uiMagicNumber != DX_RESOURCE_MAGIC_NUMBER)
			{
				DX_DEBUG_OUTPUT2(L"WARNING!!! DXResourceManager NAME: %s. Called on an invalid resource. Key: %s.", GetName(), key.m_szName);
				return E_INVALIDARG;
			}
			return pResource->Recreate();
		}

		return S_OK;
	}


	HRESULT IDXResourceManager::RecreateResource(IDXResource* pResource)
	{
		HRESULT hr = S_OK;

		GotoBegin();
		while (IsIterValid())
		{
			if (GetCurrentResource() == pResource)
			{
				RecreateResource(m_mapIterator->first);
			}
			GotoNext();
		}

		return hr;
	}


	HRESULT IDXResourceManager::RecreateResource(DXResourceHandle handle)
	{
		return RecreateResource(handle.key);
	}


	IDXResource* IDXResourceManager::GetResource(DXResourceKey key)
	{
		DXResourceMapIterator iter = m_mapResources.find(key);
		if (iter != m_mapResources.end())
		{
			return iter->second;
		}
		return (IDXResource*)nullptr;
	}


	void IDXResourceManager::Destroy()
	{
		GotoBegin();
		while (IsIterValid())
		{
			DestroyResource(m_mapIterator->first);
			GotoBegin();
		}

		if (m_uiCurrentMemoryUsage)
		{
			DX_DEBUG_OUTPUT2(L"WARNING!!! DXResourceManager NAME: %s, memory usage %d bytes.", GetName(), m_uiCurrentMemoryUsage);
		}
	}


	void IDXResourceManager::DestroyResource(IDXResource* pResource)
	{
		GotoBegin();
		while (IsIterValid())
		{
			if (m_mapIterator->second == pResource)
			{
				DestroyResource(m_mapIterator);
				return;
			}
			GotoNext();
		}
	}


	void IDXResourceManager::DestroyResource(DXResourceKey key)
	{
		DXResourceMapIterator iter = m_mapResources.find(key);
		if (iter != m_mapResources.end())
		{
			DestroyResource(iter);
		}
	}


	void IDXResourceManager::DestroyResource(DXResourceMapIterator iter)
	{
		if (iter != m_mapResources.end())
		{
			IDXResource* pResource = iter->second;
			if (pResource->ms_uiMagicNumber != DX_RESOURCE_MAGIC_NUMBER || !pResource)
			{
				DX_DEBUG_OUTPUT1(L"WARNING!!! Called on an invalid resource, KEY: %s.", iter->first.m_szName);
			}
			else
			{
				if (pResource->GetRefCount())
				{
					DX_DEBUG_OUTPUT4(L"WARNING!!! Called on a resource with non-zero usage: TYPE: %s, KEY: %s, NAME: %s, USAGE: %d.", pResource->GetResourceTypeName(), m_mapIterator->first.m_szName, pResource->GetName(), pResource->GetRefCount());
				}
				// Otherwise destroy the resource.
				DecreaseMemoryUsage(pResource->GetSize());
				pResource->Destroy();

				// Delete the resource. Currently the CreateResource() method
				// is expected to allocate resource on the heap using new.
				DX_SAFE_DELETE(pResource);
			}

			// Remove the resource from the map.
			m_mapResources.erase(iter);
		}
	}


	DXResourceMapInsertResult IDXResourceManager::InsertResource(DXResourceKey key, IDXResource* pResource)
	{
		DXResourceMapInsertResult insertResult;

		// Try to insert the resource into the map.
		insertResult = m_mapResources.insert(DXResourceHandleInternal(key, pResource));

		// Resource already exists?
		if (!insertResult.second)
		{
			DX_DEBUG_OUTPUT2(L"Resource already present (return existing one) KEY: %s, TYPE: %s.", key.m_szName, insertResult.first->second->GetResourceTypeName());
		}

		return insertResult;
	}


	void IDXResourceManager::RemoveResource(IDXResource* pResource)
	{
		GotoBegin();
		while (IsIterValid())
		{
			if (GetCurrentResource() == pResource)
			{
				// Remove the resource. NOTE the resource is not deleted
				// from the memory, only from the map.
				m_mapResources.erase(m_mapIterator);
				break;
			}
			GotoNext();
		}
	}


	void IDXResourceManager::RemoveResource(DXResourceKey key)
	{
		// Find the resource.
		DXResourceMapIterator iter = m_mapResources.find(key);
		// If it exists.
		if (iter != m_mapResources.end())
		{
			// Remove the resource. NOTE the resource is not deleted
			// from the memory, only from the map.
			m_mapResources.erase(key);
		}
	}

} // DXFramework namespace end