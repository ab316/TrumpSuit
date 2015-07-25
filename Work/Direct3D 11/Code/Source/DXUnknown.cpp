#include "DXStdafx.h"
#include "DXRefCounter.h"
#include "DXAllocator.h"
#include "DXUnknown.h"

namespace DXFramework
{ // DXFramework namespace begin

#if defined(DX_DEBUG)

	void* CDXDynamicallyAllocatedObject::operator new(size_t uiBlockSize, IDXBaseAllocator* pAllocator, UINT8 ubAlign, WCHAR* szFile, DWORD dwLine)
	{
		return pAllocator->AllocateAligned(uiBlockSize, ubAlign);
	}

#else

	void* CDXDynamicallyAllocatedObject::operator new(size_t uiBlockSize, IDXBaseAllocator* pAllocator, UINT8 ubAlign)
	{
		return pAllocator->AllocateAligned(uiBlockSize, ubAlign);
	}

#endif

	void CDXDynamicallyAllocatedObject::operator delete(void* p, IDXBaseAllocator* pAllocator)
	{
		pAllocator->DeallocateAligned(p);
	}


	IDXUnknown::IDXUnknown() : m_uiRefCount(0)
	{
#ifdef DX_DEBUG
		wcscpy_s(m_szName, DX_DEFAULT_NAME);
#endif
		// Add this instance to the ref counter.
		CDXRefCounter::GetInstance()->AddObject(this);
	}


	IDXUnknown::~IDXUnknown()
	{
		CDXRefCounter::GetInstance()->RemoveObject(this);
	}


	UINT IDXUnknown::Release()
	{
		if (m_uiRefCount == 0)
		{
			DX_DEBUG_OUTPUT2(L"Release() called on an object with 0 Ref count: Name: %s, Type: %s.", GetName(), GetTypeName());
			return 0;
		}
		else
		{
			UINT32 iOldRef = m_uiRefCount--;
			return iOldRef;
		}
	}


	void IDXUnknown::SetName(LPCWSTR szName)
	{
		if (szName)
		{
			wcscpy_s(m_szName, DX_MAX_NAME, szName);
		}
	}

} // DXFramework namespace end