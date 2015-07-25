#pragma once

#pragma warning (disable : 4127)

#include <new>

#if defined(DX_DEBUG)

#define DX_ALLOCATOR_DEBUG
#define DX_SET_ALLOCATOR_DEBUG_NAME(a, wstr) (a).SetDebugName(wstr)
#define DX_ALLOCATE(alloc, size)								alloc.Allocate(size, __FILEW__, __LINE__)
#define DX_ALLOCATE_NEW(alloc, type)							alloc.AllocateNew<type>(__FILEW__, __LINE__)
#define DX_ALLOCATE_ALIGNED(alloc, size, align)					alloc.AllocateAligned(size, align, __FILEW__, __LINE__)
#define DX_ALLOCATE_NEW_ALIGNED(alloc, type)					alloc.AllocateNewAligned<type>(__FILEW__, __LINE__)
#define DX_ALLOCATE_ARRAY(alloc, length, type)					alloc.AllocateArray<type>(length, __FILEW__, __LINE__)
#define DX_ALLOCATE_ARRAY_ALIGNED(alloc, length, align, type)	alloc.AllocateArrayAligned<type>(length, align, __FILEW__, __LINE__)

#else

#define DX_ALLOCATE(alloc, size)						alloc.Allocate(size)
#define DX_ALLOCATE_NEW(alloc, type)					alloc.AllocateNew<type>()
#define DX_ALLOCATE_ALIGNED(alloc, size, align)			alloc.AllocateAligned(size, align)
#define DX_ALLOCATE_NEW_ALIGNED(alloc, type)			alloc.AllocateNewAligned<type>()
#define DX_SET_ALLOCATOR_DEBUG_NAME(a, wstr)

#endif


#define DX_DEALLOCATE(alloc, p)							alloc.Deallocate(p)
#define DX_DEALLOCATE_DELETE(alloc, p, type)			alloc.DeallocateDelete<type>(p)
#define DX_DEALLOCATE_ALIGNED(alloc, p)					alloc.DeallocateAligned(p)
#define DX_DEALLOCATE_DELETE_ALIGNED(alloc, p, type)	alloc.DeallocateDeleteAligned<type>(p)
#define DX_DEALLOCATE_ARRAY(alloc, p, type)				alloc.DeallocateArray<type>(p)
#define DX_DEALLOCATE_ARRAY_ALIGNED(alloc, p, type)		alloc.DeallocateArrayAligned<type>(p)


namespace DXFramework
{ // DXFramework namespace end

	struct DXAllocatorMemoryBlockStatus
	{
		uintptr_t	uiAllocationSize;
		uintptr_t	uiRequestedAllocationSize;
		DWORD		dwLine;
		WCHAR		szFile[DX_MAX_FILE_NAME+1];
	};

	struct DXAllocatorMemoryStatus
	{
		// Total memory allocated by the allocator.
		uintptr_t		uiTotalMemory;
		// Allocated memory.
		uintptr_t		uiAllocatedMemory;
		// The actual total memory that was requested.
		uintptr_t		uiRequestedMemory;
		// Necessary additional memory required for each allocation.
		uintptr_t		uiBookKeepingMemory;
		// Memory only allocated in debug builds.
		uintptr_t		uiDebugMemory;
		// Individual memory blocks.
		std::vector<DXAllocatorMemoryBlockStatus>	vecMemoryBlocks;
	};

	class IDXBaseAllocator;
	// Provide the return value of DumpMemory
	void			DXDumpAllocatorMemoryStatus(IDXBaseAllocator* pAllocator);

	// Returns true, if the given alignment is valid.
	inline bool		DXAlignmentValid(UINT8 uiAlignment)
	{
		if (!uiAlignment)
		{
			return false;
		}
		else
		{
			while (uiAlignment > 1)
			{
				if (uiAlignment % 2)
				{
					return false;
				}
				uiAlignment /= 2;
			}
		}

		return true;
	}


