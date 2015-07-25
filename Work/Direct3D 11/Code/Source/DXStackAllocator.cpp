#include "DXStdafx.h"
#include "DXStackAllocator.h"


namespace DXFramework
{ // DXFramework namespace begin

	CDXStackAllocator::CDXStackAllocator()	:
		m_pInitialPosition(nullptr),
		m_pCurrentPosition(nullptr),
		m_pPreviousPosition(nullptr),
		m_uiNumAllocations(0),
		m_uiTotalMemory(0),
		m_uiUsedMemory(0)
	{

	}


	CDXStackAllocator::~CDXStackAllocator()
	{
		m_pInitialPosition = nullptr;
		m_pCurrentPosition = nullptr;
		m_pPreviousPosition = nullptr;
		m_uiNumAllocations = 0;
		m_uiTotalMemory = 0;
		m_uiUsedMemory = 0;
	}


	bool CDXStackAllocator::Initialize(uintptr_t uiSize, void* pStart)
	{
		if (!uiSize || !pStart)
		{
			DX_DEBUG_OUTPUT1(L"ERROR!!! Invalid args. ALLOCATOR NAME: %s.", m_wstrDebugName.data());
			return false;
		}

		m_uiTotalMemory = uiSize;
		m_pCurrentPosition = pStart;
		m_pInitialPosition = pStart;

		return true;
	}


#if defined (DX_ALLOCATOR_DEBUG)
	DXAllocatorMemoryStatus CDXStackAllocator::GetStatus()
	{
		DXAllocatorMemoryStatus dump;

		static const uintptr_t uiHeaderDebugDataSize = (sizeof(WCHAR) * DX_MAX_FILE_NAME) + sizeof(DWORD);

		// Higher address. Intially the address where the header of next block would begin
		DXStackAllocationHeader* pUpper = (DXStackAllocationHeader*)m_pCurrentPosition;
		// Lower address. Intitally the begining of actual memory of top most block. (Invalid
		// header).
		DXStackAllocationHeader* pLower = (DXStackAllocationHeader*)m_pPreviousPosition;

		// Is there any allocation.
		if (m_pCurrentPosition != m_pInitialPosition)
		{
			while (pLower)
			{
				// Go to the start of the block header.
				--pLower;

				DXAllocatorMemoryBlockStatus block;

				uintptr_t uiBlockSize = (uintptr_t)pUpper - (uintptr_t)pLower;
				block.uiAllocationSize = uiBlockSize;
				block.uiRequestedAllocationSize = uiBlockSize - ( sizeof(DXStackAllocationHeader) + sizeof(DXStackAllocationFooter) );
				block.dwLine = pLower->dwLineNumber;
				wcscpy_s(block.szFile, DX_MAX_FILE_NAME+1, pLower->szFile);

				dump.vecMemoryBlocks.push_back(block);

				pUpper = pLower;
				// This doesn't set pLower of the start of the previous block's header but
				// instead to the start of the block's actual memory.
				pLower = (DXStackAllocationHeader*)pLower->pPreviousAddress;
			}
		}

		uintptr_t numAllocations = dump.vecMemoryBlocks.size();
		dump.uiDebugMemory = numAllocations * ( uiHeaderDebugDataSize + sizeof(DXStackAllocationFooter) );
		dump.uiBookKeepingMemory = numAllocations * ( sizeof(DXStackAllocationHeader) - uiHeaderDebugDataSize );
		dump.uiTotalMemory = m_uiTotalMemory;
		dump.uiAllocatedMemory = m_uiUsedMemory;
		dump.uiRequestedMemory = dump.uiAllocatedMemory - (dump.uiDebugMemory + dump.uiBookKeepingMemory);
		
		return dump;
	}
#endif

	void* CDXStackAllocator::Allocate(uintptr_t uiSize)
	{
		if (!uiSize)
		{
			DX_DEBUG_OUTPUT1(L"WARNING!! Size 0 allocation. Returning nullptr. ALLOCATOR NAME: %s.", m_wstrDebugName.data());
			return nullptr;
		}

		uintptr_t requiredMemory = uiSize + sizeof(DXStackAllocationHeader);
//#if defined (DX_ALLOCATOR_DEBUG)
//		requiredMemory += sizeof(DXStackAllocationFooter);
//#endif

		uintptr_t remainingMemory = m_uiTotalMemory - m_uiUsedMemory;
		// Do we have insufficient memory.
		if (requiredMemory > remainingMemory)
		{
			DX_DEBUG_OUTPUT1(L"WARNING!! Insufficient memory. Returning nullptr. ALLOCATOR NAME: %s.", m_wstrDebugName.data());
			// Sorry!
			return nullptr;
		}

		// Setup the header for this allocation.
		DXStackAllocationHeader* pHeader = (DXStackAllocationHeader*)m_pCurrentPosition;
		pHeader->pPreviousAddress = m_pPreviousPosition;
		pHeader->uiAdjustment = sizeof(DXStackAllocationHeader);

		// Update the previous and current position.
		m_pPreviousPosition = (void*)( (uintptr_t)m_pCurrentPosition + sizeof(DXStackAllocationHeader) );
		m_pCurrentPosition = (void*)( (uintptr_t)m_pPreviousPosition + uiSize );


//#if defined (DX_ALLOCATOR_DEBUG)
//		// This also increases the current position pointer appropriately after setting the footer.
//		AddFooter();
//#endif

		// Update memory usage.
		m_uiUsedMemory += requiredMemory;
		++m_uiNumAllocations;

		return m_pPreviousPosition;
	}


