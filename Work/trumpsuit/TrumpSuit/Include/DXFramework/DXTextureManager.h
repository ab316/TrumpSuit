#pragma once

#include "DXResourceManager.h"

namespace DXFramework
{ // DXFramework namespace begin

	class CDX3DDriver;

	class CDXTextureManager : public IDXResourceManager
	{

	public:
		virtual	HRESULT					Create(void* pNull = 0);
		// Custom key is must for non-file textures. Optional for file textures.
		// Keep file names small.
		virtual	DXResourceHandle		CreateResource(void* pTex2DDesc, WCHAR* szCustomKey=0);

	public:
		static CDXTextureManager* GetInstance()
		{
			static CDXTextureManager manager;
			return &manager;
		}

	private:
		CDXTextureManager();

	public:
		~CDXTextureManager();
	};

} // DXFramework namespace end