#include "DXStdafx.h"
#include "DXResource.h"

namespace DXFramework
{ // DXFramework namespace begin

	const UINT16 IDXResource::ms_uiMagicNumber = DX_RESOURCE_MAGIC_NUMBER;


	WCHAR*	GetResourceTypeName(DX_RESOURCE_TYPE type)
	{
		switch (type)
		{
		case DX_RESOURCE_TYPE_TEXTURE:
			return DX_RESOURCE_TYPE_NAME_TEXTURE;
			
		case DX_RESOURCE_TYPE_HARDWARE_BUFFER:
			return DX_RESOURCE_TYPE_NAME_HARDWARE_BUFFER;

		case DX_RESOURCE_TYPE_INPUT_LAYOUT:
			return DX_RESOURCE_TYPE_NAME_INPUT_LAYOUT;

		case DX_RESOURCE_TYPE_SHADER:
			return DX_RESOURCE_TYPE_NAME_SHADER;

		default:
			return L"ERROR TYPE";
		}
	}


	IDXResource::IDXResource() : m_bDisposed(true)
	{
	}

	
	IDXResource::~IDXResource()
	{
	}

} // DXFramework namespace end