	void* CDXStackAllocator::AllocateAligned(uintptr_t uiSize, UINT8 uiAlignment)
	{
		if (!uiSize)
		{
			DX_DEBUG_OUTPUT1(L"WARNING!! Size 0 allocation. Returning nullptr. ALLOCATOR NAME: %s.", m_wstrDebugName.data());
			return nullptr;
		}

#if defined (DX_ALLOCATOR_DEBUG)
		if (!DXAlignmentValid(uiAlignment))
		{
			DX_DEBUG_OUTPUT1(L"WARNING!! Invalid alignment. ALLOCATOR NAME: %s.", m_wstrDebugName.data());
		}
#endif

		uintptr_t uiAlignedAddress = (uintptr_t)m_pCurrentPosition;
		UINT8 uiAdjustment = DXAlignAdjustmentWithHeader(m_pCurrentPosition, uiAlignment, sizeof(DXStackAllocationHeader));
		uiAlignedAddress += uiAdjustment;

		uintptr_t uiRequiredSize = uiSize + uiAdjustment;
		
//		// Reserve space for footer as well if debug mode is on.
//#if defined (DX_ALLOCATOR_DEBUG)
//		uiRequiredSize += sizeof(DXStackAllocationFooter);
//#endif
		
		uintptr_t uiRemainingSize = m_uiTotalMemory - m_uiUsedMemory;

		// If we don't have insufficient memory.
		if (uiRequiredSize > uiRemainingSize)
		{
			DX_DEBUG_OUTPUT1(L"WARNING!! Insufficient memory. Cannot grow. Returning nullptr. ALLOCATOR NAME: %s.", m_wstrDebugName.data());
			return nullptr;
		}

		// Setup the header for this allocation.
		DXStackAllocationHeader* pHeader = (DXStackAllocationHeader*)uiAlignedAddress - 1;
		pHeader->pPreviousAddress = m_pPreviousPosition;
		pHeader->uiAdjustment = uiAdjustment;

		// Update the previous and current position.
		m_pPreviousPosition = (void*)(uiAlignedAddress);
		m_pCurrentPosition = (void*)((uintptr_t)m_pPreviousPosition + uiSize);

//#if defined (DX_ALLOCATOR_DEBUG)
//		AddFooter();
//#endif

		// Update memory usage.
		m_uiUsedMemory += uiRequiredSize;
		++m_uiNumAllocations;

		return (void*)uiAlignedAddress;
	}


	void CDXStackAllocator::Deallocate(void* p)
	{
		if (p != m_pPreviousPosition)
		{
			DX_DEBUG_OUTPUT1(L"WARNING!!! Incorrect address given. Stack can't be deallocated in arbitrary order. ALLOCATOR NAME: %s.", m_wstrDebugName.data());
			return;
		}

		//DXStackAllocationFooter* pFooter = (DXStackAllocationFooter*)( (uintptr_t)m_pCurrentPosition - sizeof(DXStackAllocationFooter) );
		//if (pFooter->uiFoot != DX_STACK_ALLOCATION_FOOTER)
		//{
		//	DX_DEBUG_OUTPUT1(L"WARNING!!! Invalid footer. ALLOCATOR NAME: %s.", m_wstrDebugName.data());
		//	return;
		//}

		// Get the allocation header.
		DXStackAllocationHeader* pHeader = (DXStackAllocationHeader*)p - 1;
		// Decrease the memory usage. m_pCurrentPosition is the top of stack. p is at the begining
		// of block (the header and any padding are before p).
		m_uiUsedMemory -= (uintptr_t)m_pCurrentPosition - (uintptr_t)p + pHeader->uiAdjustment;

		// Update the top of stack.
		m_pCurrentPosition = (void*)( (uintptr_t)p - pHeader->uiAdjustment );

		m_pPreviousPosition = pHeader->pPreviousAddress;

		--m_uiNumAllocations;
	}

} // DXFramework namespace end