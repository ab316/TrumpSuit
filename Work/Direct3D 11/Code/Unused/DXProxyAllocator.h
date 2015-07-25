#pragma once

#include "DXAllocator.h"

namespace DXFramework
{ // DXFramework namespace begin

	// A proxy allocator simply redirects all allocations/deallocations to the allocator passed
	// as argument while keeping track of its memory. It is useful when more than one sub systems
	// are using the same allocator and each needs to keep track of its own memory. Direct use of
	// other memory allocators used be avoided. Instead use ProxyAllocator directly as it has some
	// security checks.
	class CDXProxyAllocator : public IDXAllocator
	{
	private:
		// The client allocator.
		IDXAllocator*		m_pAllocator;

	public:
		virtual	void*		Allocate(uintptr_t uiSize, U8 uiAlignment);
		virtual void		Deallocate(void* p);

		// Now copying allowed.
	private:
		CDXProxyAllocator(const CDXProxyAllocator&)					{ };
		CDXProxyAllocator& operator=(const CDXProxyAllocator&)		{ };

	public:
		CDXProxyAllocator(IDXAllocator* pAllocator);
		~CDXProxyAllocator();
	};

} // DXFramework namespace end