#pragma once

#include <unordered_map>
#include "DXHeapAllocator.h"
#include "DXStackAllocator.h"
#include "DXPoolAllocator.h"

namespace DXFramework
{ // DXFramework namespace begin


#if defined(DX_DEBUG)
#define DX_MEMORY_DEBUG
#endif

#define DX_DECLARE_METHOD(returnType, className, nameArgs) returnType className::nameArgs


	class CDXHeapAllocator;
	class CDXPoolAllocator;
	class CDXFreeListAllocator;
	class CDXStackAllocator;

	// The framework's memory manager.
	class CDXMemoryManager
	{
		typedef std::map<UINT32, CDXPoolAllocator*>		DXPoolMap;
		typedef std::map<UINT32, CDXFreeListAllocator*>	DXFreeListMap;
		typedef std::map<UINT32, CDXStackAllocator*>	DXStackMap;
		typedef DXPoolMap::value_type					DXPoolMapValue;
		typedef DXFreeListMap::value_type				DXFreeListMapValue;
		typedef DXStackMap::value_type					DXStackMapValue;

		enum DX_ALLOCATOR_TYPE
		{
			DX_ALLOCATOR_TYPE_POOL,
			DX_ALLOCATOR_TYPE_STACK,
			DX_ALLOCATOR_TYPE_FREELIST,
		};

		struct DXAllocationHeader
		{
			IDXBaseAllocator*	pAllocator;
			DX_ALLOCATOR_TYPE	allocatorType;

#if defined(DX_MEMORY_DEBUG)
			DWORD				dwLine;
			WCHAR				szFileName[DX_MAX_FILE_NAME+1];
#endif
		};

	private:
		CDXHeapAllocator			m_Heap;
		DXPoolMap					m_mappPools;
		DXFreeListMap				m_mappFreeLists;
		DXStackMap					m_mappStacks;
		uintptr_t					m_uiTotalMemory;
		UINT32						m_uiNumPools;
		UINT32						m_uiNumFreeLists;
		UINT32						m_uiNumStacks;

	public:
		static CDXMemoryManager* GetInstance()
		{
			static CDXMemoryManager man;
			return &man;
		}


#if defined (DX_MEMORY_DEBUG)
#define DX_DECL_MEM_MAN_ADD_POOL			AddPool(uintptr_t uiObjectSize, UINT8 uiAlignment, uintptr_t uiNumObjects, WCHAR* szFile, DWORD dwLine)
#define DX_DECL_MEM_MAN_ADD_STACK			AddStack(uintptr_t uiMemorySize, WCHAR* szFile, DWORD dwLine)
#define DX_DECL_MEM_MAN_ALLOCATE_FROM_POOL	AllocateFromPool(CDXPoolAllocator* pPool, WCHAR* szFile, DWORD dwLine)
#define DX_DECL_MEM_MAN_ALLOCATE_FROM_STACK	AllocateFromStack(CDXStackAllocator* pStack, uintptr_t uiSize, WCHAR* szFile, DWORD dwLine)
#else
#define DX_DECL_MEM_MAN_ADD_POOL			AddPool(uintptr_t uiObjectSize, UINT8 uiAlignment, uintptr_t uiNumObjects)
#define DX_DECL_MEM_MAN_ADD_STACK			AddStack(uintptr_t uiMemorySize)
#define DX_DECL_MEM_MAN_ALLOCATE_FROM_POOL	AllocateFromPool(CDXPoolAllocator* pPool)
#define DX_DECL_MEM_MAN_ALLOCATE_FROM_STACK	AllocateFromStack(CDXStackAllocator* pStack, uintptr_t uiSize)
#endif

	public:
		CDXPoolAllocator*	DX_DECL_MEM_MAN_ADD_POOL;
		CDXStackAllocator*	DX_DECL_MEM_MAN_ADD_STACK;

	public:
		void* DX_DECL_MEM_MAN_ALLOCATE_FROM_POOL;
		void* DX_DECL_MEM_MAN_ALLOCATE_FROM_STACK;


	public:
#define DX_MEM_MAN_DEALLOCATE_DEFINTION(allocatorType, p) \
	DXAllocationHeader* pHeader = (DXAllocationHeader*)p - 1; \
	allocatorType* pAlloc = (allocatorType*)pHeader->pAllocator; \
	try \
	{ \
		pAlloc->Deallocate(pHeader); \
	} \
	catch (...) \
	{ \
		DX_DEBUG_OUTPUT2(L"ERROR while deallocating. ADDRESS: %p, ALLOCATOR NAME: %s.", p, pAlloc->GetDebugName()); \
	}

		// Using DeallocateFromXXX methods avoid a virtual table lookup which can gain
		// some performance so do use them when you are sure of the type of allocator.
		// However calling incorrect 'from' method can be disastrous.
	void	Deallocate(void* p)
	{
		DX_MEM_MAN_DEALLOCATE_DEFINTION(IDXBaseAllocator, p);
	}

	void	DeallocateFromPool(void* p)
	{
		DX_MEM_MAN_DEALLOCATE_DEFINTION(CDXPoolAllocator, p);
	}

	void	DeallocateFromStack(void* p)
	{
		DX_MEM_MAN_DEALLOCATE_DEFINTION(CDXStackAllocator, p);
	}

	private:
		CDXMemoryManager();
		~CDXMemoryManager();
	};

} // DXFramework namespace end.