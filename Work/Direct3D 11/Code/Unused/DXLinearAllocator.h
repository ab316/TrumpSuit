#pragma once

#include "DXAllocator.h"

namespace DXFramework
{ // DXFramework namespace begin

	// A linear allocator allows allocations of any size to be made. Individual deallcations
	// can't be made however.
	class CDXLinearAllocator : public IDXAllocator
	{
	private:
		// The begining of the memory block.
		void*		m_pInitialPosition;
		// Current position in the memory block.
		void*		m_pCurrentPosition;
		// The size of the memory block.
		uintptr_t	m_uiSize;

	public:
		virtual	void*		Allocate(uintptr_t uiSize, U8 uiAlignment);
		virtual void		Deallocate(void* p);

	public:
		// Clears the memory.
		void				Clear();

		// No copying allowed.
	private:
		CDXLinearAllocator(const CDXLinearAllocator&)					{ };
		CDXLinearAllocator& operator=(const CDXLinearAllocator&)		{ };

	public:
		CDXLinearAllocator(uintptr_t uiSize, void* pStart);
		~CDXLinearAllocator();
	};

} // DXFramework namespace end