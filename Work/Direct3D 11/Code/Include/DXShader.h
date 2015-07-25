#pragma once

#include "DXResource.h"


namespace DXFramework
{ // DXFramework namespace begin

#define DX_MAX_SHADER_ENTRY		15
#define DX_MAX_SHADER_PROFILE	15

	enum DX_SHADER_TYPE
	{
		DX_SHADER_PIXEL_SHADER,
		DX_SHADER_VERTEX_SHADER,
	};

	struct DXShaderDesc
	{
		WCHAR			szFileName[DX_MAX_FILE_NAME+1];
		// DX_MAX_SHADER_ENTRY chars max
		WCHAR			szEntryPoint[DX_MAX_SHADER_ENTRY+1];
		// DX_MAX_SHADER_ENTRY chars max
		WCHAR			szShaderLevel[DX_MAX_SHADER_PROFILE+1];
		DX_SHADER_TYPE	type;
		// Internal usage.
		size_t			uiBufferSize;
		// Internal usage.
		void*			pBuffer;
	};


	// A shader is a program that runs on the GPU. It used to process vertices
	// (Vertex shader) or pixels (Pixel shaders) in tn the graphics pipeline.
	class CDXShader : public IDXResource
	{
	private:
		DXShaderDesc			m_desc;
		ID3DBlob*				m_pShaderBlob;
		void*					m_pShader;

	public:
		// Internal usage only.
		inline void*					GetD3DShader()	const	{ return m_pShader; };
		inline void*					GetShaderCode()	const	{ return m_pShaderBlob->GetBufferPointer();	};
		inline virtual  const void*		GetDesc()		const	{ return (void*)&m_desc;	};

	public:
		virtual	HRESULT		Create(void* pDesc);
		virtual	HRESULT		Recreate();
		virtual	void		Destroy();
		virtual void		Dispose();
		virtual void		SetDebugName(char* szName);
		virtual size_t		GetSize()		const		{ return m_desc.uiBufferSize; };

	public:
		virtual DX_RESOURCE_TYPE	GetResourceType()		const	{ return DX_RESOURCE_TYPE_SHADER;		};
		virtual LPCWSTR				GetResourceTypeName()	const	{ return DX_RESOURCE_TYPE_NAME_SHADER;	};

	public:
		CDXShader();
		~CDXShader();
	};


	// This interface handles the #include directives in the shader files. It is
	// internally used direct3D driver.
	interface IDXShaderIncludeHandler : public ID3D10Include
	{
	private:
		WCHAR m_szFile[DX_MAX_FILE_PATH];

	public:
		// Reads the file using the file system.
		HRESULT _stdcall Open(D3D10_INCLUDE_TYPE includeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID* ppData, UINT* pBytes);
		// Decreases the file usage.
		HRESULT _stdcall Close(LPCVOID pData);
	};

} // DXFramework namespace end