	// Aligns the address. uiAlignment must be power of 2.
	inline void*	DXNextAlignedAddress(void* pAddress, UINT8 uiAlignment)
	{
		// To n byte align a memory address x, we need to mask off log2(n) least sig bits of x.
		// Only masking the bits would return the first n-byte aligned address before x. To find
		// the first aligned address after x, we add (alignment - 1) and mask that address.
		// Suppose uiAlignment is 4. Its bitfield will be 0b00000100. Subtracting 1 would give
		// 0b00000011. Now complementing this gives 0b11111100. ANDing this with the given address
		// yields 0bxxxxxx00. Which is the 4 byte alignment address.
		return (void*)( ((uintptr_t)pAddress + (uiAlignment - 1)) & ~(uiAlignment - 1) );
	}


	// Returns the numbers of bytes with which the given address is to be adjusted to align it
	// correctly.
	inline UINT8 DXAlignAdjustment(void *pAddress, UINT8 uiAlignment)
	{
		UINT8 uiAdjustment = uiAlignment - ( (uintptr_t)pAddress & (uiAlignment - 1) );
		if (uiAdjustment == uiAlignment)
		{
			return 0;
		}

		return uiAdjustment;
	}


	// Gives the numbers of bytes with which the given address is to be adjusted to align it
	// correctly and leave enough space for the header before it.
	inline UINT8 DXAlignAdjustmentWithHeader(void* pAddress, UINT8 uiAlignment, UINT8 uiHeaderSize)
	{
		// Find the adjustment to align the address.
		UINT8 uiAdjustment = DXAlignAdjustment(pAddress, uiAlignment);

		// Now we need space for the header.
		UINT8 uiNeededSpace = uiHeaderSize;

		// If the bytes added for alignment are not sufficient to hold the header size.
		if (uiAdjustment < uiNeededSpace)
		{
			// Find out how much more space do we need.
			uiNeededSpace -= uiAdjustment;
			// Find out the new adjustment. First a whole number of uiAlignment bytes are
			// added to the adjustment needed. This is needed if the needed space is more than the
			// alignment. E.g needed space is 11 bytes. Alignment is 4 bytes. We add 8 bytes to the
			// adjustment. Now 3 bytes are still needed.
			uiAdjustment += uiAlignment * (uiNeededSpace / uiAlignment);

			// Then if the needed space is not a multiple of alignment, we add one more alignment
			// bytes to make enough room for the header.
			// E.g we add 4 more bytes to make room for the header. This leaves us with 1 wasted byte.
			if ( (uiNeededSpace % uiAlignment) > 0 )
			{
				uiAdjustment += uiAlignment;
			}
		}

		return uiAdjustment;
	}


	// The interface class for memory allocators.
	class IDXAllocator
	{
	public:
		virtual	uintptr_t	GetUsedMemory()			= 0;
		virtual	uintptr_t	GetNumAllocations()		= 0;

		// Methods in this block are to be defined in derived classes.
	public:
		virtual	void*	Allocate(uintptr_t uiSize)								= 0;
		virtual void*	AllocateAligned(uintptr_t uiSize, UINT8 uiAlign)		= 0;
		virtual void	Deallocate(void* p)										= 0;
		virtual void	DeallocateAligned(void* p)								= 0;
		//virtual DXAllocatorMemoryStatus	GetStatus()								= 0;

	public:
		template <class T> void		DeallocateDelete(T* p)						= 0;
		template <class T> void		DeallocateDeleteAligned(T* p)				= 0;
		template <class T> void		DeallocateArray(T* p)						= 0;
		template <class T> void		DeallocateArrayAligned(T* p)				= 0;

#if defined (DX_ALLOCATOR_DEBUG)
		template <class T> T*		AllocateNew(WCHAR* szFile, DWORD dwLine)			= 0;
		template <class T> T*		AllocateNewAligned(WCHAR* szFile, DWORD dwLine)		= 0;
		template <class T> T*		AllocateArray(uintptr_t uiLength, WCHAR szFile, DWORD dwLine)			= 0;
		template <class T> T*		AllocateArrayAligned(uintptr_t uiLength, WCHAR szFile, DWORD dwLine)	= 0;
#else
		template <class T> T*		AllocateNew()								= 0;
		template <class T> T*		AllocateNewAligned()						= 0;
		template <class T> T*		AllocateArray(uintptr_t uiLength)			= 0;
		template <class T> T*		AllocateArrayAligned(uintptr_t uiLength)	= 0;
#endif

	};



