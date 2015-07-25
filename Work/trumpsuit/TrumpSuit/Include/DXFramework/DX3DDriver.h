#pragma once

#include "DXUnknown.h"
#include "DXResourcesInclude.h"

namespace DXFramework
{ // DXFramework namespace begin

#define DX_D3D_DEVICE_NAME			"DX D3D11 Device"
#define DX_IMMEDIATE_CONTEXT_NAME	"DX Immediate Context"
#define DX_SWAPCHAIN_NAME			"DX SwapChain"
#define DX_BACKBUFFER_NAME			"DX BackBuffer"
#define DX_RENDERTARGET_NAME		"DX RenderTarget"

#define DX_CLEAR_DEPTH		D3D11_CLEAR_DEPTH
#define DX_CLEAR_STENCIL	D3D11_CLEAR_STENCIL

#define DX_MAX_PIPELINE_TEXTURES_SLOTS		16
#define DX_MAX_PIPELINE_SAMPLERS_SLOTS		8

// This pointer is used to indicate for render target pointer that the target
// is the main swap chain render target whose data is output to the screen. This render target
// is not a CDXTexture instance.
#define DX_DEFAULT_RENDER_TARGET	(CDXTexture*)-1

	enum DX_PRIMITIVE_TYPE
	{
		// Default.
		DX_PRIMITIVE_TYPE_TRIANGLE_LIST	=	D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		DX_PRIMITIVE_TYPE_POINT_LIST	=	D3D11_PRIMITIVE_TOPOLOGY_POINTLIST,
		DX_PRIMITIVE_TYPE_LINE_LIST		=	D3D11_PRIMITIVE_TOPOLOGY_LINELIST,
		DX_PRIMITIVE_TYPE_UNDEFINED		=	D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED,
	};


	// This specifies the data source used in blending.
	// DEST is the pixel fragment already in the render target. SRC is the incoming
	// pixel fragment.
	enum DX_BLEND
	{
		DX_BLEND_ZERO				=	D3D11_BLEND_ZERO,
		DX_BLEND_ONE				=	D3D11_BLEND_ONE,
		DX_BLEND_SRC_COLOR			=	D3D11_BLEND_SRC_COLOR,
		DX_BLEND_INV_SRC_COLOR		=	D3D11_BLEND_INV_SRC_COLOR,
		DX_BLEND_SRC_ALPHA			=	D3D11_BLEND_SRC_ALPHA,
		DX_BLEND_INV_SRC_ALPHA		=	D3D11_BLEND_INV_SRC_ALPHA,
		DX_BLEND_DEST_ALPHA			=	D3D11_BLEND_DEST_ALPHA,
		DX_BLEND_INV_DEST_ALPHA		=	D3D11_BLEND_INV_DEST_ALPHA,
		DX_BLEND_DEST_COLOR			=	D3D11_BLEND_DEST_COLOR,
		DX_BLEND_INV_DEST_COLOR		=	D3D11_BLEND_INV_DEST_COLOR,
		DX_BLEND_BLEND_FACTOR		=	D3D11_BLEND_BLEND_FACTOR,
		DX_BLEND_INV_BLEND_FACTOR	=	D3D11_BLEND_INV_BLEND_FACTOR,
	};


	// The blending operation to use in blending.
	enum DX_BLEND_OP
	{
		DX_BLEND_OP_ADD				=	D3D11_BLEND_OP_ADD,
		DX_BLEND_OP_SUBTRACT		=	D3D11_BLEND_OP_SUBTRACT,
		DX_BLEND_OP_REV_SUBTRACT	=	D3D11_BLEND_OP_REV_SUBTRACT,
		DX_BLEND_OP_MIN				=	D3D11_BLEND_OP_MIN,
		DX_BLEND_OP_MAX				=	D3D11_BLEND_OP_MAX,
	};

	
	// Which channels to write during blending.
	enum DX_WRITE_MASK
	{
		DX_WRITE_MASK_RED		=	D3D11_COLOR_WRITE_ENABLE_RED,
		DX_WRITE_MASK_GREEN		=	D3D11_COLOR_WRITE_ENABLE_GREEN,
		DX_WRITE_MASK_BLUE		=	D3D11_COLOR_WRITE_ENABLE_BLUE,
		DX_WRITE_MASK_ALPHA		=	D3D11_COLOR_WRITE_ENABLE_ALPHA,
		DX_WRITE_MASK_ALL		=	D3D10_COLOR_WRITE_ENABLE_ALL,
	};


