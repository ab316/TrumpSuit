#pragma once

#include "DXAllocator.h"

namespace DXFramework
{ // DXFramework namespace begin.

	// This allocator allocator memory from the heap. This allocator is meant to be top most
	// level parent allocator for other growable allocators and is not meant to be used directly
	// for allocations.
	class CDXHeapAllocator : public IDXBaseAllocator
	{
	private:
		uintptr_t	m_uiUsedMemory;
		uintptr_t	m_uiTotalMemory;
		uintptr_t	m_uiNumAllocations;

	public:
		virtual	uintptr_t	GetUsedMemory()			{ return m_uiUsedMemory;		};
		virtual	uintptr_t	GetTotalMemory()		{ return m_uiTotalMemory;		};
		virtual	uintptr_t	GetNumAllocations()		{ return m_uiNumAllocations;	};
		virtual	void*		GetMemoryStart()		{ return 0;						};

		virtual	void*	Allocate(uintptr_t uiSize)
		{
			// Non-aligned memory can be said to be 1-byte aligned.
			return AllocateAligned(uiSize, 1);
		}

		virtual void*	AllocateAligned(uintptr_t uiSize, UINT8 uiAlign)
		{
#if defined (DX_ALLOCATOR_DEBUG)
			DXAlignmentValid(uiAlign);
#endif
			if (uiSize)
			{
				uiSize += uiAlign;
				void* p = _aligned_malloc(uiSize, uiAlign);
				if (p)
				{
					// Get a byte pointer to the memory.
					unsigned char* a = (unsigned char*)p + uiAlign - 1;
					// Store the alignment a byte before the actual memory.
					*a = uiAlign;
					++a;
					m_uiUsedMemory += uiSize;
					++m_uiNumAllocations;

					return a;
				}
			}
			return nullptr;
		}

		virtual void	Deallocate(void* p)
		{
			DeallocateAligned(p);
		}

		virtual void	DeallocateAligned(void* p)
		{
			if (p)
			{
				BYTE* pAlignment = (BYTE*)p - 1;
				BYTE* pStart = (BYTE*)p - *pAlignment;
				m_uiUsedMemory -= _aligned_msize(pStart, *pAlignment, 0);
				_aligned_free(pStart);
				--m_uiNumAllocations;
			}
		}

		virtual void*	Reallocate(void* pMemory, uintptr_t uiNewSize)
		{
			return ReallocateAligned(pMemory, uiNewSize, 1);
		}

		virtual void*	ReallocateAligned(void* pMemory, uintptr_t uiNewSize, UINT8 uiAlignment)
		{
			if (pMemory)
			{
				BYTE* pAlignment = (BYTE*)pMemory - 1;
				BYTE* pStart = (BYTE*)pMemory - *pAlignment;

				uintptr_t uiOldSize = _aligned_msize(pStart, *pAlignment, 0);

				uiNewSize += uiAlignment;
				pMemory = _aligned_realloc(pStart, uiNewSize, uiAlignment);

				if (pMemory)
				{
					pMemory = (void*)( (uintptr_t)pMemory + uiAlignment );
					pAlignment = (BYTE*)pMemory - 1;
					*pAlignment = uiAlignment;

					bool bIncrease = (uiNewSize > uiOldSize) ? true : false;
					uintptr_t uiSizeDiff = bIncrease ? (uiNewSize - uiOldSize) : (uiOldSize - uiNewSize);

					if (bIncrease)
					{
						m_uiUsedMemory += uiSizeDiff;
					}
					else
					{
						m_uiUsedMemory -= uiSizeDiff;
					}

					if (!uiNewSize)
					{
						--m_uiNumAllocations;
					}
				}
			}

			return pMemory;
		}

#if defined (DX_ALLOCATOR_DEBUG)
		virtual	void*	Allocate(uintptr_t uiSize, WCHAR* szFile, DWORD dwLine)
		{
			UNREFERENCED_PARAMETER(szFile);
			UNREFERENCED_PARAMETER(dwLine);

			return Allocate(uiSize);
		}

		virtual void*	AllocateAligned(uintptr_t uiSize, UINT8 uiAlign, WCHAR* szFile, DWORD dwLine)
		{
			UNREFERENCED_PARAMETER(szFile);
			UNREFERENCED_PARAMETER(dwLine);

			return AllocateAligned(uiSize, uiAlign);
		}

		virtual DXAllocatorMemoryStatus	GetStatus()
		{
			DX_DEBUG_OUTPUT1(L"Dump not supported. ALLOCATOR NAME: %s.", m_wstrDebugName.data());
			return DXAllocatorMemoryStatus();
		}
#endif

	public:
		CDXHeapAllocator()	:	m_uiTotalMemory((uintptr_t)-1)
		{
		}

		~CDXHeapAllocator()
		{
		}
	};

} // DXFramework namespace end.