	// The base class for memory allocators.
	class IDXBaseAllocator	:	public IDXAllocator
	{
		friend class CDXMemoryManager;

	protected:
#if defined (DX_ALLOCATOR_DEBUG)
		// Don't waste space for debug name in release builds.
		std::wstring	m_wstrDebugName;
#endif

		//Book keeping methods.
	public:
		virtual	uintptr_t	GetUsedMemory()			= 0;
		virtual	uintptr_t	GetTotalMemory()		= 0;
		virtual	uintptr_t	GetNumAllocations()		= 0;
		virtual void*		GetMemoryStart()		= 0;

#if defined (DX_ALLOCATOR_DEBUG)
		public:
			// Max allowed length is DX_MAX_NAME.
		void		SetDebugName(WCHAR* szName)	{ m_wstrDebugName.assign(szName);	};
		const WCHAR*GetDebugName() const		{ return m_wstrDebugName.data();	};
#endif

		// The meat of this interface.
	public:
		/// Allocates a memory block of given size.
		virtual	void*		Allocate(uintptr_t uiSize)								= 0;
		// Allocates a memory block of given size with given alignment.
		virtual	void*		AllocateAligned(uintptr_t uiSize, UINT8 uiAlignment)	= 0;
		// The given pointer must point to the begining of memory block allocated using Allocate()
		// of the same allocator.
		virtual void		Deallocate(void* p)										= 0;
		// Use only with memory block that was previously allocated with AllocateAligned().
		virtual void		DeallocateAligned(void* p)								= 0;
		//virtual DXAllocatorMemoryStatus		GetStatus()	= 0;

	public:

		template <class T> T*	AllocateNew()
		{
			// Use the placement form of new.
			return new (Allocate(sizeof(T))) T;
		}

		template <class T> T*	AllocateNewAligned()
		{
			// Use the placement form of new.
			return new (AllocateAligned(sizeof(T), __alignof(T))) T;
		}

		template <class T> T*	AllocateArray(uintptr_t uiLength)
		{
			if (uiLength == 0)
			{
				return nullptr;
			}

			// Find the header size in units of sizeof(T). We working with a pointer of type 
			// T* we can only move by multiples of sizeof(T) bytes in memory. So we need a header 
			// size of multiple of sizeof(T). Actually our header's real size is 4/8 bytes. As we can
			// only move by sizeof(T) bytes. We need appropriate header size.
			uintptr_t uiHeaderSize = sizeof(uintptr_t) / sizeof(T);
			// In case sizeof(T) is less than 4 bytes or is not 2, then sizeof(uintptr_t) / sizeof(T)
			// will not be a whole number. So we need to increase header size by 1 unit.
			if ( (sizeof(uintptr_t) % sizeof(T)) > 0)
			{
				uiHeaderSize += 1;
			}
			// Remember: uiHeader size is in terms of sizeof(T).

			// Allocate enough memory for the array and the header.
			T* p = ( (T*)Allocate(sizeof(T) * (uiLength + uiHeaderSize)) );
			if (p == nullptr)
			{
				return nullptr;
			}

			// The first sizeof(T) bytes are for the header. So move ahead by header size. p now
			// points to the begining of the array.
			p += uiHeaderSize;

			// Cast the pointer as uintptr_t pointer so that we can move by 4/8 bytes.
			uintptr_t* pHeader = (uintptr_t*)p;
			// Move back 4 bytes from the begining of the array.
			--pHeader;
			// Store the length of the array in these bytes.
			*pHeader = uiLength;

			// Now "new" each element in the array with the placement new operator which only
			// initializes the given memory properly for the object (Doesn't allocate memory).
			for (uintptr_t i=0; i<uiLength; ++i)
			{
				new (&p[i]) T;
			}

			// The the pointer to the begining of the array.
			return p;
		}