	enum DX_FILL_MODE
	{
		DX_FILL_MODE_SOLID		=	D3D11_FILL_SOLID,
		DX_FILL_MODE_WIRE		=	D3D11_FILL_WIREFRAME,
	};


	enum DX_CULL_MODE
	{
		DX_CULL_MODE_NONE		=	D3D11_CULL_NONE,
		// Don't render front face.
		DX_CULL_MODE_FRONT		=	D3D11_CULL_FRONT,
		// Don't render back face.
		DX_CULL_MODE_BACK		=	D3D11_CULL_BACK,
	};


	// Texturing filter type.
	enum DX_FILTER
	{
		DX_FILTER_MIN_MAG_MIP_POINT			=	D3D11_FILTER_MIN_MAG_MIP_POINT,
		DX_FILTER_MIN_MAG_MIP_LINEAR		=	D3D11_FILTER_MIN_MAG_MIP_LINEAR,
		DX_FILTER_ANISOTROPIC				=	D3D11_FILTER_ANISOTROPIC,
	};


	enum DX_TEXTURE_ADDRESS
	{
		// Texture is repeated outside [0, 1] range.
		DX_TEXTURE_ADDRESS_WRAP				=	D3D11_TEXTURE_ADDRESS_WRAP,
		// Flips the texture at every integer.
		DX_TEXTURE_ADDRESS_MIRROR			=	D3D11_TEXTURE_ADDRESS_MIRROR,
		// Texture coordinates are clamped to [0, 1] range.
		DX_TEXTURE_ADDRESS_CLAMP			=	D3D11_TEXTURE_ADDRESS_CLAMP,
		// Outside the [0, 1] range, border color specified in the sampler is used.
		DX_TEXTURE_ADDRESS_BORDER			=	D3D11_TEXTURE_ADDRESS_BORDER,
	};


	enum DX_MAP
	{
		// Resource must have been created with DX_CPU_READ_WRITE flag.
		DX_MAP_READ			=	D3D11_MAP_READ,
		// Resource must have been created with DX_GPU_READ_CPU_WRITE flag.
		DX_MAP_WRITE		=	D3D11_MAP_WRITE_DISCARD,
		// Resource must have been created with DX_CPU_READ_WRITE flag.
		DX_MAP_READ_WRITE	=	D3D11_MAP_READ_WRITE
	};


	enum DX_COMPARISON_FUNC
	{
		DX_COMPARISON_FUNC_NEVER			=	1,
		DX_COMPARISON_FUNC_LESS				=	2,
		DX_COMPARISON_FUNC_EQUAL			=	3,
		DX_COMPARISON_FUNC_LESS_EQUAL		=	4,
		DX_COMPARISON_FUNC_GREATER			=	5,
		DX_COMPARISON_FUNC_NOT_EQUAL		=	6,
		DX_COMPARISON_FUNC_GREATER_EQUAL	=	7,
		DX_COMPARISON_FUNC_ALWAYS			=	8,
	};


	enum DX_STENCIL_OP
	{
		DX_STENCIL_OP_KEEP			=	1,
		DX_STENCIL_OP_ZERO			=	2,
		DX_STENCIL_OP_REPLACE		=	3,
		// Increment and clamp.
		DX_STENCIL_OP_INCR_SAT		=	4,
		// Decrement and clamp
		DX_STENCIL_OP_DECR_SAT		=	5,
		DX_STENCIL_OP_INVERT		=	6,
		// Increment and wrap.
		DX_STENCIL_OP_INCR			=	7,
		// Decrement and wrap.
		DX_STENCIL_OP_DECR			=	8,
	};


	// a viewport defines the region the output will occupy on the render target.
	struct DXViewport
	{
		// In pixels.
		float		fWidth;
		// In pixels.
		float		fHeight;
		// 0.0f to 1.0f.
		float		fMinDepth;
		// 0.0f to 1.0f.
		float		fMaxDepth;
		// In pixels.
		float		fTopLeftX;
		// In pixels.
		float		fTopLeftY;
	};


