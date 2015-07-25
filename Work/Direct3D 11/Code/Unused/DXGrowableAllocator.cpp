#include "DXStdafx.h"
#include "DXGrowableAllocator.h"

namespace DXFramework
{ // DXFramework namespace begin


	IDXGrowableAllocator::IDXGrowableAllocator(IDXBaseAllocator* pParentAllocator)	:	
														m_bResizeable(false),
														m_pMemoryStart(nullptr),
														m_pParentAllocator(pParentAllocator),
														m_uiSize(0),
														m_uiNextAvailableId(0),
														m_uiMemoryGrowThreshold(DX_DEFAULT_ALLOCATOR_GROW_THRESHOLD)
	{
		UpdateIterators();
	}


	IDXGrowableAllocator::~IDXGrowableAllocator()
	{
		m_mapppRegisteredPointers.clear();
		m_uiSize = 0;
	}


	UINT32 IDXGrowableAllocator::RegisterPointerForUpdate(void** pp)
	{
		std::pair<DXRegisteredPointersMapIterator, bool> result(m_mapppRegisteredPointers.insert(DXRegisteredPointersMap::value_type(m_uiNextAvailableId, pp)));
		
		if (result.second)
		{
			UpdateIterators();
			return m_uiNextAvailableId++;
		}

		return (UINT32)-1;
	}


	void IDXGrowableAllocator::UnregisterPointer(void** pp)
	{
		DXRegisteredPointersMapIterator iter = m_mapppRegisteredPointers.begin();
		while (iter != m_iterEnd)
		{
			if (iter->second == pp)
			{
				m_mapppRegisteredPointers.erase(iter);
				UpdateIterators();
				break;
			}
			++iter;
		}

		if (!m_mapppRegisteredPointers.size())
		{
			m_uiNextAvailableId = 0;
		}
	}


	void IDXGrowableAllocator::UpdatePointers(uintptr_t uiDelta, bool bIncrease)
	{
		// Iterate through the vector.
		DXRegisteredPointersMapIterator iter = m_iterBegin;
		while (iter != m_iterEnd)
		{
			// The address of the pointer variable.
			void** pp = iter->second;
			// Get the address as a number.
			uintptr_t pos = (uintptr_t)(*pp);

			// Update the number address.
			if (bIncrease)
			{
				pos += uiDelta;
			}
			else
			{
				pos -= uiDelta;
			}

			// Store the number address again as a pointer in the register pointer variable.
			*iter->second = (void*)pos;

			// Step the iterator foreward.
			++iter;
		}
	}


	bool IDXGrowableAllocator::Grow(uintptr_t uiIncrease)
	{
		// It's strange that something would ask for 0 bytes increase. But we can't say that the
		// method called failed can't we?
		if (!uiIncrease)
		{
			DX_DEBUG_OUTPUT1(L"WARNING!!! 0 increase demanded. ALLOCATOR NAME: %s.", m_wstrDebugName.data());
			return true;
		}

		// Can't grow if not allowed.
		if (!m_bResizeable)
		{
			DX_DEBUG_OUTPUT1(L"ERROR!!! Allocator not resizable. ALLOCATOR NAME: %s.", m_wstrDebugName.data());
			return false;
		}

		if (!m_pMemoryStart)
		{
			DX_DEBUG_OUTPUT1(L"ERROR!!! Invalid initial memory block. ALLOCATOR NAME: %s.", m_wstrDebugName.data());
			return false;
		}

		if (!m_pParentAllocator)
		{
			DX_DEBUG_OUTPUT1(L"ERROR!!! Invalid parent allocator. ALLOCATOR NAME: %s.", m_wstrDebugName.data());
			return false;
		}

		
		// Calculate the new size.

		uintptr_t temp1;
		uintptr_t delta;

		temp1 = m_uiMemoryGrowThreshold / m_uiSize;
		switch (temp1)
		{
		case 0:		// Current size greater than threshold.
			delta = m_uiMemoryGrowThreshold;		// Increase by threshold.
			break;
		case 1:		// Size equal to threshold.
			delta = m_uiMemoryGrowThreshold / 2;	// Make it equal to threshold.
			break;
		default:	// Size smaller than or equal to half of threshold.
			delta = m_uiSize;	// Double it.
			break;
		}

		// Still insufficient?
		if (delta < uiIncrease)
		{
			// Make sure we have enough.
			delta += uiIncrease;
		}

		uintptr_t newSize = m_uiSize + delta;

		// Try to acquire a new block of required size from the parent allocator.
		void* pNewBlock = m_pParentAllocator->Reallocate(m_pMemoryStart, newSize);
		if (!pNewBlock)
		{
			DX_DEBUG_OUTPUT1(L"ERROR!!! Unable to reallocate memory. ALLOCATOR NAME: %s.", m_wstrDebugName.data());
			return false;
		}

		// The old block's address.
		uintptr_t uiOldPos = (uintptr_t)m_pMemoryStart;
		// The new block's address.
		uintptr_t uiNewPos = (uintptr_t)pNewBlock;

		// Update our memory block size record.
		m_uiSize = newSize;

		bool bAddressIncreased = false;
		uintptr_t uiAddressDelta = 0;
		// If the 2 addresses are different we need to update the registered pointers pointing
		// to addresses in the old block. And also give the news to the derived class so that
		// it can make any necessary changes.
		if (uiOldPos != uiNewPos)
		{
			DX_DEBUG_OUTPUT1(L"Memory block moved after reallocation. ALLOCATOR NAME: %s.", m_wstrDebugName.data());
			// Update our memory block's start pointer.
			m_pMemoryStart = pNewBlock;

			bAddressIncreased = uiNewPos > uiOldPos;
			uiAddressDelta = bAddressIncreased ? (uiNewPos - uiOldPos) : (uiOldPos - uiNewPos);
			UpdatePointers(uiAddressDelta, bAddressIncreased);
		}
		
		// Allow the derived class to perform any house keeping required.
		AfterMemoryResize(uiAddressDelta, bAddressIncreased);

		DX_DEBUG_OUTPUT1(L"Success!!! ALLOCATOR NAME: %s.", m_wstrDebugName.data());

		return true;
	}

} // DXFramework namespace end