		template <class T> T*	AllocateArrayAligned(uintptr_t uiLength)
		{
			if (uiLength == 0)
			{
				return nullptr;
			}

			// Find the header size in units of sizeof(T). We working with a pointer of type 
			// T* we can only move by multiples of sizeof(T) bytes in memory. So we need a header 
			// size of multiple of sizeof(T). Actually our header's real size is 4/8 bytes. As we can
			// only move by sizeof(T) bytes. We need appropriate header size.
			uintptr_t uiHeaderSize = sizeof(uintptr_t) / sizeof(T);
			// In case sizeof(T) is less than 4 bytes or is not 2, then sizeof(uintptr_t) / sizeof(T)
			// will not be a whole number. So we need to increase header size by 1 unit.
			if ( (sizeof(uintptr_t) % sizeof(T)) > 0)
			{
				uiHeaderSize += 1;
			}
			// Remember: uiHeader size is in terms of sizeof(T).

			// Allocate enough memory for the array and the header.
			T* p = ( (T*)AllocateAligned( sizeof(T) * (uiLength + uiHeaderSize), __alignof(T) ) );
			if (p == nullptr)
			{
				return nullptr;
			}

			// The first sizeof(T) bytes are for the header. So move ahead by header size. p now
			// points to the begining of the array.
			p += uiHeaderSize;

			// Cast the pointer as uintptr_t pointer so that we can move by 4/8 bytes.
			uintptr_t* pHeader = (uintptr_t*)p;
			// Move back 4 bytes from the begining of the array.
			--pHeader;
			// Store the length of the array in these bytes.
			*pHeader = uiLength;

			// Now "new" each element in the array with the placement new operator which only
			// initializes the given memory properly for the object (Doesn't allocate memory).
			for (uintptr_t i=0; i<uiLength; ++i)
			{
				new (&p[i]) T;
			}

			// The the pointer to the begining of the array.
			return p;
		}

	public:
		template <class T> void		DeallocateDelete(T* p)
		{
			if (p)
			{
				p->~T();
				Deallocate(p);
			}
		}

		template <class T> void		DeallocateDeleteAligned(T* p)
		{
			if (p)
			{
				p->~T();
				DeallocateAligned(p);
			}
		}


		template <class T>	void	DeallocateArray(T* p)
		{
			// Must be valid pointer.
			if (p == 0)
			{
				return;
			}

			// Cast the pointer as uintptr_t pointer so we can move by 4/8 bytes.
			uintptr_t* pHeader = (uintptr_t*)p;
			// Move back 4 bytes from the begining of the array.
			--pHeader;
			// Get the length of the array.
			uintptr_t uiLength = *pHeader;

			// Call the dtor on the array elements.
			for (int i=uiLength-1; i>=0; --i)
			{
				p[i].~T();
			}

			// Calculate the header size as in AllocateArray().
			uintptr_t uiHeaderSize = sizeof(uintptr_t) / sizeof(T);
			if ( (sizeof(uintptr_t) % sizeof(T)) > 0 )
			{
				uiHeaderSize += 1;
			}

			// Deallocate the array memory including the header.
			Deallocate(p - uiHeaderSize);
		}

		template <class T>	void	DeallocateArrayAligned(T* p)
		{
			// Must be valid pointer.
			if (p == 0)
			{
				return;
			}

			// Cast the pointer as uintptr_t pointer so we can move by 4/8 bytes.
			uintptr_t* pHeader = (uintptr_t*)p;
			// Move back 4 bytes from the begining of the array.
			--pHeader;
			// Get the length of the array.
			uintptr_t uiLength = *pHeader;

			// Call the dtor on the array elements.
			for (int i=uiLength-1; i>=0; --i)
			{
				p[i].~T();
			}

			// Calculate the header size as in AllocateArray().
			uintptr_t uiHeaderSize = sizeof(uintptr_t) / sizeof(T);
			if ( (sizeof(uintptr_t) % sizeof(T)) > 0 )
			{
				uiHeaderSize += 1;
			}

			// Deallocate the array memory including the header.
			DeallocateAligned(p - uiHeaderSize);
		}


	public:
		IDXBaseAllocator();
		virtual	~IDXBaseAllocator();
	};

} // DXFramework namespace end