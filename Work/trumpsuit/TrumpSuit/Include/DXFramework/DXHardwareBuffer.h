#pragma once

#include "DXResource.h"


namespace DXFramework
{ // DXFramework namespace begin

	class CDX3DDriver;

	enum DX_HARDWAREBUFFER_TYPE
	{
		DX_HARDWAREBUFFER_TYPE_INDEXBUFFER		= D3D11_BIND_INDEX_BUFFER,
		DX_HARDWAREBUFFER_TYPE_VERTEXBUFFER		= D3D11_BIND_VERTEX_BUFFER,
		DX_HARDWAREBUFFER_TYPE_CONSTANTBUFFER	= D3D11_BIND_CONSTANT_BUFFER
	};


	// Description of a hardware buffer.
	struct DXHardwareBufferDesc
	{
		// Total size of the buffer. NOTE: Only 16 bit indices are supported.
		// For a constant buffer. The buffer size must be multiple of 16 and no
		// larger than 4096 bytes.
		UINT						uiBufferSize;
		// The size of one unit in the buffer. Only for vertex buffers.
		UINT						uiBufferStride;
		// Only 1 value should be specified. Combinations not allowed.
		DX_HARDWAREBUFFER_TYPE		bufferType;
		// GPU_READ_WRITE_ONLY can be used for constant buffers. If used then
		// UpdateConstantBuffer() must be used to update the resource. Otherwise
		// Map should be used.
		DX_GPU_RESOURCE_USAGE		gpuUsage;
		// Pointer to the buffer data. It is not used in case of a constant buffer.
		// If the gpuUsage has a CPU_WRITE access flag this can be NULL for VB and IB.
		// Otherwise it must be given.
		// The size of the buffer data MUST be equal to uiBufferSize.
		// Constant buffers do not allocate any memory for the data. They only
		// provide link to the GPU constant buffer.
		void*						pBufferData;
	};


	// A hardware buffer includes vertex, index and constant buffers. Hardware because
	// they reside in the GPU memory (mainly). The vertex and index buffer allocate their
	// own memory for the data. NOTE: Only 16 bit indices are supported.
	class CDXHardwareBuffer : public IDXResource
	{
	private:
		DXHardwareBufferDesc		m_descBuffer;
		ID3D11Buffer*				m_pD3dBuffer;

	public:
		// Internal usage only.
		inline ID3D11Buffer*			GetD3DBuffer()	const	{ return m_pD3dBuffer;			};
		inline virtual  const void*		GetDesc()		const	{ return (void*)&m_descBuffer;	};

	public:
		virtual HRESULT		Create(void* pDesc);
		virtual HRESULT		Recreate();
		virtual void		Destroy();
		virtual void		Dispose();
		virtual size_t		GetSize()	const { return (size_t)(m_descBuffer.uiBufferSize); };
		virtual void		SetDebugName(char* szName);

	public:
		virtual DX_RESOURCE_TYPE	GetResourceType()		const	{ return DX_RESOURCE_TYPE_HARDWARE_BUFFER;		};
		virtual LPCWSTR				GetResourceTypeName()	const	{ return DX_RESOURCE_TYPE_NAME_HARDWARE_BUFFER;	};

	public:
		CDXHardwareBuffer();
		~CDXHardwareBuffer();
	};

} // DXFramework namespace end