	// Represents a blendstate description.
	// The new pixel fragment is:
	// (source color.srcBlend) (blendOp) (dest color.destBlend)
	// Alpha channel blending is seperate. Done with the same equation.
	// If bBlendEnable and bAlphaToCoverage are both false, the other fields don't matter.
	struct DXBlendDesc
	{
		// Data source for source color.
		DX_BLEND		srcBlend;
		// Data source for the destination color.
		DX_BLEND		destBlend;
		// The blending operation to carry on srcBlend and destBlend.
		DX_BLEND_OP		blendOp;
		DX_BLEND		srcBlendAlpha;
		DX_BLEND		destBlendAlpha;
		DX_BLEND_OP		blendOpAlpha;
		// Which channels to write to after blending. One or more of DX_WRITE_MASK_
		// default DX_WRITE_MASK_ALL.
		UINT8			writeMask;
		// Default false.
		bool			bBlendEnable;
		// Multisampling technique, useful when there are several overlapping polygons.
		// Default false. Set to false if bBlendEnable is false.
		bool			bAlphaToCoverage;

		DXBlendDesc()
		{
			bBlendEnable = false;
			bAlphaToCoverage = false;
			srcBlend = DX_BLEND_ONE;
			destBlend = DX_BLEND_ZERO;
			srcBlendAlpha = DX_BLEND_ONE;
			destBlendAlpha = DX_BLEND_ZERO;
			blendOp = DX_BLEND_OP_ADD;
			blendOpAlpha = DX_BLEND_OP_ADD;
			writeMask = DX_WRITE_MASK_ALL;
		}
	};


	// Description of a rasterizer state,
	struct DXRasterizerDesc
	{
		// Default solid.
		DX_FILL_MODE		fillMode;
		// Default back.
		DX_CULL_MODE		cullMode;
		// depth value added to the pixel. Default 0.
		int					iDepthBias;
		// Max depth bias of a mixel. Default 0.0f;
		float				fDepthBiasClamp;
		// Triangle with vertex order ccw are front. Default false.
		bool				bFrontCCW;
		// If true (default) clipping based on distance is enabled.
		bool				bDepthClip;
		//  Use multisampling. Default false.
		bool				bMultisample;

		DXRasterizerDesc()
		{
			bDepthClip = true;
			bFrontCCW = false;
			bMultisample = false;
			cullMode = DX_CULL_MODE_BACK;
			fDepthBiasClamp = 0.0f;
			fillMode = DX_FILL_MODE_SOLID;
			iDepthBias = 0;
			bMultisample = false;
		}
	};


	// A sampler is used when sampling a texture (when reading data from the
	// texture in a shader using UV coords).
	struct DXSamplerDesc
	{
		float					fBorderColor[4];
		// Default MIN_MAG_MIP_LINEAR.
		DX_FILTER				filter;
		// Default WRAP.
		DX_TEXTURE_ADDRESS		addressU;
		// Default WRAP.
		DX_TEXTURE_ADDRESS		addressV;
		// Must be [1, 16]. Only of ANISOTROPIC address mode.
		UINT					uMaxAnisotropicity;

		DXSamplerDesc()
		{
			fBorderColor[0] = 0.0f;
			fBorderColor[1] = 0.0f;
			fBorderColor[2] = 0.0f;
			fBorderColor[3] = 1.0f;
			addressU = DX_TEXTURE_ADDRESS_WRAP;
			addressV = DX_TEXTURE_ADDRESS_WRAP;
			filter = DX_FILTER_ANISOTROPIC;
			uMaxAnisotropicity = 8;
		}
	};


	struct DXDepthStencilOpDesc
	{
		DX_STENCIL_OP			opStencilFail;
		DX_STENCIL_OP			opDepthStencilFail;
		DX_STENCIL_OP			opStencilPass;
		DX_COMPARISON_FUNC		stencilFunc;

		DXDepthStencilOpDesc()
		{
			stencilFunc = DX_COMPARISON_FUNC_ALWAYS;
			opDepthStencilFail = DX_STENCIL_OP_KEEP;
			opStencilFail = DX_STENCIL_OP_KEEP;
			opStencilPass = DX_STENCIL_OP_KEEP;
		}
	};


	struct DXDepthStencilDesc
	{
		// Default: Func: always, Ops: keep.
		DXDepthStencilOpDesc	frontFace;
		DXDepthStencilOpDesc	backFace;
		// Default: LESS.
		DX_COMPARISON_FUNC		depthFunc;
		// Default: 0xff.
		UINT8					uiStencilWriteMask;
		// Default: 0xff.
		UINT8					uiStencilReadMask;
		// Default: true.
		bool					bDepthEnable;
		// Default: true.
		bool					bDepthWrite;
		// Default: false
		bool					bStencilEnable;

