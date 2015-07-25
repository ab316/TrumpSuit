#pragma once

#include "DXAllocator.h"
#include <unordered_map>

namespace DXFramework
{ // DXFramework namespace begin

	#define DX_DEFAULT_ALLOCATOR_GROW_THRESHOLD		1024	// 1K

	class CDXTrackingPointer;

	// A DXGrowableAllocator can allocated more memory if needed from its parent allocator.
	// 1- All pointers that point to the memory allocated by a growable allocated must register
	// for update and unregister before they become invalid.
	// 2- The memory pointer provided to a growable allocator during initialization may change
	// internally. Therefore the original pointer may become invalid. To deallocate the memory
	// block of the parent allocator pass the return value GetMemoryStart() accessor to the 
	// parent's deallocate method.
	// 3- Derived classes should implement functionality in the AfterMemoryResize() method
	// to make any changes necessary if the memory block is moved after growing.
	// 4- Grow method is a costly operation so it should be avoided generally. Allocate
	// enough memory initially and set the threshold appropriately so Grow is hardly ever
	// required.
	class IDXGrowableAllocator : public IDXBaseAllocator
	{
		typedef std::unordered_map<UINT32, void**>	DXRegisteredPointersMap;
		typedef DXRegisteredPointersMap::iterator	DXRegisteredPointersMapIterator;
		typedef std::unordered_map<UINT32, CDXTrackingPointer*> DXTrackedPointersMap;
		typedef DXTrackedPointersMap::iterator	DXTrackedPointersMapIterator;
		//typedef std::unordered_map<UINT32, CDXTrackingPointer<void>*>	DXTrackedPointersMap;
		//typedef DXTrackedPointersMap::iterator							DXTrackedPointerMapIterators;

	private:
		// The registered pointers.
		DXRegisteredPointersMap				m_mapppRegisteredPointers;
		//DXTrackedPointersMap				m_mapppTrackedPointersToBlocks;
		// Iterators for the map.
		DXRegisteredPointersMapIterator		m_iterBegin;
		DXRegisteredPointersMapIterator		m_iterEnd;
		// The parent memory allocator which supplies memory to this allocator.
		IDXBaseAllocator*					m_pParentAllocator;
		// Default: DX_DEFAULT_ALLOCATOR_GROW_THRESHOLD.
		uintptr_t							m_uiMemoryGrowThreshold;
		UINT32								m_uiNextAvailableId;
		// Is the allocator growable? Default: false.
		bool								m_bResizeable;

	protected:
		// Derived class must set this during initialization and never play with it again!
		void*								m_pMemoryStart;
		// Size of memory.
		uintptr_t							m_uiSize;


	// These method carried on from IDXBaseAllocator are to be defined by the derived class.
	public:
#if defined (DX_ALLOCATOR_DEBUG)
		virtual	void*	Allocate(uintptr_t uiSize, WCHAR* szFile, DWORD dwLine)			= 0;
		virtual void*	AllocateAligned(uintptr_t uiSize, UINT8 uiAlign, WCHAR* szFile, DWORD dwLine)	= 0;
		virtual DXAllocatorMemoryStatus	GetStatus()										= 0;
#else
		virtual	void*	Allocate(uintptr_t uiSize)										= 0;
		virtual void*	AllocateAligned(uintptr_t uiSize, UINT8 uiAlign)				= 0;
#endif

		virtual void	Deallocate(void* p)												= 0;
		virtual void	DeallocateAligned(void* p)										= 0;
		virtual void*	Reallocate(void* pMemory, uintptr_t uiNewSize)					= 0;
		virtual void*	ReallocateAligned(void* pMemory, uintptr_t uiNewSize, UINT8 uiAlignment) = 0;
		// This virtual is method declared in this class. The derived class should define it to perform
		// any required post-resize tasks.
		// pDelta is the absolute difference b/w the new start of the allocator
		// memory and the previous start.
		// bAddressIncreased is true if the delta is to be added to the old address.
		virtual	void	AfterMemoryResize(uintptr_t uiDelta, bool bAddressIncreased)	= 0;


	public:
		// Registers a pointer variable for update so that if the memory block is moved
		// the variable is updated by the allocated to the correct new address.
		UINT32	RegisterPointerForUpdate(void** pp);
		//template <class T> UINT32	RegisterPointerForUpdate(CDXTrackingPointer<T>* pp);
		// Unregisters an already registered pointer variable. The id accepting overload
		// is prefered over this one.
		void	UnregisterPointer(void** pp);
		// Grows the memory block atleast by given size.
		bool	Grow(uintptr_t uiIncrease);
		// Not for now. Shrinking is difficult. What to do with the pointers pointing
		// outside the block after shrinking???
		/*bool	Shrink(uintptr_t uiAmount);*/

	private:
		// Updates the registered pointers to point to the correct addresses after
		// memory block has moved.
		void			UpdatePointers(uintptr_t uiDelta, bool bIncrease);
		inline	void	UpdateIterators()
		{
			m_iterBegin = m_mapppRegisteredPointers.begin();
			m_iterEnd = m_mapppRegisteredPointers.end();
		}

	public:
		inline	void	SetResizeable(bool bResizeable)			{ m_bResizeable = bResizeable;	};
		inline	void	SetMemoryGrowThreshold(uintptr_t ui)	{ m_uiMemoryGrowThreshold = ui;	};
		inline	bool	IsResizeable()		const				{ return m_bResizeable;			};
		inline	void*	GetMemoryStart()	const				{ return m_pMemoryStart;		};


		void IDXGrowableAllocator::UnregisterPointer(UINT id)
		{
			m_mapppRegisteredPointers.erase(id);
		}

		// If the address of the registered pointer variable itself changes (possible because
		// the pointer variable was itself in an allocated block which has now moved) then this
		// method should be called to update.
		inline	void	UpdateStoredPointedAddress(UINT32 id, void* pNewAddress)
		{
			DXRegisteredPointersMapIterator iter = m_mapppRegisteredPointers.find(id);
			if (iter != m_iterEnd)
			{
				*iter->second = pNewAddress;
			}
		}

		// Returns true if the given pointer variable is registered in the allocator for update.
		inline	bool	PointerRegistered(void** pp)	const
		{
			DXRegisteredPointersMapIterator iter = m_iterBegin;
			while (iter != m_iterEnd)
			{
				if (iter->second == pp)
				{
					return true;
				}

				++iter;
			}

			return false;
		}

		inline	bool	PointerRegistered(UINT32 id)
		{
			DXRegisteredPointersMapIterator iter = m_mapppRegisteredPointers.find(id);
			return (iter != m_iterEnd);
		}


	public:
		// Changing the parent allocator after construction would cause chaos so it can
		// only be set in ctor.
		IDXGrowableAllocator(IDXBaseAllocator* pParentAllocator);
		virtual ~IDXGrowableAllocator();
	};

} // DXFramework namespace end