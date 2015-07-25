#pragma once

#include "DXAllocator.h"

namespace DXFramework
{ // DXFramework namespace begin

	// A pool allocator is used for allocating objects of the same size and alignment requirement.
	// The object size must be greater than the size of a pointer (4 bytes on 32-bit and 8 bytes on 64-bit
	// platforms).
	class CDXPoolAllocator : public IDXBaseAllocator
	{
		friend class CDXMemoryManager;

		struct DXPoolAllocatorHeader
		{
			WCHAR	szFile[DX_MAX_FILE_NAME+1];
			DWORD	dwLine;
		};

	private:
		// A linked list of free blocks. As blocks are of same size only pointer to next block
		// is needed.
		void**		m_ppFreeList;
		void**		m_ppLastAllocation;
		void*		m_pInitialPosition;
		// Size of single object.
		uintptr_t	m_uiObjectSize;
		uintptr_t	m_uiUsedMemory;
		uintptr_t	m_uiTotalMemory;
		uintptr_t	m_uiNumAllocations;
		// Object alignment.
		UINT8		m_uiObjectAlignment;

	public:
		virtual	uintptr_t	GetUsedMemory()			{ return m_uiUsedMemory;		};
		virtual	uintptr_t	GetTotalMemory()		{ return m_uiTotalMemory;		};
		virtual	uintptr_t	GetNumAllocations()		{ return m_uiNumAllocations;	};
		virtual	void*		GetMemoryStart()		{ return m_pInitialPosition;	};

		// Allocate and Deallocate currently do not work for arrays.
		virtual	void*	Allocate(uintptr_t uiSize);
		virtual void	Deallocate(void* p);
		
		virtual void*	AllocateAligned(uintptr_t uiSize, UINT8 uiAlign)
		{
			UNREFERENCED_PARAMETER(uiAlign);
			return Allocate(uiSize);
		}

		virtual void	DeallocateAligned(void* p)
		{
			Deallocate(p);
		};

		virtual	DXAllocatorMemoryStatus	GetStatus();

	public:
		bool	Initialize(uintptr_t uiObjectSize, UINT8 uiObjectAlignment, uintptr_t uiSize, void* pMemory);

		// No copying allowed.
	private:
		CDXPoolAllocator(const CDXPoolAllocator&);
		CDXPoolAllocator& operator=(const CDXPoolAllocator&);

	public:
		CDXPoolAllocator();
		~CDXPoolAllocator();
	};

} // DXFramework namespace end