		DXDepthStencilDesc()
		{
			bDepthEnable = true;
			bDepthWrite = true;
			bStencilEnable = false;
			depthFunc = DX_COMPARISON_FUNC_LESS;
			uiStencilReadMask = 0xff;
			uiStencilWriteMask = 0xff;
		}
	};


	// A blend state.
	struct DXBlendState
	{
		DXBlendDesc			desc;
		// Internal usage only.
		ID3D11BlendState*	m_pBlendState;
	};


	struct DXRasterizerState
	{
		DXRasterizerDesc		desc;
		// Internal usage only.
		ID3D11RasterizerState*	m_pRasterState;
	};


	struct DXSamplerState
	{
		DXSamplerDesc			desc;
		// Internal usage only.
		ID3D11SamplerState*		m_pSamplerState;
	};


	struct DXDepthStencilState
	{
		DXDepthStencilDesc			desc;
		ID3D11DepthStencilState*	m_pDepthStencilState;
	};


	struct DXMappedResource
	{
		// Row pitch is the size of 1 row in bytes (for a texture).
		UINT			uiRowPitch;
		// For 3d textures (no used).
		UINT			uiDepthPitch;
		// Pointer to the resource data.
		void*			pData;
	};


	// Represents the present state of the 3d pipeline. 0 means none assigned.
	struct DXPipelineState
	{
		DXViewport			viewport;
		CDXTexture*			pPSTextures[DX_MAX_PIPELINE_TEXTURES_SLOTS];
		DXSamplerState*		pPSSamplers[DX_MAX_PIPELINE_SAMPLERS_SLOTS];
		// The current render target. -1 if the main backbuffer.
		CDXTexture*			pRenderTarget;
		CDXTexture*			pDepthStencil;
		CDXHardwareBuffer*	pVertexBuffer;
		CDXHardwareBuffer*	pIndexBuffer;
		CDXShader*			pVertexShader;
		CDXShader*			pPixelShader;
		CDXInputLayout*		pInputLayout;
		DXBlendState*		pBlendState;
		DXRasterizerState*	pRasterState;
		DXDepthStencilState*pDepthStencilState;
		UINT				uiIndexBufferOffset;
		UINT				uiVertexBufferOffset;
		// Width of the backbuffer.
		UINT				uiWidth;
		// Height of the backbuffer;
		UINT				uiHeight;
		DX_PRIMITIVE_TYPE	primitveType;
		bool				bWindowed;
	};


	typedef std::vector<ID3D11BlendState*>			DXBlendStateVector;
	typedef std::vector<ID3D11RasterizerState*>		DXRasterStateVector;
	typedef std::vector<ID3D11SamplerState*>		DXSamplerStateVector;
	typedef std::vector<ID3D11DepthStencilState*>	DXDepthStencilStateVector;
	typedef DXBlendStateVector::iterator			DXBlendStateVectorIterator;
	typedef DXRasterStateVector::iterator			DXRasterStateVectorIterator;
	typedef DXSamplerStateVector::iterator			DXSamplerStateVectorIterator;
	typedef DXDepthStencilStateVector::iterator		DXDepthStencilStateVectorIterator;
	// A D3DDriver is responsible for handling 3d related tasks. This class is for
	// internal usage only. It is a singleton class. The terms 'backbuffer' and
	// 'default render target' refer to the texture (internal texture, not CDXTexture)
	// whose data is presented on the client screen.
	class CDX3DDriver : public IDXUnknown
	{
	private:
		DXBlendStateVector			m_vecpBlendStates;
		DXRasterStateVector			m_vecpRasterStates;
		DXSamplerStateVector		m_vecpSamplerStates;
		DXDepthStencilStateVector	m_vecpDepthStencilStates;
		// The present state of the pipeline.
		DXPipelineState				m_PipeLineState;
		// The client window.
		HWND						m_hWnd;
		size_t						m_uiVideoMemory;
		// The video adapter abstraction.
		IDXGIAdapter1*				m_pAdapter;
		// The D3D 11 device.
		ID3D11Device*				m_pD3dDevice;
		// The immediate device context which renders immediately.
		ID3D11DeviceContext*		m_pImmediateContext;
		// Swap contains 2 buffer, front and back for rendering. Rendering is done
		// on the back buffer. The output display is extracted from the front. One
		// rendering on the back is finished the buffers are swapped.
		IDXGISwapChain*				m_pMainSwapChain;
		// The render target view. THe device context renders on the back buffer
		// through this view.
		ID3D11RenderTargetView*		m_pRenderTargetView;
		// The d3d feature level e.g d3d11, d3d10.1 d3d10, d3d9.
		DXBlendState*				m_pBlendStateDefault;
		DXRasterizerState*			m_pRasterizerStateDefault;
		DXSamplerState*				m_pSamplerDefault;
		DXDepthStencilState*		m_pDepthStencilStateDefault;
		D3D_FEATURE_LEVEL			m_featureLevel;

