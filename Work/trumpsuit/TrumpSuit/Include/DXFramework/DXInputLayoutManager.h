#pragma once

#include "DXResourceManager.h"

namespace DXFramework
{ // DXFramework namespace begin

	class CDXInputLayoutManager : public IDXResourceManager
	{
	public:
		virtual	HRESULT					Create(void* pNull=0);
		// Custom key is NOT recommended.
		virtual	DXResourceHandle		CreateResource(void* pLayoutDesc, WCHAR* szCustomKey);

	public:
		static CDXInputLayoutManager* GetInstance()
		{
			static CDXInputLayoutManager manager;
			return &manager;
		}

	private:
		CDXInputLayoutManager();

	public:
		~CDXInputLayoutManager();
	};

} // DXFramework namespace end