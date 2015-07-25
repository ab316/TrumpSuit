#pragma once

#include "DXResourceManager.h"

namespace DXFramework
{ // DXFramework namespace begin

	class CDXShaderManager : public IDXResourceManager
	{
	public:
		virtual	HRESULT					Create(void* pNull=0);
		// Custom key is NOT recommended. Keep file names small (less than 20 chars).
		virtual	DXResourceHandle		CreateResource(void* pShaderDesc, WCHAR* szCustomKey=0);

	public:
		static CDXShaderManager* GetInstance()
		{
			static CDXShaderManager manager;
			return &manager;
		}

	private:
		CDXShaderManager();

	public:
		~CDXShaderManager();
	};

} // DXFramework namespace end