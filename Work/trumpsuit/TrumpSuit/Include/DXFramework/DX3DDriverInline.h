namespace DXFramework
{ // DXFramework namespace begin

	void CDX3DDriver::SetVertexShader(CDXShader* pShader)
	{
		if (m_PipeLineState.pVertexShader != pShader)
		{
			m_PipeLineState.pVertexShader = pShader;
			m_pImmediateContext->VSSetShader(pShader ? (ID3D11VertexShader*)pShader->GetD3DShader() : nullptr, nullptr, 0);
		}
	}


	void CDX3DDriver::SetPixelShader(CDXShader* pShader)
	{
		if (m_PipeLineState.pPixelShader != pShader)
		{
			m_PipeLineState.pPixelShader = pShader;
			m_pImmediateContext->PSSetShader(pShader ? (ID3D11PixelShader*)pShader->GetD3DShader() : nullptr, nullptr, 0);
		}
	}


	// Offset is the first byte in the buffer that will be used. If not 0,
	// it should be a multiple of the buffer stride.
	void CDX3DDriver::SetVertexBuffer(CDXHardwareBuffer* pBuffer, UINT offset)
	{
		if (m_PipeLineState.pVertexBuffer != pBuffer || m_PipeLineState.uiVertexBufferOffset != offset)
		{
			m_PipeLineState.pVertexBuffer = pBuffer;
			m_PipeLineState.uiVertexBufferOffset = offset;
			ID3D11Buffer* p;
			UINT stride;

			if (pBuffer)
			{
				p = pBuffer->GetD3DBuffer();

				const DXHardwareBufferDesc* pDesc = (const DXHardwareBufferDesc*)pBuffer->GetDesc();
				stride = pDesc->uiBufferStride;
				m_pImmediateContext->IASetVertexBuffers(0, 1, &p, &stride, &offset);
			}
			else
			{
				p = nullptr;
				stride = 0;
				m_pImmediateContext->IASetVertexBuffers(0, 1, &p, &stride, &offset);
			}
		}
	}


	// Offset is the first byte of the buffer to use.
	void CDX3DDriver::SetIndexBuffer(CDXHardwareBuffer* pBuffer, UINT offset)
	{
		if (m_PipeLineState.pIndexBuffer != pBuffer || m_PipeLineState.uiIndexBufferOffset != offset)
		{
			m_PipeLineState.pIndexBuffer = pBuffer;
			m_PipeLineState.uiIndexBufferOffset = offset;

			if (pBuffer)
			{
				m_pImmediateContext->IASetIndexBuffer(pBuffer->GetD3DBuffer(), DXGI_FORMAT_R16_UINT, offset);
			}
			else
			{
				m_pImmediateContext->IASetIndexBuffer(nullptr, DXGI_FORMAT_R16_UINT, 0);
			}
		}
	}


	void CDX3DDriver::SetInputLayout(CDXInputLayout* pLayout)
	{
		if (m_PipeLineState.pInputLayout != pLayout)
		{
			m_PipeLineState.pInputLayout = pLayout;
			m_pImmediateContext->IASetInputLayout(pLayout ? pLayout->GetD3DLayout() : nullptr);
		}
	}
	
	
	void CDX3DDriver::SetPrimitiveType(DX_PRIMITIVE_TYPE type)
	{
		if (m_PipeLineState.primitveType != type)
		{
			m_PipeLineState.primitveType = type;
			m_pImmediateContext->IASetPrimitiveTopology((D3D11_PRIMITIVE_TOPOLOGY)type);
		}
	}
	
	
	// Sets the constant buffers for the pixel shader. There are 15 slots. 0 - 14.
	void CDX3DDriver::SetPSConstantBuffer(UINT slot, CDXHardwareBuffer* pBuffer)
	{
		ID3D11Buffer* p = pBuffer->GetD3DBuffer();
		m_pImmediateContext->PSSetConstantBuffers(slot, 1, &p);
	}
	
	
	// Sets the constant buffers for the vertex shader. There are 15 slots. 0 - 14.
	void CDX3DDriver::SetVSConstantBuffer(UINT slot, CDXHardwareBuffer* pBuffer)
	{
		ID3D11Buffer* p = pBuffer->GetD3DBuffer();
		m_pImmediateContext->VSSetConstantBuffers(slot, 1, &p);
	}
	

	// uiStartSlot is the texture register in which to place the first texture.
	// Only DX_MAX_PIPELINE_TEXTURES_SLOTS texture slots are supported.
	// startSlot and numTex are not validated.
	void CDX3DDriver::SetPSTextures(char startSlot, char numTex, CDXTexture** ppTextures)
	{
		static ID3D11ShaderResourceView* pTexturesViews[DX_MAX_PIPELINE_TEXTURES_SLOTS];

		bool bChange = false;
		for (char i=0; i<numTex; i++)
		{
			char j = i + startSlot;
			if (m_PipeLineState.pPSTextures[j] != ppTextures[i])
			{
				bChange = true;
				m_PipeLineState.pPSTextures[j] = ppTextures[i];
				if (ppTextures[i])
				{
					pTexturesViews[i] = ppTextures[i]->GetShaderResourceView();
				}
				else
				{
					pTexturesViews[i] = 0;
				}
			}
		}

		if (bChange)
		{
			m_pImmediateContext->PSSetShaderResources(startSlot, numTex, pTexturesViews);
		}
	}


	// Sets the pixel shader sampler states. DX_MAX_PIPELINE_SAMPLERS_SLOTS slots are available.
	// Give 0 to use the default sampler.
	inline void CDX3DDriver::SetPSSampler(char slot, DXSamplerState* pSampler)
	{
		if (m_PipeLineState.pPSSamplers[slot] != pSampler)
		{
			if (pSampler)
			{
				m_PipeLineState.pPSSamplers[slot] = pSampler;
				m_pImmediateContext->PSSetSamplers((UINT)slot, 1, &pSampler->m_pSamplerState);
			}
			else
			{
				m_PipeLineState.pPSSamplers[slot] = m_pSamplerDefault;
				m_pImmediateContext->PSSetSamplers((UINT)slot, 1, &m_pSamplerDefault->m_pSamplerState);
			}
		}
	}


	// DX_DEFAULT_RENDER_TARGET for render target means the main render target.
	// DX_DEFAULT_RENDER_TARGET for depth stencil means that it remains unchanged. 
	// Zeroing means set to null.
	void CDX3DDriver::SetOutputs(CDXTexture* pRenderTarget, CDXTexture* pDepthStencil)
	{
		// Set both new render and depth stencil views to null.
		ID3D11RenderTargetView* pRT = nullptr;
		ID3D11DepthStencilView* pDS = nullptr;
		
		// If pRenderTarget is DX_DEFAULT_RENDER_TARGET, use the main render target.
		if (pRenderTarget == DX_DEFAULT_RENDER_TARGET)
		{
			pRT = m_pRenderTargetView;
		}
		else if (pRenderTarget != nullptr) // Render target provided?
		{
			pRT = pRenderTarget->GetRenderTargetView();
		}
		m_PipeLineState.pRenderTarget = pRenderTarget;

		// If pDepthStencil is DX_DEFAULT_RENDER_TARGET, use the existing depth stencil.
		if (pDepthStencil == DX_DEFAULT_RENDER_TARGET)
		{
			pDepthStencil = m_PipeLineState.pDepthStencil;
			if (pDepthStencil)
			{
				pDS = pDepthStencil->GetDepthStencilView();
			}
		}
		else if (pDepthStencil != nullptr) // Depth stencil provided.
		{
			m_PipeLineState.pDepthStencil = pDepthStencil;
			pDS = pDepthStencil->GetDepthStencilView();
		}
		
		// Set the new targets.
		m_pImmediateContext->OMSetRenderTargets(1, &pRT, pDS);
	}


	// Sets the blend state of the pipeline. Set to 0 to reset, fBlendFactors can be set to
	// 0 as long as the blend state does not use the blend factors.
	inline void CDX3DDriver::SetBlendState(DXBlendState* pState, const float fBlendFactors[4])
	{
		if (m_PipeLineState.pBlendState != pState)
		{
			m_PipeLineState.pBlendState = pState;
			if (pState)
			{
				m_pImmediateContext->OMSetBlendState(pState->m_pBlendState, fBlendFactors, 0xffffffff);
			}
			else
			{
				m_pImmediateContext->OMSetBlendState(m_vecpBlendStates[0], nullptr, 0xffffffff);
			}
		}
	}


	// Set to 0 to reset.
	inline void CDX3DDriver::SetRasterizerState(DXRasterizerState* pState)
	{
		if (m_PipeLineState.pRasterState != pState)
		{
			m_PipeLineState.pRasterState = pState;
			if (pState)
			{
				m_pImmediateContext->RSSetState(pState->m_pRasterState);
			}
			else
			{
				m_pImmediateContext->RSSetState(m_vecpRasterStates[0]);
			}
		}
	}


	inline void CDX3DDriver::SetDepthStencilState(DXDepthStencilState* pState, UINT uiStencilRef)
	{
		if (m_PipeLineState.pDepthStencilState != pState)
		{
			m_PipeLineState.pDepthStencilState = pState;
			if (pState)
			{
				m_pImmediateContext->OMSetDepthStencilState(pState->m_pDepthStencilState, uiStencilRef);
			}
			else
			{
				m_pImmediateContext->OMSetDepthStencilState(m_vecpDepthStencilStates[0], uiStencilRef);
			}
		}
	}


	// Clears the current render target.
	inline void CDX3DDriver::ClearRenderTarget(float colorRGBA[4])
	{
		if (m_PipeLineState.pRenderTarget == DX_DEFAULT_RENDER_TARGET)
		{
			m_pImmediateContext->ClearRenderTargetView(m_pRenderTargetView, colorRGBA);
		}
		else if (m_PipeLineState.pRenderTarget)
		{
			m_pImmediateContext->ClearRenderTargetView(m_PipeLineState.pRenderTarget->GetRenderTargetView(), colorRGBA);
		}
	}


	// Clears the current depth stencil texture.
	inline void CDX3DDriver::ClearDepthStencil(float depth, UINT8 stencil, UINT flags)
	{
		if (m_PipeLineState.pDepthStencil)
		{
			m_pImmediateContext->ClearDepthStencilView(m_PipeLineState.pDepthStencil->GetDepthStencilView(), flags, depth, stencil);
		}
	}

	
	// Updates the GPU constant buffer data with the given data. The size of pData
	// must be the size of the constant buffer provided. The resource must NOT be created
	// with DX_GPU_READ_ONLY usage.
	inline void CDX3DDriver::UpdateConstantBuffer(CDXHardwareBuffer* pBuffer, void* pData)
	{
		DX_GPU_RESOURCE_USAGE uiUsage = ((const DXHardwareBufferDesc*)pBuffer->GetDesc())->gpuUsage;
		// If the resource has no cpu access. It is an unmappable resource. It is updated as:
		if (uiUsage == DX_GPU_READ_WRITE_ONLY)
		{
			m_pImmediateContext->UpdateSubresource(pBuffer->GetD3DBuffer(), 0, nullptr, pData, 0, 0);
		}
		else // Otherwise, a mappable resource.
		{
			ID3D11Buffer* pD3DBuffer = pBuffer->GetD3DBuffer();

			D3D11_MAPPED_SUBRESOURCE map;
			m_pImmediateContext->Map(pD3DBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &map);
			map.pData = pData;
			m_pImmediateContext->Unmap(pD3DBuffer, 0);
		}
	}


	// startVertex is the first vertex to use from the vertex buffer. Note that
	// the SetVertexBuffer() method also has an offset. startVertex can therefore be
	// thought of as an additional offset.
	inline void CDX3DDriver::Draw(UINT vertexCount, UINT startVertex)
	{
		m_pImmediateContext->Draw(vertexCount, startVertex);
	}


	// startIndex is an additional offset into the assigned indices.
	inline void CDX3DDriver::DrawIndexed(UINT indexCount, UINT startIndex)
	{
		m_pImmediateContext->DrawIndexed(indexCount, startIndex, 0);
	}

	
	// This should be called after to display the output on the screen.
	// Necessary after finished rendering.
	inline void CDX3DDriver::Present()
	{
		m_pMainSwapChain->Present(0, 0);
	}


	inline void CDX3DDriver::SetViewport(DXViewport* pViewport)
	{
		m_PipeLineState.viewport = *pViewport;
		
		D3D11_VIEWPORT port;
		port.Width = pViewport->fWidth;
		port.Height = pViewport->fHeight;
		port.TopLeftX = pViewport->fTopLeftX;
		port.TopLeftY = pViewport->fTopLeftY;
		port.MinDepth = pViewport->fMinDepth;
		port.MaxDepth = pViewport->fMaxDepth;

		m_pImmediateContext->RSSetViewports(1, &port);
	}
	
	
	inline void CDX3DDriver::ResetViewport()
	{
		DXViewport* pPort = &m_PipeLineState.viewport;
		pPort->fWidth = (float)m_PipeLineState.uiWidth;
		pPort->fHeight = (float)m_PipeLineState.uiHeight;
		pPort->fTopLeftX = 0.0f;
		pPort->fTopLeftY = 0.0f;
		pPort->fMinDepth = 0.0f;
		pPort->fMaxDepth = 1.0f;

		SetViewport(pPort);
	}


	inline void CDX3DDriver::SetD3DPSTextureView(ID3D11ShaderResourceView* pView)
	{
		m_PipeLineState.pPSTextures[0] = 0;
		m_pImmediateContext->PSSetShaderResources(0, 1, &pView);
	}


	// Gives CPU access to a mappable resource. Only for Hardware Buffers and Textures.
	// Resource must have been created with one of the usage flags that allow CPU access.
	// mapType, then must be of the corresponding type. If a texture is supplied, it must
	// not be created with a DX_BIND_DEPTH_STENCIL flag and must no be multisampled.
	///<param name="mapType">One of DX_MAP flags</param>
	void CDX3DDriver::Map(IDXResource* pResource, DX_MAP mapType, DXMappedResource* pOutMappedResource, UINT uiTextureMipLevel)
	{
		D3D11_MAPPED_SUBRESOURCE mapped;
		mapped.pData = nullptr;
		mapped.DepthPitch = 0;
		mapped.RowPitch = 0;

		switch (pResource->GetResourceType())
		{
		case DX_RESOURCE_TYPE_TEXTURE:
			m_pImmediateContext->Map( ((CDXTexture*)(pResource))->GetD3DTexture(), uiTextureMipLevel, (D3D11_MAP)mapType, 0, &mapped);
			break;

		case DX_RESOURCE_TYPE_HARDWARE_BUFFER:
			m_pImmediateContext->Map( ((CDXHardwareBuffer*)(pResource))->GetD3DBuffer(), 0, (D3D11_MAP)mapType, 0, &mapped);
			break;
		}

		pOutMappedResource->pData = mapped.pData;
		pOutMappedResource->uiRowPitch = mapped.RowPitch;
		pOutMappedResource->uiDepthPitch = mapped.DepthPitch;
	}


	// Call to Map() must be followed by a call to Unmap() on the same resource.
	void CDX3DDriver::Unmap(IDXResource* pResource, UINT uiTextureMipLevel)
	{
		switch (pResource->GetResourceType())
		{
		case DX_RESOURCE_TYPE_TEXTURE:
			m_pImmediateContext->Unmap( ((CDXTexture*)(pResource))->GetD3DTexture(), uiTextureMipLevel);
			break;

		case DX_RESOURCE_TYPE_HARDWARE_BUFFER:
			m_pImmediateContext->Unmap( ((CDXHardwareBuffer*)(pResource))->GetD3DBuffer(), 0);
			break;
		}
	}

} // DXFramework namespace end