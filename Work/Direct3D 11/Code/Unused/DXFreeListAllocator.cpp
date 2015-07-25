#include "DXFreeListAllocator.h"


namespace DXFramework
{ // DXFramework namespace begin

	CDXFreeListAllocator::CDXFreeListAllocator(uintptr_t uiSize, void* pStart)	:	m_pFreeBlocks((DXFreeBlock*)pStart)
	{
		// Can't have a memory smaller than the size of the linked list element.
		if (uiSize < sizeof(DXFreeBlock))
		{
			return;
		}

		// Setup the linked list. Initially it has only one block the size of the given memory size
		// and, thus, no next free memory block.
		m_pFreeBlocks->uiSize = uiSize;
		m_pFreeBlocks->pNextFreeBlock = nullptr;
	}


	CDXFreeListAllocator::~CDXFreeListAllocator()
	{
		m_pFreeBlocks = nullptr;
	}


	void* CDXFreeListAllocator::Allocate(uintptr_t uiSize, U8 uiAlignment)
	{

		// The free block that was previously visited.
		DXFreeBlock* pPreviousFreeBlock = nullptr;
		// The current free block. Intially the first free block.
		DXFreeBlock* pFreeBlock = m_pFreeBlocks;

		// While there is a free block.
		while (pFreeBlock)
		{
			// Find the memory address adjustment for this allocation size plus the size of the header.
			U8 uiAdjustment = DXAlignAdjustmentWithHeader(pFreeBlock, uiAlignment, sizeof(DXAllocationHeader));
			// If the size of this free block is enough to hold the aligned object
			// (including necessary adjustment).
			if (pFreeBlock->uiSize < uiSize + uiAdjustment)
			{
				// Set it as our previous block.
				pPreviousFreeBlock = pFreeBlock;
				// Move to the next free block.
				pFreeBlock = pFreeBlock->pNextFreeBlock;
				// Continue the loop on the the next block.
				continue;
			}
			// Free block has enough room.

			if (sizeof(DXAllocationHeader) < sizeof(DXFreeBlock))
			{
				return 0;
			}

			// If the remaining space in the free block after allocating it would be insufficient
			// for further allocation.
			if (pFreeBlock->uiSize - uiSize - uiAdjustment <= sizeof(DXAllocationHeader))
			{
				// Set the size of our allocation to take up the entire free block.
				uiSize = pFreeBlock->uiSize;

				if (pPreviousFreeBlock != nullptr)
				{
					pPreviousFreeBlock = pFreeBlock->pNextFreeBlock;
				}
				else
				{
					m_pFreeBlocks = pFreeBlock->pNextFreeBlock;
				}
			}
			else // The remaining space in the free block is sufficient for further allocations.
			{
				// Get the pointer to the address where the next free block should begin, i.e at the
				// end of the demanded space plus the necessary adjustment.
				DXFreeBlock* pNextBlock = (DXFreeBlock*)( (uintptr_t)pFreeBlock + uiSize + uiAdjustment );
				// Find the size of this new block. Simply the remaining size in the free block.
				pNextBlock->uiSize = pFreeBlock->uiSize - uiSize - uiAdjustment;
				// The free block next to this new block will be the free block that was next to the
				// original free block.
				pNextBlock->pNextFreeBlock = pFreeBlock->pNextFreeBlock;

				if (pPreviousFreeBlock != nullptr)
				{
					pPreviousFreeBlock = pNextBlock;
				}
				else
				{
					m_pFreeBlocks = pNextBlock;
				}
			}

			// Calculate the aligned address to the begining of the allocated memory.
			uintptr_t uiAlignedAddress = (uintptr_t)pFreeBlock + uiAdjustment;
			// Go back sizeof(DXAllocationHeader) bytes for our allocation header.
			DXAllocationHeader* pHeader = (DXAllocationHeader*)(uiAlignedAddress - sizeof(DXAllocationHeader));
			// Setup the allocation header.
			pHeader->uiAdjustment = uiAdjustment;
			pHeader->uiSize = uiSize + uiAdjustment;

			// Update our memory usage.
			m_uiUsedMemory += uiAdjustment + uiSize;
			m_uiNumAllocations++;

			// Return the address.
			return (void*)uiAlignedAddress;
		}

		return nullptr;
	}


	void CDXFreeListAllocator::Deallocate(void* p)
	{
		DXAllocationHeader* pHeader = (DXAllocationHeader*)( (uintptr_t)p - sizeof(DXAllocationHeader) );
		uintptr_t uiSize = pHeader->uiSize;

		uintptr_t uiBlockStart = (uintptr_t)p - pHeader->uiAdjustment;
		uintptr_t uiBlockEnd = uiBlockStart + uiSize;
		uintptr_t uiBlockSize = uiSize;

		bool bBlockMerged = false;
		bool bSearch = true;

		while (bSearch)
		{
			bSearch = false;

			DXFreeBlock* pPreviousFreeBlock = 0;
			DXFreeBlock* pFreeBlock = m_pFreeBlocks;

			while (pFreeBlock != 0)
			{
				if ( (uintptr_t)pFreeBlock + pFreeBlock->uiSize == uiBlockStart )
				{
					pFreeBlock->uiSize += uiBlockSize;
					uiBlockStart = (uintptr_t)pFreeBlock;
					uiBlockEnd = uiBlockStart + pFreeBlock->uiSize;
					uiBlockSize = pFreeBlock->uiSize;

					bSearch = true;
					bBlockMerged = true;
					break;
				}
				else if (uiBlockEnd == (uintptr_t)pFreeBlock)
				{
					DXFreeBlock* pNewFreeBlock = (DXFreeBlock*)uiBlockStart;
					pNewFreeBlock->pNextFreeBlock = pFreeBlock->pNextFreeBlock;
					pNewFreeBlock->uiSize = uiBlockSize + pFreeBlock->uiSize;

					if (pFreeBlock == m_pFreeBlocks)
					{
						m_pFreeBlocks = pNewFreeBlock;
					}
					else if (pPreviousFreeBlock != pNewFreeBlock)
					{
						pPreviousFreeBlock->pNextFreeBlock = pNewFreeBlock;
					}

					uiBlockStart = (uintptr_t)pNewFreeBlock;
					uiBlockEnd = uiBlockStart + pNewFreeBlock->uiSize;
					uiBlockSize = pNewFreeBlock->uiSize;

					bSearch = true;
					bBlockMerged = true;
					break;
				}

				pPreviousFreeBlock = pFreeBlock;
				pFreeBlock = pFreeBlock->pNextFreeBlock;
			}
		}

		if (!bBlockMerged)
		{
			DXFreeBlock* pBlock = (DXFreeBlock*)( (uintptr_t)p - pHeader->uiAdjustment );
			pBlock->pNextFreeBlock = m_pFreeBlocks;
			pBlock->uiSize = uiBlockSize;

			m_pFreeBlocks = pBlock;
		}

		m_uiNumAllocations--;
		m_uiUsedMemory -= uiSize;
	}

} // DXFramework namespace end