#pragma once

#include "DXUnknown.h"


namespace DXFramework
{ // DXFramework namespace begin

#define DX_RESOURCE_MAGIC_NUMBER				0xABFD

#define DX_RESOURCE_TYPE_NAME_UNKNOWN			L"UNKNOWN"
#define DX_RESOURCE_TYPE_NAME_TEXTURE			L"TEXTURE"
#define DX_RESOURCE_TYPE_NAME_HARDWARE_BUFFER	L"HARDWARE_BUFFER"
#define DX_RESOURCE_TYPE_NAME_INPUT_LAYOUT		L"INPUT_LAYOUT"
#define DX_RESOURCE_TYPE_NAME_SHADER			L"SHADER"

#ifdef DX_DEBUG
#define DX_RESOURCE_SET_DEBUG_NAME(pResource, szName)	try { pResource->SetDebugName(szName); } catch (...) { DX_DEBUG_OUTPUT1(L"WARNING!!! Exception raised in setting resource debug name: %s", L#szName); }
#else
#define DX_RESOURCE_SET_DEBUG_NAME(pResource, szName) 
#endif

#define DX_BIND_SHADER_RESOURCE		D3D11_BIND_SHADER_RESOURCE
#define DX_BIND_RENDER_TARGET		D3D11_BIND_RENDER_TARGET
#define DX_BIND_DEPTH_STENCIL		D3D11_BIND_DEPTH_STENCIL


	enum DX_RESOURCE_TYPE
	{
		DX_RESOURCE_TYPE_TEXTURE,
		DX_RESOURCE_TYPE_HARDWARE_BUFFER,
		DX_RESOURCE_TYPE_INPUT_LAYOUT,
		DX_RESOURCE_TYPE_SHADER,
	};

	enum DX_GPU_RESOURCE_USAGE
	{
		// The GPU should have read only access to the resource. No CPU access.
		// Resource created this with flags is NON-MAPPABLE.
		DX_GPU_READ_ONLY,
		// GPU has read/write access to the resource. No CPU access.
		// Resource created this with flags is NON-MAPPABLE.
		DX_GPU_READ_WRITE_ONLY,
		// Read only for GPU, Write only for CPU. Resource created with
		// this flag can't be set as output to a pipeline stage.
		// Resource created this with flags is MAPPABLE.
		DX_GPU_READ_CPU_WRITE,
		// Such a resource can not be set as either input or output to any
		// pipeline stage. // Resource created this with flags is MAPPABLE.
		DX_CPU_READ_WRITE,
	};


	WCHAR*	GetResourceTypeName(DX_RESOURCE_TYPE type);


	// Resources include Vertex, Index buffers, textures, shaders,
	// materials, (and other stuff i have'nt named yet).
	class IDXResource : public IDXUnknown
	{
	public:
		static const UINT16		ms_uiMagicNumber;

	protected:
		// The Recreate() method should only do the job if disposed and created flags
		// are both true.
		bool	m_bDisposed;
		// Set this to true in the Create method after successful creation.
		bool	m_bCreated;

	public:
		// This method should load data into the resource such that
		// the resource can be recreated as many times as necessary using
		// Recreate(). Create only prepares the resource so that it can be created
		// at any time by calling Recreate(). This method should not be called
		// directly. Instead use the appropriate resource manager to create a
		// resource.
		virtual HRESULT		Create(void* pParams) = 0;
		// This method should include the functionality to create the resource
		// such that the resource manager can Dispose() the resource and Recreate()
		// it as needed at any time. E.g. if the resource is a texture, this function
		// should include the functionality to create the D3D11 resource.
		virtual HRESULT		Recreate() = 0;
		// This method should destroy the resource data permanently so that it
		// can never haunt us again.
		virtual	void		Destroy() = 0;
		// This method should discard the resource data in such a way that
		// the resource can be recreated uisng Recreate() when needed.
		virtual void		Dispose() = 0;
		// This method must return the size of the data that the Recreate(), Dispose()
		// methods create and destroy. This data may be in CPU or GPU memory.
		virtual size_t		GetSize() const = 0;
		// This method must return true if the data has been discared and false if not.
		// Option method to set a debug name for the resource. E.g. for hardware resources.
		// This method in the inherited class should return the type of the resource
		virtual DX_RESOURCE_TYPE		GetResourceType()		const	= 0;
		virtual LPCWSTR					GetResourceTypeName()	const	= 0;
		// Only for debug builds. Use DX_RESOURCE_SET_DEBUG_NAME macro.
		virtual void		SetDebugName(char* szName)		{UNREFERENCED_PARAMETER(szName);}; 
		// This function in the derived class should return a "Description" struct of the resource.
		virtual const void*	GetDesc()				const	{ return 0; };
		bool				IsDisposed()			const	{ return m_bDisposed;	};
		bool				IsCreated()				const	{ return m_bCreated;	};

	public:
		virtual UINT		GetTypeId()		const	{ return DX_RESOURCE_TYPE_ID;	};
		virtual LPCWSTR		GetTypeName()	const	{ return DX_RESOURCE_TYPE_NAME;	};

	public:
		IDXResource();
		virtual ~IDXResource();
	};

} // DXFramework namespace end