#include "DXStdafx.h"
#include "DXMemoryManager.h"

namespace DXFramework
{ // DXFramework namespace begin

#define DX_ALLOCATION_HEADER_SIZE	sizeof(DXAllocationHeader)
#define DX_ALLOCATION_DEBUG_SIZE	DX_ALLOCATION_HEADER_SIZE
#define DX_MEMORY_INVALID_ID		((UINT32)-1)


	CDXMemoryManager::CDXMemoryManager()	:	m_uiNumFreeLists(0),
												m_uiNumPools(0),
												m_uiNumStacks(0),
												m_uiTotalMemory(0)
	{
	}


	CDXMemoryManager::~CDXMemoryManager()
	{
		auto poolIter = m_mappPools.begin();
		auto poolEnd = m_mappPools.end();

		while (poolIter != poolEnd)
		{
			if (poolIter->second)
			{
				m_Heap.Deallocate(poolIter->second->GetMemoryStart());
				poolIter->second->~CDXPoolAllocator();
				m_Heap.Deallocate(poolIter->second);
			}
			++poolIter;
		}


		auto stackIter = m_mappStacks.begin();
		auto stackEnd = m_mappStacks.end();
		while (stackIter != stackEnd)
		{
			if (stackIter->second)
			{
				m_Heap.Deallocate(stackIter->second->GetMemoryStart());
				stackIter->second->~CDXStackAllocator();
				m_Heap.Deallocate(stackIter->second);
			}
			++stackIter;
		}


		//auto freeIter = m_mappFreeLists.begin();
		//auto freeEnd = m_mappFreeLists.end();
		//while (freeIter != freeEnd)
		//{
		//	if (freeIter->second)
		//	{
		//		m_Heap.Deallocate(freeIter->second->GetMemoryStart());
		//		delete freeIter->second;
		//	}
		//	++freeIter;
		//}
	}


	DX_DECLARE_METHOD(CDXPoolAllocator*, CDXMemoryManager, DX_DECL_MEM_MAN_ADD_POOL)
	{
		CDXPoolAllocator* pPool = nullptr;
		void* pMemory = nullptr;

		// Create a pool.
#if defined(DX_MEMORY_DEBUG)
		pPool = new(m_Heap.Allocate(sizeof(CDXPoolAllocator), __FILEW__, __LINE__)) CDXPoolAllocator();
#else
		pPool = new(m_Heap.Allocate(sizeof(CDXPoolAllocator))) CDXPoolAllocator();
#endif
	
		if (pPool)
		{
			// Insert the pool into the map.
			auto x = m_mappPools.insert(DXPoolMapValue(m_uiNumPools, pPool));
			
			// Calculate the pool memory.
			uintptr_t uiMemorySize = uiObjectSize;

#if defined (DX_MEMORY_DEBUG)
			uiMemorySize += DX_ALLOCATION_DEBUG_SIZE;
#endif

			uiMemorySize = (uiMemorySize * uiNumObjects) + uiAlignment;


			// Allocate the memory.
#if defined (DX_MEMORY_DEBUG)
			pMemory = m_Heap.Allocate(uiMemorySize, szFile, dwLine);
#else
			pMemory = m_Heap.Allocate(uiMemorySize);
#endif
			
			// Memory allocated?
			if (pMemory)
			{

#if defined (DX_MEMORY_DEBUG)
				// Calculate the size of the debug data for each allocation.
				uintptr_t uiDebugSize = DX_ALLOCATION_DEBUG_SIZE / uiAlignment;
				if (DX_ALLOCATION_DEBUG_SIZE % uiAlignment)
				{
					++uiDebugSize;
				}
				uiDebugSize *= uiAlignment;

				// The object size now also includes the debug size.
				uiObjectSize += uiDebugSize;
#endif

				// Initialize the pool.
				x.first->second->Initialize(uiObjectSize, uiAlignment, uiMemorySize, pMemory);

				++m_uiNumPools;

				return pPool;
			}
		}

		// This point is reached on memory allocation failure.
		if (pMemory)
		{
			m_Heap.Deallocate(pMemory);
		}

		if (pPool)
		{
			m_mappPools.erase(m_uiNumPools);
			delete pPool;
		}

		DX_DEBUG_OUTPUT0(L"Unable to allocate sufficient memory.");

		return nullptr;
	}


	DX_DECLARE_METHOD(CDXStackAllocator*, CDXMemoryManager, DX_DECL_MEM_MAN_ADD_STACK)
	{
		CDXStackAllocator* pStack = nullptr;
		void* pMemory = nullptr;

		// Create a stack.
#if defined (DX_MEMORY_DEBUG)
		pStack = new(m_Heap.Allocate(sizeof(CDXStackAllocator), szFile, dwLine)) CDXStackAllocator();
#else
		pStack = new(m_Heap.Allocate(sizeof(CDXStackAllocator))) CDXStackAllocator();
#endif

		if (pStack)
		{
			// Insert the pool into the map.
			auto x = m_mappStacks.insert(DXStackMapValue(m_uiNumStacks, pStack));

#if defined (DX_MEMORY_DEBUG)
			// Add additional memory if we are in debug mode..
			uiMemorySize += (uiMemorySize / DX_ALLOCATION_DEBUG_SIZE);
			void* pMemory = m_Heap.Allocate(uiMemorySize, szFile, dwLine);
#else
			void* pMemory = m_Heap.Allocate(uiMemorySize);
#endif

			// Allocate the memory.
			
			// Memory allocated?
			if (pMemory)
			{
				// Initialize the pool.
				x.first->second->Initialize(uiMemorySize, pMemory);

				return pStack;
			}
		}

		// This point is reached on memory allocation failure.
		if (pMemory)
		{
			m_Heap.Deallocate(pMemory);
		}

		if (pStack)
		{
			m_mappStacks.erase(m_uiNumStacks);
			delete pStack;
		}

		DX_DEBUG_OUTPUT0(L"Unable to allocate sufficient memory.");

		return nullptr;
	}


	DX_DECLARE_METHOD(void*, CDXMemoryManager, DX_DECL_MEM_MAN_ALLOCATE_FROM_POOL)
	{
		// Get the pool.
		void* p = pPool->Allocate(pPool->m_uiObjectSize);

		// Fill the header.
		DXAllocationHeader* pHeader = (DXAllocationHeader*)p;
		pHeader->pAllocator = pPool;
		pHeader->allocatorType = DX_ALLOCATOR_TYPE_POOL;

#if defined(DX_MEMORY_DEBUG)
		pHeader->dwLine = dwLine;
		wcscpy_s(pHeader->szFileName, DX_MAX_FILE_NAME+1, wcsrchr(szFile, '\\'));
#endif

		// Return the memory address after the header.
		return (void*)(++pHeader);
	}


	DX_DECLARE_METHOD(void*, CDXMemoryManager, DX_DECL_MEM_MAN_ALLOCATE_FROM_STACK)
	{
		// Allocate memory.
		void* p = pStack->Allocate(uiSize);

		// Fill the header.
		DXAllocationHeader* pHeader = (DXAllocationHeader*)p;
		pHeader->pAllocator = pStack;
		pHeader->allocatorType = DX_ALLOCATOR_TYPE_STACK;

#if defined(DX_MEMORY_DEBUG)
		pHeader->dwLine = dwLine;
		wcscpy_s(pHeader->szFileName, DX_MAX_FILE_NAME+1, wcsrchr(szFile, '\\'));
#endif

		// Return the memory address after the header.
		return (void*)(++pHeader);
	}

} // DXFramework namespace end.