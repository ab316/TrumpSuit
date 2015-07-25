#pragma once

namespace DXFramework
{ // DXFramework namespace begin

	// Type name and id define a derived class of IDXUnknown.
#define DX_UNKNOWN_TYPE_NAME				L"DX_UNKNOWN"
#define DX_APPLICATION_TYPE_NAME			L"DX_BASE_APPLICATION"
#define DX_GRAPHICSDRIVER_TYPE_NAME			L"DX_GRAPHICS_DRIVER"
#define DX_TIMER_TYPE_NAME					L"DX_TIMER"
#define DX_RESOURCE_TYPE_NAME				L"DX_RESOURCE"
#define DX_RESOURCE_MANAGER_TYPE_NAME		L"DX_RESOURCE_MANAGER"
#define DX_3DDRIVER_TYPE_NAME				L"DX_3D_DRIVER"
#define DX_UIELEMENT_TYPE_NAME				L"DX_UI_ELEMENT"
#define DX_UIMANAGER_TYPE_NAME				L"DX_UI_MANAGER"
#define DX_TEXTRENDERER_TYPE_NAME			L"DX_TEXTRENDERER"
#define DX_SPRITE_TYPE_NAME					L"DX_SPRITE"

#define DX_UNKNOWN_TYPE_ID					0
#define DX_APPLICATION_TYPE_ID				1
#define DX_GRAPHICSDRIVER_TYPE_ID			2
#define DX_TIMER_TYPE_ID					3
#define DX_RESOURCE_TYPE_ID					4
#define DX_RESOURCE_MANAGER_TYPE_ID			5
#define DX_3DDRIVER_TYPE_ID					6
#define DX_UIELEMENT_TYPE_ID				7
#define DX_UIMANAGER_TYPE_ID				8
#define DX_TEXT_RENDERER_TYPE_ID			9
#define DX_SPRITE_TYPE_ID					10

	class IDXBaseAllocator;

	class CDXDynamicallyAllocatedObject
	{

	public:
#if defined(DX_DEBUG)
		void* operator new(size_t uiBlockSize, IDXBaseAllocator* pAllocator, UINT8 ubAlign, WCHAR* szFile, DWORD dwLine);
#else
		void* operator new(size_t uiBlockSize, IDXBaseAllocator* pAllocator, UINT8 ubAlign);
#endif
		void operator delete(void* p, IDXBaseAllocator* pAllocator);
	};

	// Base class of the DXFramework classes.
	// All DXFramework classes will inherit from this base class. This class stores
	// the the reference count which must be 0 when the object is no longer need.
	// It also holds a name for the object for debugging purposes.
	// SetName() is instance specific.

	// NOTE: The RefCount system does not seem to fit in here. It is useful only
	// for resources but not here. Planning to remove it.
	class IDXUnknown
	{
	public:

	private:
		UINT			m_uiRefCount;
		WCHAR			m_szName[DX_MAX_NAME+1];

	public:
		// AddRef() and Release() return the old Ref count.
		// AddRef() should be called in those functions which return
		// a pointer to a derived class of IDXUnknown.
		UINT				AddRef()					{ return m_uiRefCount++;};
		// Should be called before a derived class instance is destroyed,
		// or when it will no longer be accessible (going out of scope). Returns the
		// previous ref count.
		UINT				Release();
		UINT				GetRefCount()	const		{ return m_uiRefCount;	};
		LPCWSTR				GetName()		const		{ return m_szName;		};

		virtual UINT		GetTypeId()		const		= 0;
		virtual LPCWSTR		GetTypeName()	const		= 0;

	public:
		// Sets a name for the object. For debugging purposes
		// only. Calls to this function should not be included in Release build.
		void		SetName(LPCWSTR szName);

	public:
		// When IDXUnknown is created (ctor called) it sets its RefCount to 0
		// and calls the AddObject method of the CDXRefCounter singleton class which
		// adds this instance into the vector of the singleton class.
					IDXUnknown();
		// Ref is not decreased when dtor is called. It is the responsibility
		// of the instance to call Release().
		// The dtor is called according to the static type of any class pointer. 
		// If we have saved derived class pointers in IDXUnknown class pointers,
		// when we will call dtor using delete, the dtor of base class IDXUnknown 
		// will be called. This will destroy the base class only. The derived class
		// will remain intact. Making the dtor virtual solves this problem.
		virtual		~IDXUnknown();
	};

} // DXFramework namespace end