#include "DXStdafx.h"
#include "DXAllocator.h"


namespace DXFramework
{ // DXFramework namespace begin

	IDXBaseAllocator::IDXBaseAllocator()
	{
#if defined (DX_ALLOCATOR_DEBUG)
		m_wstrDebugName.reserve(DX_MAX_NAME+1);
		m_wstrDebugName.assign(L"UNSPECIFIED");
#endif
	}


	IDXBaseAllocator::~IDXBaseAllocator()
	{
	}


	void DXDumpAllocatorMemoryStatus(IDXBaseAllocator* pAllocator)
	{
		DXAllocatorMemoryStatus dump /*= pAllocator->GetStatus()*/;

		typedef std::vector<DXAllocatorMemoryBlockStatus>::iterator BlockIterator;
		BlockIterator iter = dump.vecMemoryBlocks.begin();
		BlockIterator end = dump.vecMemoryBlocks.end();

		DX_DEBUG_OUTPUT2(L"ALLOCATOR NAME: %s.\n\tDump Begin.\n------------------------------------------------------\n\tAllocations: %u.", pAllocator->GetDebugName(), dump.vecMemoryBlocks.size());
		
		while (iter != end)
		{
			DX_DEBUG_OUTPUT(L"FILE: %s, LINE: %u, ALLOCATED: %u bytes, REQUESTED: %u bytes.", iter->szFile, iter->dwLine, iter->uiAllocationSize, iter->uiRequestedAllocationSize);
			++iter;
		}

		DX_DEBUG_OUTPUT(L"SUMMARY:\n\tTotal Allocator Memory: %u bytes.\n\tMemory Allocated: %u bytes\n\tMemory Requested: %u bytes\n\tBook Keeping Memory: %u bytes\n\tDebug Memory: %u bytes.", dump.uiTotalMemory, dump.uiAllocatedMemory, dump.uiRequestedMemory, dump.uiBookKeepingMemory, dump.uiDebugMemory);
		DX_DEBUG_OUTPUT(L"\n------------------------------------------------------\n\tDump End.\n");
	}

} // DXFramework namespace end