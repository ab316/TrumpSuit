#include "DXStdafx.h"
#include "DXUnknown.h"
#include "DXResource.h"
#include "DXRefCounter.h"

namespace DXFramework
{ // DXFramework namespace begin

	CDXRefCounter::CDXRefCounter()
	{
		DX_DEBUG_OUTPUT0(L"ctor.");
	}


	CDXRefCounter::~CDXRefCounter()
	{
		DX_DEBUG_OUTPUT0(L"dtor.");

		Clear();
	}


	CDXRefCounter* CDXRefCounter::GetInstance()
	{
		// Must not use new because it would require the object
		// to be freed using the delete operator.
		static CDXRefCounter pRefCounter;

		return &pRefCounter;
	}


	void CDXRefCounter::AddObject(IDXUnknown* pObject)
	{
		m_vecpObjects.push_back(pObject);
	}


	void CDXRefCounter::RemoveObject(IDXUnknown* pUnknown)
	{
		std::vector <IDXUnknown*>::iterator it;
		it = std::find(m_vecpObjects.begin(), m_vecpObjects.end(), pUnknown);
		// If this object is in the vector.
		if (it != m_vecpObjects.end())
		{
			// Erase it any way from the vector.
			m_vecpObjects.erase(it);
		}
	}


	void CDXRefCounter::Clear()
	{
		m_vecpObjects.clear();
	}


	void CDXRefCounter::DebugDumpObjectList()
	{
		std::vector<IDXUnknown*>::iterator it;

		CDXHelper::OutputDebugStringW(L"RefCounter Dump Begin: Num Objects: %d", m_vecpObjects.size());

		for (it=m_vecpObjects.begin(); it!=m_vecpObjects.end(); it++)
		{
			IDXUnknown* pObject = *it;
			if (pObject->GetTypeId() == DX_RESOURCE_TYPE_ID)
			{
				CDXHelper::OutputDebugStringW(L"NAME: %s, TYPE: %s, RESOURCE TYPE: %s, Ref: %d", pObject->GetName(), pObject->GetTypeName(), ((IDXResource*)pObject)->GetResourceTypeName(), pObject->GetRefCount());
			}
			else
			{
				CDXHelper::OutputDebugStringW(L"NAME: %s, TYPE: %s, Ref: %d", pObject->GetName(), pObject->GetTypeName(), pObject->GetRefCount());
			}
		}

		CDXHelper::OutputDebugStringW(L"RefCounter Dump End.\n");
	}

} // DXFramework namespace end