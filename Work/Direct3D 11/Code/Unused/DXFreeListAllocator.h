#pragma once

#include "DXAllocator.h"


namespace DXFramework
{ // DXFramework namespace begin

	// A free list allocator allows allocations of any size to be made and deallocations in
	// any order by maintaining a linked-list of free memory blocks.
	class CDXFreeListAllocator : public IDXAllocator
	{
	private:
		// Header for each allocation made.
		struct DXAllocationHeader
		{
			uintptr_t	uiSize;
			uintptr_t	uiAdjustment;
		};


		// A free-block linked list element.
		struct DXFreeBlock
		{
			DXFreeBlock*	pNextFreeBlock;
			uintptr_t		uiSize;
		};


	private:
		// The free block linked list. Points to the first free block.
		DXFreeBlock*	m_pFreeBlocks;


	public:
		// Returns the first free block large enough.
		virtual	void*		Allocate(uintptr_t uiSize, U8 uiAlignment);
		virtual void		Deallocate(void* p);

		// No copying allowed.
	private:
		CDXFreeListAllocator(const CDXFreeListAllocator&)					{ };
		CDXFreeListAllocator& operator=(const CDXFreeListAllocator&)		{ };

	public:
		CDXFreeListAllocator(uintptr_t uiSize, void* pStart);
		~CDXFreeListAllocator();
	};

} // DXFramework namespace end