		// Internal usage only.
	public:
		ID3D11Device*			GetD3DDevice()			const	{ return m_pD3dDevice;		};
		IDXGIAdapter1*			GetAdapter()			const	{ return m_pAdapter;		};
		const DXPipelineState*	GetPipelineState()		const	{ return &m_PipeLineState;	};

	private:
		// Finds the best video adapter. Usually there is only one.
		void		FindBestAdapter();
		void		GetAppropriateD3DUsage(DX_GPU_RESOURCE_USAGE inUsage, D3D11_USAGE* pUsage, UINT* pCpuAccess);
		// Creates the default blend, rasterizer and sampler states.
		void		CreateDefaultPipelineStates();

	public:
		// Creates the d3d11 device object, the rendering context and the swap chain.
		HRESULT		Startup(DXWindowState windowState);
		// Create/Recreates the render target view for rendering. This method should 
		// be recalled whenever the rendeing client area is resized.
		HRESULT		CreateRenderTargetView();
		// Needed to be called during shutdown after destroying all the resources.
		void		Shutdown();
		// Resets the pipeline to the default state. Nothing remains bound to the pipeline.
		// The primitive type is set to tiangle list and the render target is set to the
		// main render target. This does not change the back buffer width, height and windowed
		// fields.
		void		ResetPipelineState();
		// Updates the dimensions of the backbuffer in the pipeline. Fullscreen/windowed
		// mode is not set.
		void		UpdateWindowState(DXWindowState pNewState);
		// Fullscreen/windowed switch.
		void		SetWindowedMode(bool bWindowed);
		// True if the given state's width, height and windowed fields are equal
		// to those of the 3d driver's.
		bool		WindowStateSame(DXWindowState state);

		// These methods are used by the resources.
	public:
		HRESULT		CreateTexture2D(DXTexture2DDesc* pDesc, ID3D11Texture2D** ppOutTex);
		HRESULT		CreateResourceViews(DXTexture2DDesc* pDescTex, ID3D11Texture2D* pTex, ID3D11ShaderResourceView** ppSRView, ID3D11RenderTargetView** ppRTView, ID3D11DepthStencilView** ppDSView);
		HRESULT		CreateBuffer(DXHardwareBufferDesc* pDesc, ID3D11Buffer** ppBuffer);
		HRESULT		CreateInputLayout(DXInputLayoutDesc* pDesc, ID3D11InputLayout** ppLayout);
		HRESULT		CompileShader(DXShaderDesc* pDesc, ID3DBlob** ppOutBlob);
		HRESULT		CreateShader(DX_SHADER_TYPE type, ID3DBlob* pShaderData, void** pOutShader);
		HRESULT		CreateBlendState(DXBlendState* pState);
		HRESULT		CreateRasterizerState(DXRasterizerState* pState);
		HRESULT		CreateSamplerState(DXSamplerState* pState);
		HRESULT		CreateDepthStencilState(DXDepthStencilState* pState);


