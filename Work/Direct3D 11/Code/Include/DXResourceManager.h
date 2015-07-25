#pragma once

#include "DXUnknown.h"

namespace DXFramework
{ // DXFramework namespace begin

#define DX_INVALID_RESOURCE_HANDLE DXResourceHandle::GetInvalidResourceHandle()

#define DX_MAX_RESOURCE_KEY_NAME 100
	// A key class for use with a map. No 2 keys in a map may be equal.
	class DXStringMapKey
	{
	public:
		WCHAR	m_szName[DX_MAX_RESOURCE_KEY_NAME+1];

	public:
		DXStringMapKey(LPCWSTR szName=nullptr)
		{
			if (szName)
			{
				wcscpy_s(m_szName, DX_MAX_RESOURCE_KEY_NAME, szName);
			}
			else
			{
				*m_szName = 0;
			}
		}


		~DXStringMapKey()
		{
		}
	};
	
	// Some way to give the comparison function to the map.
	struct DXStringMapKeyCompareFunc
	{
		// This function compares two keys. It should return true if left precedes
		// or is not equal to right.
		bool operator()(const DXStringMapKey& left, const DXStringMapKey& right) const
		{
			int a = _wcsicmp(left.m_szName, right.m_szName);
			// see doc of _wcsicmp.
			return (a < 0);
		}
	};

	// Represents a unique key for each resource.
	typedef DXStringMapKey DXResourceKey;

	// A <DXResourceKey, IDXResource*> pair stored in the resource map.
	struct DXResourceHandle
	{
		DXResourceKey	key;
		IDXResource*	pResource;

		bool operator==(DXResourceHandle handle)
		{
			int i = _wcsicmp(key.m_szName, handle.key.m_szName);
			return ( (i == 0) && (pResource == handle.pResource) );
		}

		DXResourceHandle()
		{
			key.m_szName[0] = 0;
			pResource = nullptr;
		}

		static DXResourceHandle GetInvalidResourceHandle()
		{
			static DXResourceHandle handle;
			return handle;
		}
	};


	typedef std::map<DXResourceKey, IDXResource*, DXStringMapKeyCompareFunc> DXResourceMap;
	// 1st value is the key. 2nd is the resource pointer.
	typedef DXResourceMap::iterator						DXResourceMapIterator;
	// 1st value is the key, and 2nd the pointer to the resource itself.
	typedef DXResourceMap::value_type					DXResourceHandleInternal;
	// 1st value is iterator to the element. 2nd is true if new element was inserted.
	typedef std::pair<DXResourceMapIterator, bool>		DXResourceMapInsertResult;


	// Base class for the resource managers. It forces the derived managers
	// to be singleton classes.
	class IDXResourceManager : public IDXUnknown
	{
	private:
		// A map stores <key, value> pairs. No 2 pairs can have the same key.
		DXResourceMap				m_mapResources;
	protected:
		// An iterator is used to iterate through a collection e.g. map in this case.
		DXResourceMapIterator		m_mapIterator;
	private:
		size_t						m_uiCurrentMemoryUsage;

	public:
		// Called to initialize the resource manager.
		virtual HRESULT			Create(void* pParams) = 0;
		// Should first make a key, insert it into the map using InsertResource()
		// if new element is inserted, then it should allocate memory for resource
		// Create() it and AddMemory() to update the manager's memory usage. AddRef()
		// must also be called on the resource.
		// It should return the key of the resource whether new one or old one.
		// What should happen if resource creation fails is the concern of the
		// derived class. In case of failure the return value is { DXResourceKey(nullptr), nullptr };
		virtual DXResourceHandle	CreateResource(void *pParams, WCHAR* szCustomKey=nullptr) = 0;
		HRESULT						RecreateResource(DXResourceKey key);
		HRESULT						RecreateResource(IDXResource* pResource);
		HRESULT						RecreateResource(DXResourceHandle handle);
		// Destroys all the resources.
		void					Destroy();
		// Destroys the resource, removes it for the manager and frees its memory.
		void					DestroyResource(IDXResource* pResource);
		// Destroys the resource given its key.
		void					DestroyResource(DXResourceKey key);
		// Gets a resource given its key. 0 if not found.
		IDXResource*			GetResource(DXResourceKey key);

		// Methods to iterate through the map. Used internally mainly.
	public:
		// Gets the resource pointed by the current iterator.
		IDXResource*	GetCurrentResource()			{ return m_mapIterator->second;				};
		// Sets the current iterator to the 1st resource.
		void			GotoBegin()						{ m_mapIterator = m_mapResources.begin();	};
		// Sets the current iterator to beyond the last resource.
		void			GotoEnd()						{ m_mapIterator = m_mapResources.end();		};
		// Steps the iterator forward by 1.
		void			GotoNext()						{ ++m_mapIterator;							};
		// Checks if the iterator is in the valid range. NOTE: This function gives a valid
		// return only if it is called after every call to GotoNext().
		bool			IsIterValid()					{ return (m_mapIterator != m_mapResources.end()); };
		size_t			GetMemoryUsage() const			{ return m_uiCurrentMemoryUsage;	};

	protected:
		void		ClearMap()						{ m_mapResources.clear();			};
		void		AddMemoryUsage(size_t mem)		{ m_uiCurrentMemoryUsage += mem;	};
		void		DecreaseMemoryUsage(size_t mem)	{ m_uiCurrentMemoryUsage -= mem;	};

		// Inserts the resource into the map if already not present. The second element
		// of the return value is true if a new element was inserted. False if an element
		// with the same key already existed.
		DXResourceMapInsertResult	InsertResource(DXResourceKey key, IDXResource* pResource);
		// Removes the resource from the map. NOTE: This does not destroy the resource
		// nor deallocates its memory.
		void					RemoveResource(IDXResource* pResource);
		void					RemoveResource(DXResourceKey key);
		void					DestroyResource(DXResourceMapIterator iter);

	public:
		virtual UINT		GetTypeId()		const	{ return DX_RESOURCE_MANAGER_TYPE_ID;	};
		virtual LPCWSTR		GetTypeName()	const	{ return DX_RESOURCE_MANAGER_TYPE_NAME;	};

	protected:
		IDXResourceManager();

	public:
		virtual ~IDXResourceManager();
	};

} // DXFramework namespace end