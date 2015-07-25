#include "DXProxyAllocator.h"


namespace DXFramework
{ // DXFramework namespace begin

	CDXProxyAllocator::CDXProxyAllocator(IDXAllocator* pAllocator)	:	m_pAllocator(pAllocator)
	{
		if (!m_pAllocator)
		{
		}
	}


	CDXProxyAllocator::~CDXProxyAllocator()
	{
		m_pAllocator = nullptr;
	}


	void* CDXProxyAllocator::Allocate(uintptr_t uiSize, U8 uiAlignment)
	{
		if (uiSize)
		{
			// Get the current memory usage of the allocator.
			uintptr_t uiMemory = m_pAllocator->GetUsedMemory();
			// Allocate.
			void* p = m_pAllocator->Allocate(uiSize, uiAlignment);
			if (p) // Succcess?
			{
				// Add the delta(Memory Usage) to the used memory. This is done because more than
				// one proxy allocators may be hosting the same allocator. Each proxy allocator should
				// only keep track of its own allocations.
				m_uiUsedMemory += m_pAllocator->GetUsedMemory() - uiMemory;
				++m_uiNumAllocations;
			}

			return p;
		}

		return nullptr;
	}


	void CDXProxyAllocator::Deallocate(void* p)
	{
		if (p)
		{
			uintptr_t uiMemory = m_pAllocator->GetUsedMemory();
			m_pAllocator->Deallocate(p);
			m_uiUsedMemory -= uiMemory - m_pAllocator->GetUsedMemory();
			--m_uiNumAllocations;
		}
	}

} // DXFramework namespace end