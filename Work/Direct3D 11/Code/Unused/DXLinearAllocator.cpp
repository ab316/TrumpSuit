#include "DXLinearAllocator.h"


namespace DXFramework
{ // DXFramework namespace begin

	CDXLinearAllocator::CDXLinearAllocator(uintptr_t uiSize, void* pStart)	:	m_uiSize(uiSize),
		m_pInitialPosition(pStart),
		m_pCurrentPosition(pStart)
	{
		if (uiSize == 0)
		{
		}
	}


	CDXLinearAllocator::~CDXLinearAllocator()
	{
		m_pInitialPosition = nullptr;
		m_pCurrentPosition = nullptr;
		m_uiSize = 0;
	}


	void* CDXLinearAllocator::Allocate(uintptr_t uiSize, U8 uiAlignment)
	{
		// Calculate the necessary adjustment to properly align the allocation.
		U8 uiAdjustment = DXAlignAdjustment(m_pCurrentPosition, uiAlignment);
		// If the required memory is greater than what is left.
		if ( (m_uiUsedMemory + uiAdjustment + uiSize) > m_uiSize)
		{
			// Sorry!
			return nullptr;
		}

		// Calculate the aligned address.
		uintptr_t uiAlignedAddress = (uintptr_t)m_pCurrentPosition + uiAdjustment;
		// Increase the next free position pointer.
		m_pCurrentPosition = (void*)(uiAlignedAddress + uiSize);

		// Update memory usage.
		m_uiUsedMemory += uiSize + uiAdjustment;
		--m_uiNumAllocations;

		return (void*)uiAlignedAddress;
	}


	void CDXLinearAllocator::Deallocate(void* p)
	{
		// Individual deallocations not possible in linear allocator.
	}


	void CDXLinearAllocator::Clear()
	{
		// Clear all allocations.
		m_uiUsedMemory  = 0;
		m_uiNumAllocations = 0;
		m_pCurrentPosition = m_pInitialPosition;
	}

} // DXFramework namespace end