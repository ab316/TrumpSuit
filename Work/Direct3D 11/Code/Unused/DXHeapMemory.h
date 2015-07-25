#pragma once

namespace DXFramework
{ // DXFramework namespace begin

	class CDXHeapMemory
	{
	public:
		static	void*	Malloc(uintptr_t uiSize)
		{
			return malloc(uiSize);
		}

		static	void*	AlignedMalloc(uintptr_t uiSize, UINT8 uiAlignment)
		{
			return _aligned_malloc(uiSize, uiAlignment);
		}

		static void*	Realloc(void* p, uintptr_t uiNewSize)
		{
			return realloc(p, uiNewSize);
		}

		static void*	AlignedRealloc(void* p, uintptr_t uiNewSize, UINT8 uiAlignment)
		{
			return _aligned_realloc(p, uiNewSize, uiAlignment);
		}

		static void		Free(void* p)
		{
			free(p);
		}

		static	void	AlignedFree(void* p)
		{
			_aligned_free(p);
		}

	private:
		CDXHeapMemory();
		~CDXHeapMemory();
	};

} // DXFramework namespace end