		// Public pipeline state get methods. These methods return the current state
		// of the rendering pipeline.
	public:
		// Is the 3d driver in windowed mode.
		bool				IsWindowed()			const	{ return m_PipeLineState.bWindowed;		};
		// Returns the width of the rendering target.
		UINT				GetBackbufferWidth()	const	{ return m_PipeLineState.uiWidth;		};
		// Returns the height of the rendering target.
		UINT				GetBackbufferHeight()	const	{ return m_PipeLineState.uiHeight;		};
		CDXTexture*			GetRenderTarget()		const	{ return m_PipeLineState.pRenderTarget;	};
		CDXTexture*			GetDepthStencil()		const	{ return m_PipeLineState.pDepthStencil;	};
		DX_PRIMITIVE_TYPE	GetPrimitiveType()		const	{ return m_PipeLineState.primitveType;	};
		CDXShader*			GetVertexShader()		const	{ return m_PipeLineState.pVertexShader;	};
		CDXShader*			GetPixelShader()		const	{ return m_PipeLineState.pPixelShader;	};
		CDXHardwareBuffer*	GetVertexBuffer()		const	{ return m_PipeLineState.pVertexBuffer;	};
		CDXHardwareBuffer*	GetIndexBuffer()		const	{ return m_PipeLineState.pIndexBuffer;	};
		CDXInputLayout*		GetInputLayout()		const	{ return m_PipeLineState.pInputLayout;	};
		DXBlendState*		GetBlendState()			const	{ return m_PipeLineState.pBlendState;	};
		DXRasterizerState*	GetRasterState()		const	{ return m_PipeLineState.pRasterState;	};
		DXDepthStencilState*GetDepthStencilState()	const	{ return m_PipeLineState.pDepthStencilState;	};
		DXViewport			GettViewport()			const	{ return m_PipeLineState.viewport;				};
		CDXTexture*			GetPSTexture(UINT slot)	const	{ return m_PipeLineState.pPSTextures[slot];		};
		DXSamplerState*		GetPSSampler(UINT slot)	const	{ return m_PipeLineState.pPSSamplers[slot];		};
		UINT				GetVertexBufferOffset()	const	{ return m_PipeLineState.uiVertexBufferOffset;	};
		UINT				GetIndexBufferOffset()	const	{ return m_PipeLineState.uiIndexBufferOffset;	};

		// Methods for changing the state of the rendering pipeline. The methods
		// used in high frequency code, therefore they do not perform any validation
		// on the arguments.
	public:
		inline void		SetVertexShader(CDXShader* pShader);
		inline void		SetPixelShader(CDXShader* pShader);
		inline void		SetVertexBuffer(CDXHardwareBuffer* pBuffer, UINT offset=0);
		inline void		SetIndexBuffer(CDXHardwareBuffer* pBuffer, UINT offset=0);
		inline void		SetInputLayout(CDXInputLayout* pLayout);
		inline void		SetPrimitiveType(DX_PRIMITIVE_TYPE type);
		inline void		SetPSConstantBuffer(UINT slot, CDXHardwareBuffer* pBuffer);
		inline void		SetVSConstantBuffer(UINT slot, CDXHardwareBuffer* pBuffer);
		inline void		SetPSTextures(char startSlot, char numTex, CDXTexture** ppTextures);
		inline void		SetPSSampler(char slot, DXSamplerState* ppSampler);
		inline void		SetOutputs(CDXTexture* pRenderTarget=DX_DEFAULT_RENDER_TARGET, CDXTexture* pDepthStencil=DX_DEFAULT_RENDER_TARGET);
		inline void		SetBlendState(DXBlendState* pState, const float fBlendFactors[4]);
		inline void		SetRasterizerState(DXRasterizerState* pState);
		inline void		SetDepthStencilState(DXDepthStencilState* pState, UINT uiStencilRef=1);
		inline void		ClearRenderTarget(float colorRGBA[4]);
		inline void		ClearDepthStencil(float depth, UINT8 stencil, UINT flags);
		inline void		UpdateConstantBuffer(CDXHardwareBuffer* pBuffer, void* pData);
		inline void		Draw(UINT vertexCount, UINT startVertex=0);
		inline void		DrawIndexed(UINT indexCount, UINT startIndex=0);
		inline void		Present();
		inline void		SetViewport(DXViewport* pViewport);
		inline void		ResetViewport();
		inline void		Map(IDXResource* pResource, DX_MAP mapType, DXMappedResource* pOutMappedResource, UINT uiTextureMipLevel=0);
		inline void		Unmap(IDXResource* pResource, UINT uiTextureMipLevel=0);

		// Internal usage only.
	public:
		inline void		SetD3DPSTextureView(ID3D11ShaderResourceView* pView);

	public:
		inline static CDX3DDriver*	GetInstance()
		{
			static CDX3DDriver driver;
			return &driver;
		}

	public:
		virtual UINT		GetTypeId()		const	{ return DX_3DDRIVER_TYPE_ID;		};
		virtual LPCWSTR		GetTypeName()	const	{ return DX_3DDRIVER_TYPE_NAME;		};

	private:
		CDX3DDriver();

	public:
		~CDX3DDriver();
	};

} // DXFramework namespace end


#include "DX3DDriverInline.h"