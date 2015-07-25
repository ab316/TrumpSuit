#pragma once

#include "DXGrowableAllocator.h"

namespace DXFramework
{ // DXFramework namespace begin

	//template <class T>
	class CDXTrackingPointer
	{
	private:
		void*					m_p;
		IDXGrowableAllocator*	m_pAllocator;
		UINT32					m_uiRegisteredId;

	public:
		template <class T>	T	operator*()						{ return *m_p;		};
		template <class T>	T*	operator->()					{ return m_p;		};
		template <class T>	T*	operator[](unsigned __int64 i)	{ return m_p[i];	};
		template <class T>		operator T*()					{ return (T*)m_p;	};
		// This operator is not defined as it would be erroneous to assign a raw pointer
		// this way without any knowledge of allocator.
		template <class T>	void	operator=(T* rhs);

		void	operator=(CDXTrackingPointer& rhs)
		{
			this->~CDXTrackingPointer();
			Initialize(rhs.m_p, rhs.m_pAllocator);
		}

		bool	operator==(CDXTrackingPointer& rhs)		{ return m_p == rhs.m_p;	};
		bool	operator==(void* rhs)					{ return m_p == rhs;		};
		bool	operator!=(CDXTrackingPointer& rhs)		{ return m_p != rhs.m_p;	};
		bool	operator!=(void* rhs)					{ return m_p != rhs;		};
		bool	operator!()								{ return !m_p;				};

	public:
		void Initialize(void* p, IDXGrowableAllocator* pAllocator)
		{
			m_p = p;
			m_pAllocator = pAllocator;
			m_uiRegisteredId = m_pAllocator->RegisterPointerForUpdate((void**)&m_p);
		}

	public:
		CDXTrackingPointer()
		{
			m_p = nullptr;
			m_pAllocator = nullptr;
		}

		CDXTrackingPointer(void* p, IDXGrowableAllocator* pAllocator)
		{
			Initialize(p, pAllocator);
		}

		~CDXTrackingPointer()
		{
			if (m_pAllocator && m_p)
			{
				m_pAllocator->UnregisterPointer(m_uiRegisteredId);
			}
		}

	};


} // DXFramework namespace end