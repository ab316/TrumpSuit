#pragma once

#include "DXUnknown.h"
#include "DXResourceManager.h"

namespace DXFramework
{ // DXFramework namespace begin

	class CDX3DDriver;
	class CDXTextRenderer;
	class CDXTexture;
	class CDXHardwareBuffer;
	class CDXShader;
	class CDXInputLayout;
	class CDXTextureManager;
	class CDXHardwareBufferManager;
	class CDXInputLayoutManager;
	class CDXShaderManager;

	struct DXBlendState;
	struct DXDepthStencilState;


	struct DXScreenVertex
	{
		XMFLOAT3 pos;
		XMFLOAT2 tex;
	};

	// Graphics driver singleton class. It manages the resource managers. Handles
	// window resizes. It also creates some default resources.
	// Resources created by default by graphics driver
	//		1X1 RGBA_32Bit white (255,255,255,255) texture.
	//		Depth stencil texture compatible with the render target.
	//		Transparent blend state.
	//		Depth stencil state with depth disabled.
	//		Vertex and Pixel shaders for GUI quad.
	//		Vertex and Index buffer for GUI quad (Full-screen quad in projection space)
	//		Constant buffer (4x4 matrix) for GUI quad, text renderer.
	class CDXGraphicsDriver : public IDXUnknown
	{
	private:
		DXWindowState					m_WindowState;
		// 3D driver.
		CDX3DDriver*					m_p3dDriver;
		// Text renderer.
		CDXTextRenderer*				m_pTextRenderer;

		CDXShader*						m_pVSGUI;
		CDXShader*						m_pPSGUI;
		// A vertex buffer to hold vertices of a 2D quad.
		CDXHardwareBuffer*				m_pVB2DQuad;
		// An index buffer to hold indices of a 2D quad.
		CDXHardwareBuffer*				m_pIB2DQuad;
		CDXHardwareBuffer*				m_pCBGUITransform;
		// An input layout containing only position and texture coords.
		CDXInputLayout*					m_pILPosTex;
		CDXTexture*						m_pTexDepthStencil;
		CDXTexture*						m_pTex1x1White;
		// A transparent blend state.
		DXBlendState*					m_pBSTransparent;
		DXDepthStencilState*			m_pDSNoDepth;
		// Texture manager.
		CDXTextureManager*				m_pManTexture;
		// Hardware buffers (vertex, index, constant) manager.
		CDXHardwareBufferManager*		m_pManHardwareBuffer;
		// Input layout manager.
		CDXInputLayoutManager*			m_pManInputLayout;
		// Shader (vertex, pixel) manager.
		CDXShaderManager*				m_pManShader;
		bool							m_bStarted;

	// Public internal system tasks.
	public:
		// Starts the graphics driver, building the hardware device and initial resources.
		HRESULT			Startup(HWND hWnd);
		// Creates or recreates the window's size dependent resources. Which
		// include the render target and depth stencil and their views. Also
		// Recreates the 2D Driver resources.
		HRESULT			CreateWindowDependentResources();
		// This method is required when the window's state changes (width, height or
		// fullscreen/windowed).
		HRESULT			HandleWindowResize();
		void			Shutdown();
		// Some pre-rendering tasks.
		void			BeginRendering();
		// Post-rendering tasks. E.g. presenting.
		void			EndRendering();

	// Private system tasks.
	private:
		HRESULT			CreateFullScreenQuadResources();
		// Acquires the new dimensions of the window on which rendering would be done.
		void			UpdateWindowDimensions();

		 // Public get functions.
	public:
		// Returns the width of region of the window used for rendering. May not be the same as
		// resolution width returned by the 3d driver.
		UINT				GetClientWidth()		const	{ return m_WindowState.uiWidth;		};
		// Returns the height of region of the window used for rendering. May not be the same as
		// resolution height returned by the 3d driver.
		UINT				GetClientHeight()		const	{ return m_WindowState.uiHeight;	};
		CDX3DDriver*		Get3DDriver()			const   { return m_p3dDriver;				};
		CDXInputLayout*		GetPosTextInputLayout()	const	{ return m_pILPosTex;				};
		CDXTexture*			GetDefaultTexture()		const	{ return m_pTex1x1White;			};
		CDXHardwareBuffer*	GetQuadVertexBuffer()	const	{ return m_pVB2DQuad;				};
		CDXHardwareBuffer*	GetQuadIndexBuffer()	const	{ return m_pIB2DQuad;				};
		CDXHardwareBuffer*	GetGUIConstantBuffer()	const	{ return m_pCBGUITransform;			};
		CDXShader*			GetQuadVertexShader()	const	{ return m_pVSGUI;					};
		CDXShader*			GetQuadPixelShader()	const	{ return m_pPSGUI;					};
		DXBlendState*		GetTransparentBlend()	const	{ return m_pBSTransparent;			};
		DXDepthStencilState*GetNoDepthState()		const	{ return m_pDSNoDepth;				};

	// Public tasks.
	public:
		//// CUSTOM KEY REQUIREMENTS:
		// TEXTURE: 	// Custom key is must for non-file textures. Optional for file textures.
						// Keep file names small.
		// SHADER:		// Custom key is NOT recommended. Keep file names small (less than 30 chars).
		// BUFFER:		// Custom key must be provided. The key will be prefixed to reflect the type
						// of the buffer. Keep key name small. (50 chars max).
		// INPUT LAYOUT:// Custom key is NOT recommended.
		DXResourceHandle	CreateResource(DX_RESOURCE_TYPE type, void* pDesc, WCHAR* szCustomKey=0);
		HRESULT				RecreateResource(DXResourceHandle* pHandle);
		HRESULT				CreateAndRecreateResource(DX_RESOURCE_TYPE type, void* pDesc, IDXResource** ppOutResource, WCHAR* szCustomKey=0);

		// Clears the render and depth stencil views.
		void	Clear(bool bRenderTarget, float fColor[4]);

	public:
		static CDXGraphicsDriver*	GetInstance()
		{
			static CDXGraphicsDriver driver;
			return &driver;
		}

	public:
		virtual UINT		GetTypeId()		const	{ return DX_GRAPHICSDRIVER_TYPE_ID;		};
		virtual LPCWSTR		GetTypeName()	const	{ return DX_GRAPHICSDRIVER_TYPE_NAME;	};

	private:
		// Default ctor.
		CDXGraphicsDriver();

	public:
		// Default dtor.
		~CDXGraphicsDriver();
	};

} // DXFramework namespace end