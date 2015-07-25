#pragma once

#include "DXResource.h"


namespace DXFramework
{ // DXFramework namespace begin

#define DX_MAX_IL_ELEMENT_NAME 15

	class CDXShader;

	// An input layout element describes the meaning of a group of bytes
	// in a vertex in the vertex buffer. As a single vertex normally contains
	// multiple information, multiple elements are required. These elements must be
	// arranged in the order in which the corresponding bytes appear in an individual vertex.
	struct DXInputLayoutElement
	{
		DX_FORMAT			format;
		WCHAR				szName[DX_MAX_IL_ELEMENT_NAME];
		// For having multiple elements having the same name.
		UINT				uiIndex;
		// Must be zero for now.
		UINT				uiInputSlot;
	};

	// Description of an input layout.
	struct DXInputLayoutDesc
	{
		// Since an input layout is closely related to a vertex shader, a vertex
		// shader is required to which the input layout will be working with. The input
		// semantics of the input layout and the shader must match. Any other vertex
		// shader that has the same semantics may also be used with the same input layout.
		CDXShader*				pVertexShader;
		DXInputLayoutElement*	pInputElements;
		UINT16					uiNumElements;
	};


	// An Input Layout describes the format of each vertex represented by a
	// a vertex buffer. This is necessary to bind the data in the vertex buffer
	// to appropriate registers in the Vertex shader. This is done through the
	// use of semantics.
	class CDXInputLayout : public IDXResource
	{
	private:
		DXInputLayoutDesc		m_descInput;
		ID3D11InputLayout*		m_pInputLayout;

	public:
		// Internal usage only.
		inline ID3D11InputLayout*		GetD3DLayout()	const	{ return m_pInputLayout;		};
		inline virtual  const void*		GetDesc()		const	{ return (void*)&m_descInput;	};

	public:
		virtual	HRESULT		Create(void* pDesc);
		virtual	HRESULT		Recreate();
		virtual	void		Destroy();
		virtual void		Dispose();
		virtual size_t		GetSize()	const { return (size_t)(m_descInput.uiNumElements * sizeof(DXInputLayoutElement)); };
		virtual void		SetDebugName(char* szName);

	public:
		virtual DX_RESOURCE_TYPE	GetResourceType()		const	{ return DX_RESOURCE_TYPE_INPUT_LAYOUT;			};
		virtual LPCWSTR				GetResourceTypeName()	const	{ return DX_RESOURCE_TYPE_NAME_INPUT_LAYOUT;	};

	public:
		CDXInputLayout();
		~CDXInputLayout();
	};

} // DXFramework namespace end