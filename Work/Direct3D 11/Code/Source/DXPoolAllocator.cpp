#include "DXStdafx.h"
#include "DXPoolAllocator.h"


namespace DXFramework
{ // DXFramework namespace begin

	CDXPoolAllocator::CDXPoolAllocator()	:	m_pInitialPosition(nullptr),
												m_ppFreeList(nullptr),
												m_ppLastAllocation(nullptr),
												m_uiNumAllocations(0),
												m_uiTotalMemory(0),
												m_uiUsedMemory(0)
	{
	}


	CDXPoolAllocator::~CDXPoolAllocator()
	{
		m_ppFreeList = nullptr;
		m_pInitialPosition = nullptr;
	}


	bool CDXPoolAllocator::Initialize(uintptr_t uiObjectSize, UINT8 uiObjectAlignment, uintptr_t uiSize, void* pMemory)
	{
		m_uiObjectSize = uiObjectSize;
		m_uiObjectAlignment = uiObjectAlignment;
		m_uiTotalMemory = uiSize;
		m_pInitialPosition = pMemory;

		// Object size must be greater than the size of a pointer (hardcoding it to 8 for consistency
		// with 64-bit platform).
		if (uiObjectSize < 8)
		{
			DX_DEBUG_OUTPUT1(L"WARNING!!! Object size less than 8. ALLOCATOR NAME:%s.", m_wstrDebugName.data());
			return false;
		}

		//uiObjectSize += sizeof(DXPoolAllocatorHeader);

		// Find the required adjustment for alignment.
		UINT8 uiAdjustment = DXAlignAdjustment(pMemory, uiObjectAlignment);
		// The linked list of pointers now points to the begining of the adjusted memory address.
		m_ppFreeList = (void**)( (uintptr_t)pMemory + uiAdjustment );
		// Calculate the number of objects that can be stored in the pool.
		uintptr_t uiNumObjects = (uiSize - uiAdjustment) / uiObjectSize;

		if (!uiNumObjects)
		{
			DX_DEBUG_OUTPUT1(L"WARNING!!! Provided size less than size of object. ALLOCATOR NAME:%s.", m_wstrDebugName.data());
			return false;
		}

		// For iterating through the list.
		void** pp = m_ppFreeList;
		// For each of the objects except the last one.
		for (uintptr_t i=0; i<uiNumObjects-1; i++)
		{
			// Initialize the linked list so that each pointer in the list except the last one (of course)
			// points to the next object's address.
			*pp = (void*)( (uintptr_t)pp + uiObjectSize );
			// Move ahead the pointer to the next object.
			pp = (void**)(*pp);
		}

		// Last one is the nullptr terminator.
		*pp = nullptr;


		// Consider object of size 6 bytes. The linked list looks like this:
		// MEMORY ADDRESS:	0		6		12		18		24		30
		// m_pFreeList:		6		12		18		24		30		0

		return true;
	}


	void* CDXPoolAllocator::Allocate(uintptr_t uiSize)
	{
		// Get the address pointed by the free list. We don't use *m_ppFreeList because the value
		// pointed by m_ppFreeList is the address of the next free block. We need the current free block
		// not the next one.
		void* p = m_ppFreeList;

		// Valid free block?
		if (p)
		{
			m_ppLastAllocation = m_ppFreeList;
			// The free list now points to the address that was pointed to by the pointer stored
			// in the list in the original position i.e. the next free block.
			m_ppFreeList = (void**)(*m_ppFreeList);

			// After first allocation in the example given in ctor, linked list would look like this:
			// MEMORY ADDRESS:	6		12		18		24		30
			// m_pFreeList:		12		18		24		30		0

			m_uiUsedMemory += uiSize;
			++m_uiNumAllocations;
		}
		else
		{
			DX_DEBUG_OUTPUT1(L"WARNING!!! Insufficient memory. Returning nullptr. ALLOCATOR NAME: %s.", m_wstrDebugName.data());
			return nullptr;
		}

		return p;
	}


	void CDXPoolAllocator::Deallocate(void* p)
	{
		// Before Deallocate.
		// MEMORY ADDRESS:		6		12		18		24		30
		// MEMORY VALUE:		12		24		{obj}	30		0
		// p = 18
		// m_ppFreeList = 6

		// This list is as follow:
		// 6 -> 12 -> 24 -> 30

		///////////////////////////////////////////////////////////////////////////
		// Convert the given address to pointer to pointer so that we change bytes the size of a pointer
		// of the value pointed.
		void** pp = (void**)p;
		// Change the pointed value to the first free block.
		*pp = m_ppFreeList;
		// RESULT:
		// MEMORY ADDRESS:		6		12		18		24		30
		// MEMORY VALUE:		12		24		6		30		0
		// p = 18
		// m_ppFreeList = 6
		///////////////////////////////////////////////////////////////////////////


		///////////////////////////////////////////////////////////////////////////
		// Now the free list points to this address p.
		m_ppFreeList = pp;
		// RESULT:
		// MEMORY ADDRESS:		6		12		18		24		30
		// MEMORY VALUE:		12		24		6		30		0
		// p = 18
		// m_ppFreeList = 18
		///////////////////////////////////////////////////////////////////////////

		// This list now goes as follow:
		// 18 -> 6 -> 12 -> 24 -> 30

		m_uiUsedMemory -= m_uiObjectSize;
		--m_uiNumAllocations;
	}


	DXAllocatorMemoryStatus	CDXPoolAllocator::GetStatus()
	{
		DXAllocatorMemoryStatus status;

		status.uiTotalMemory = m_uiTotalMemory;
		status.uiBookKeepingMemory = 0;
		status.uiAllocatedMemory = m_uiObjectSize;
		status.uiDebugMemory = sizeof(DXPoolAllocatorHeader) * m_uiNumAllocations;
		status.uiRequestedMemory = status.uiAllocatedMemory - status.uiDebugMemory;

		return status;
	}

} // DXFramework namespace end