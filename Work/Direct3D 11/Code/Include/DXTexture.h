#pragma once

#include "DXResource.h"

namespace DXFramework
{ // DXFramework namespace begin

	class CDX3DDriver;

	
	// For a file texture we only need the file name. And for a custom
	// filled texture we only need its memory data (required to recreate it) and size.
	struct DXTexture2DData
	{
		// Only supply file name. No path.
		WCHAR	szFileName[DX_MAX_FILE_NAME];
		// For a file texture, this field is the size of the file. For a manually filled texture 
		// it is, the size of a single line of the texture = # of pixels in line x size of 1 pixel.
		size_t	uiPitchOrSize;
		// Pointer to the texture data. For a self filled texture. The size of the data
		// pointed by this pointer must be equal width * height * formatSize. This is
		// valid only for texture with only 1 MIP level.
		void*	pData;
	};

	// Our texture description.
	// Currently only 2d textures are supported.
	struct DXTexture2DDesc
	{
		DXTexture2DData			textureData;
		// No need to set for file texture.
		DX_FORMAT				textureFormat;
		// Only 1 allowed. No combinations.
		DX_GPU_RESOURCE_USAGE	gpuUsage;
		// One or more of DX_BIND flags.
		UINT16					bindFlags;
		// No need to set for a file texture.
		UINT16					uiWidth;
		// No need to set for a file texture.
		UINT16					uiHeight;
		// No need to set for a file texture.  Should set to 1 for non file textures.
		UINT16					uiMipLevels;
		// no need to set for a file texture. Always set to 1 for non render targets.
		UINT16					uiMultisampleLevel;
		bool					bFileTexture;
	};


	// A texture is a structured collection of data. Usually it stores color information
	// for an object which will be applied to it in the pixel shader. The format of the
	// information contained in the texture depends on the given DX_FORMAT or predefined
	// for file textures. The output on the screen is also contained in texture called
	// a Render Target. Depth information is contained in another texture.
	// For a file texture, the texture memory is allocated by the file system. For non-file
	// textures it is allocated by this class.
	class CDXTexture : public IDXResource
	{
	private:
		DXTexture2DDesc				m_descTexture;
		ID3D11Texture2D*			m_pTexture2d;
		ID3D11ShaderResourceView*	m_pShaderResourceView;
		ID3D11RenderTargetView*		m_pRenderTargetView;
		ID3D11DepthStencilView*		m_pDepthStencilView;


	public:
		virtual	HRESULT		Create(void* pDesc);
		virtual	HRESULT		Recreate();
		virtual	void		Destroy();
		virtual void		Dispose();
		virtual size_t		GetSize() const;
		virtual void		SetDebugName(char* szName);

		// Internal usage only.
	public:
		inline virtual  const void*	GetDesc()				const	{ return (void*)&m_descTexture;	};
		ID3D11Texture2D*			GetD3DTexture()			const	{ return m_pTexture2d;			};
		ID3D11ShaderResourceView*	GetShaderResourceView()	const	{ return m_pShaderResourceView;	};
		ID3D11RenderTargetView*		GetRenderTargetView()	const	{ return m_pRenderTargetView;	};
		ID3D11DepthStencilView*		GetDepthStencilView()	const	{ return m_pDepthStencilView;	};

	public:
		virtual DX_RESOURCE_TYPE	GetResourceType()		const	{ return DX_RESOURCE_TYPE_TEXTURE;		};
		virtual LPCWSTR				GetResourceTypeName()	const	{ return DX_RESOURCE_TYPE_NAME_TEXTURE;	};

	public:
		CDXTexture();
		~CDXTexture();
	};

} // DXFramework namespace end