#pragma once

#include "DXResourceManager.h"


namespace DXFramework
{ // DXFramework namespace begin

	class CDXHardwareBufferManager : public IDXResourceManager
	{
	public:
		virtual	HRESULT					Create(void* pNull=0);
		// Custom key must be provided. The key will be prefixed to reflect the type
		// of the buffer. Keep key name small. (50 chars max).
		virtual	DXResourceHandle		CreateResource(void* pBufferDesc, WCHAR* szCustomKey=0);

	public:
		static CDXHardwareBufferManager* GetInstance()
		{
			static CDXHardwareBufferManager manager;
			return &manager;
		}

	private:
		CDXHardwareBufferManager();

	public:
		~CDXHardwareBufferManager();
	};

} // DXFramework namespace end