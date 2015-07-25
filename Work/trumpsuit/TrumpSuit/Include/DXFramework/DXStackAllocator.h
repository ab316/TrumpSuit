#pragma once

#include "DXAllocator.h"


namespace DXFramework
{ // DXFramework namespace begin

#define DX_STACK_ALLOCATION_FOOTER 23130	// The characters ZZ

	// Stack based allocator. Allows aligned allocations of any size. But deallocations can only
	// be made in reverse order.
	class CDXStackAllocator : public IDXBaseAllocator
	{
	private:
		// Allocations header for each allocation.
		__declspec(align(1))
		struct DXStackAllocationHeader
		{
#if defined (DX_ALLOCATOR_DEBUG)
			WCHAR	szFile[DX_MAX_FILE_NAME];
			DWORD	dwLineNumber;
#endif
			// Pointer to the previous allocation.
			void*	pPreviousAddress;
			// Adjustment used in this allocation.
			UINT8	uiAdjustment;
		};

#if defined (DX_ALLOCATOR_DEBUG)
		struct DXStackAllocationFooter
		{
			UINT16	uiFoot;
		};
#endif

	private:
		// Start of stack.
		void*		m_pInitialPosition;
		// Pointer to last allocation made.
		void*		m_pPreviousPosition;
		// Top of stack.
		void*		m_pCurrentPosition;

		uintptr_t	m_uiUsedMemory;
		uintptr_t	m_uiTotalMemory;
		uintptr_t	m_uiNumAllocations;

		// Inherited methods.
	public:
		virtual	uintptr_t	GetUsedMemory()			{ return m_uiUsedMemory;		};
		virtual	uintptr_t	GetTotalMemory()		{ return m_uiTotalMemory;		};
		virtual	uintptr_t	GetNumAllocations()		{ return m_uiNumAllocations;	};
		virtual	void*		GetMemoryStart()		{ return m_pInitialPosition;	};

		virtual	void*	Allocate(uintptr_t uiSize);
		virtual void*	AllocateAligned(uintptr_t uiSize, UINT8 uiAlign);
		// Pop is preferred.
		virtual void	Deallocate(void* p);
		// Equivalent to Deallocate()
		virtual void	DeallocateAligned(void* p)		{	Deallocate(p);	};
#if defined (DX_ALLOCATOR_DEBUG)
		virtual	DXAllocatorMemoryStatus	GetStatus();
#endif

	public:
		bool	Initialize(uintptr_t uiSize, void* pStart);			
		// This should be used instead of Deallocate, because deallocations in stack
		// can only be made is reverse order of allocation.
		inline void	Pop()						{ Deallocate(m_pPreviousPosition);			};
		template <class T>	void PopDelete()	{ DeallocateDelete<T>(m_pPreviousPosition)	};
		template <class T>	void PopArray()		{ DeallocateArray<T>(m_pPreviousPosition)	};
		// Cleans the entire stack.
		inline void Clear()
		{
			m_pCurrentPosition = m_pInitialPosition;
			m_pPreviousPosition = m_pInitialPosition;
		}

	private:
		// A constant footer at the end is useful for detecting stack corruption.
		//__forceinline void	AddFooter()
		//{		
		//	DXStackAllocationFooter* pFooter = (DXStackAllocationFooter*)m_pCurrentPosition;
		//	pFooter->uiFoot = DX_STACK_ALLOCATION_FOOTER;
		//	m_pCurrentPosition = (void*)( (uintptr_t)m_pCurrentPosition + sizeof(DXStackAllocationFooter) );
		//}

		// No copying allowed.
	private:
		CDXStackAllocator(const CDXStackAllocator&);
		CDXStackAllocator& operator=(const CDXStackAllocator&);

	public:
		CDXStackAllocator();
		~CDXStackAllocator();
	};

} // DXFramework namespace end