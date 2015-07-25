#pragma once

#include "DXUnknown.h"

namespace DXFramework
{ // DXFramework namespace begin

	class  CDXHardwareBuffer;
	class  CDXInputLayout;
	class  CDXShader;
	class  CDX3DDriver;
	
	struct DXMappedResource;

	// A sprite vertex.
	struct DXSpriteVertex
	{
		XMFLOAT4	f4Color;
		XMFLOAT3	f3Pos;
		XMFLOAT2	f2Tex;
	};

	// A DXSprite is used for batch rendering 2D quads. The quads are drawn when EndBatchRendering()
	// or Flush() is called or when the buffer gets full.
	class CDXSprite : public IDXUnknown
	{
	public:
		CDX3DDriver*			m_p3dDriver;
		// The sprite input layout.
		CDXInputLayout*			m_pILSprites;
		// The sprite vertex buffer.
		CDXHardwareBuffer*		m_pVBSprites;
		// Temporary pointer to vertex buffer for saving state of pipeline before rendering.
		CDXHardwareBuffer*		m_pVBTemp;
		// Temporary pointer to input layout for saving pre-render pipeline state.
		CDXInputLayout*			m_pILTemp;
		DXMappedResource*		m_pMappedVB;
		// The number of vertices the vertex buffer can hold.
		UINT					m_uiMaxVertices;
		// The number of vertices currently in the rendering batch.
		UINT					m_uiVerticesInBatch;
		// Temporary storage of vertex buffer offset of pipeline.
		UINT					m_uiVBOffsetTemp;
		float					m_fReciprocalBackBufferWidth;
		float					m_fReciprocalBackBufferHeight;
		// True b/w Begin/End-BatchRendering() calls.
		bool					m_bRendering;
		// True when the vertex buffer is locked.
		bool					m_bVBMapped;
		bool					m_bConvertToClipSpace;

	public:
		// Set to true if the coords input to pRect in BatchRenderSprite() are in screen
		// space. Default is true. Any method which calls this method with false must reset
		// it back to true.
		void		SetInputRectCoordMode(bool bCoordsInScreenSpace)
		{
			m_bConvertToClipSpace = bCoordsInScreenSpace;
		}

	public:
		HRESULT					Create(WCHAR* szName, CDXShader* pSignatureVS, UINT uiMaxVertices);
		void					Destroy();
		// Needed after creation and when the back buffer size changes.
		void					OnBackBufferResize(float fWidth, float fHeight);
		// Setups the pipeline state for rendering.
		void					BeginBatchRendering();
		// Restores the changed pipeline state and renders any sprites in batch.
		void					EndBatchRendering();
		///<summary>Adds a sprite to the rendering batch.</summary>
		///<param name="pRect">If SetInputRectCoordMode() is called with true, the coords are treated as given in
		// screen space, otherwise they are taken as given in projection space.
		void					BatchRenderSprite(DXRect* pRect, DXRect* pRectTexture, XMFLOAT4* pColor, float fDepth);
		// Renders all the sprites in the batch.
		void					Flush();

	public:
		virtual UINT		GetTypeId()		const	{ return DX_SPRITE_TYPE_ID;		};
		virtual LPCWSTR		GetTypeName()	const	{ return DX_SPRITE_TYPE_NAME;	};
		
	public:
		CDXSprite();
		~CDXSprite();
	};

} // DXFramework namespace end