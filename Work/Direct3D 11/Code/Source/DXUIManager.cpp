#include "DXStdafx.h"
#include "DXMouseHandler.h"
#include "DXKeyboardHandler.h"

#include "DXShader.h"
#include "DXTexture.h"
#include "DX3DDriver.h"
#include "DXGraphicsDriver.h"
#include "DXSprite.h"
#include "DXTextRenderer.h"

#include "DXUIElement.h"
#include "DXUIButton.h"
#include "DXUIEditBox.h"
#include "DXUIStatic.h"
#include "DXUICheckBox.h"
#include "DXUIRadioButton.h"
#include "DXUIScrollBar.h"
#include "DXUIManager.h"


namespace DXFramework
{ // DXFramework namespace begin

	CDXUIManager::CDXUIManager()	:	m_pTexUIControls(nullptr),
										m_pCallbackOnUIMessage(nullptr),
										m_pKeyboardFocussedElement(nullptr),
										m_pVSGUI(nullptr),
										m_pPSGUI(nullptr),
										m_fBackBufferWidth(0),
										m_fBackBufferHeight(0),
										m_bIsRendering(false)
	{
		m_pSprite = new CDXSprite();
		UpdateVectorIterators();
	}


	CDXUIManager::~CDXUIManager()
	{
		DX_SAFE_DELETE(m_pSprite);

		DXUIElementVectorIterator iter = m_vecpUIElements.begin();
		DXUIElementVectorIterator end = m_vecpUIElements.end();
		while (iter != end)
		{
			DX_SAFE_DELETE((*iter)->pElement);
			DX_SAFE_DELETE(*iter);
			iter++;
		}
		m_vecpUIElements.clear();


		DXUIRadioButtonGroupMapIterator radioIter = m_mapRadioButtonGroups.begin();
		DXUIRadioButtonGroupMapIterator radioEnd = m_mapRadioButtonGroups.end();
		while (radioIter != radioEnd)
		{
			DX_SAFE_DELETE(radioIter->second);
			radioIter++;
		}
		m_mapRadioButtonGroups.clear();
	}


	void CDXUIManager::OnBackBufferResize(float fWidth, float fHeight)
	{
		m_fBackBufferWidth = fWidth;
		m_fBackBufferHeight = fHeight;
		m_pSprite->OnBackBufferResize(fWidth, fHeight);
	}


	bool CDXUIManager::HasElement(IDXUIElement* pElement)
	{
		DXUIElementVectorIterator iter = m_iterpUIElementsBegin;
		DXUIElementVectorIterator end = m_iterpUIElementsEnd;

		while (iter != end)
		{
			if ((*iter)->pElement == pElement)
			{
				return true;
			}
			++iter;
		}

		return false;
	}


	bool CDXUIManager::HasElement(UINT id)
	{
		DXUIElementVectorIterator iter = m_iterpUIElementsBegin;
		DXUIElementVectorIterator end = m_iterpUIElementsEnd;

		while (iter != end)
		{
			if ((*iter)->uiID == id)
			{
				return true;
			}
			++iter;
		}

		return false;
	}


	IDXUIElement* CDXUIManager::GetElement(UINT id)
	{
		DXUIElementVectorIterator iter = m_iterpUIElementsBegin;
		DXUIElementVectorIterator end = m_iterpUIElementsEnd;

		while (iter != end)
		{
			if ((*iter)->uiID == id)
			{
				return (*iter)->pElement;
			}
			++iter;
		}

		return nullptr;
	}


	UINT CDXUIManager::GetElementID(IDXUIElement* pElement)
	{
		DXUIElementVectorIterator iter = m_iterpUIElementsBegin;
		DXUIElementVectorIterator end = m_iterpUIElementsEnd;

		while (iter != end)
		{
			if ((*iter)->pElement == pElement)
			{
				return (*iter)->uiID;
			}
			++iter;
		}

		return DX_INVALID_UIELEMENT_ID;
	}


	bool CDXUIManager::InCorrectZOrder(DXUIElement* pFirst, DXUIElement* pSecond)
	{
		return (pFirst->pElement->m_cZOrder > pSecond->pElement->m_cZOrder);
	}


	void CDXUIManager::SortElements()
	{
		DXUIElementVectorIterator start = m_iterpUIElementsBegin;
		DXUIElementVectorIterator end = m_iterpUIElementsEnd;
		std::sort(start, end, InCorrectZOrder);
	}


	bool CDXUIManager::AddButton(UINT id, WCHAR* szCaption, float fX, float fY, float fWidth, float fHeight)
	{
		DXUIElement* pNew = AddElement(id);
		if (pNew)
		{
			CDXUIButton* pButton = new CDXUIButton();
			if (FAILED(pButton->Create(szCaption, fX, fY, fWidth, fHeight)))
			{
				DX_SAFE_DELETE(pButton);
				RemoveElement(pNew);

				return false;
			}

			pButton->m_pUIManager = this;
			pButton->m_pSprite = m_pSprite;

			pNew->pElement = pButton;
		}

		return true;
	}


	bool CDXUIManager::AddEditBox(UINT id, UINT maxLength, float fUpperLeftX, float fUpperLeftY, float fWidth, float fHeight)
	{
		DXUIElement* pNew = AddElement(id);
		if (pNew)
		{
			CDXUIEditBox* pEdit = new CDXUIEditBox();
			if (FAILED(pEdit->Create(maxLength, fUpperLeftX, fUpperLeftY, fWidth, fHeight)))
			{
				DX_SAFE_DELETE(pEdit);
				RemoveElement(pNew);

				return false;
			}
			pEdit->m_pUIManager = this;
			pEdit->m_pSprite = m_pSprite;

			pNew->pElement = pEdit;
		}

		return true;
	}


	bool CDXUIManager::AddStatic(UINT id, WCHAR* szText, float fX, float fY, float fFontSize)
	{
		DXUIElement* pNew = AddElement(id);
		if (pNew)
		{
			CDXUIStatic* pStatic = new CDXUIStatic();
			if (FAILED(pStatic->Create(szText, fX, fY, fFontSize)))
			{
				DX_SAFE_DELETE(pStatic);
				RemoveElement(pNew);

				return false;
			}

			pNew->pElement = pStatic;
		}

		return true;
	}


	bool CDXUIManager::AddCheckBox(UINT id, float fX, float fY, float fSide, bool bChecked)
	{
		DXUIElement* pNew = AddElement(id);
		if (pNew)
		{
			CDXUICheckBox* pCheck = new CDXUICheckBox();
			if (FAILED(pCheck->Create(fX, fY, fSide, bChecked)))
			{
				DX_SAFE_DELETE(pCheck);
				RemoveElement(pNew);

				return false;
			}
			pCheck->m_pSprite = m_pSprite;
			pCheck->m_pUIManager = this;

			pNew->pElement = pCheck;
		}

		return true;
	}


	bool CDXUIManager::AddRadioButton(UINT id, UINT group, float fCentreX, float fCentreY, float fRadius)
	{
		DXUIElement* pNew = AddElement(id);
		if (pNew)
		{
			DXRadioButtonGroup* pGroup;
			DXUIRadioButtonGroupMapIterator iter = m_mapRadioButtonGroups.find(group);
			if (iter != m_mapRadioButtonGroups.end())
			{
				pGroup = iter->second;
			}
			else
			{
				pGroup = new DXRadioButtonGroup();
				pGroup->uiActiveId = (UINT)-1;
				m_mapRadioButtonGroups.insert(DXUIRadioButtonGroupMap::value_type(group, pGroup));
			}

			CDXUIRadioButton* pRadio = new CDXUIRadioButton();
			if (FAILED(pRadio->Create(pGroup, fCentreX, fCentreY, fRadius)))
			{
				DX_SAFE_DELETE(pRadio);
				RemoveElement(pNew);

				return false;
			}
			pRadio->m_pSprite = m_pSprite;
			pRadio->m_pUIManager = this;

			pNew->pElement = pRadio;
		}

		return true;
	}


	bool CDXUIManager::AddScrollBar(UINT id, float fX, float fY, float fLength, UINT uiNumUnits, bool bVertical, float fThickness)
	{
		DXUIElement* pNew = AddElement(id);
		if (pNew)
		{
			CDXUIScrollBar* pScroll = new CDXUIScrollBar();
			if (FAILED(pScroll->Create(fX, fY, fLength, fThickness, uiNumUnits, bVertical)))
			{
				DX_SAFE_DELETE(pScroll);
				RemoveElement(pNew);

				return false;
			}

			pScroll->m_pUIManager = this;
			pScroll->m_pSprite = m_pSprite;

			pNew->pElement = pScroll;
		}

		return true;
	}


	DXUIElement* CDXUIManager::AddElement(UINT id)
	{
		bool bAlreadyExists = HasElement(id);

		if (!bAlreadyExists)
		{
			DXUIElement* pNewElement = new DXUIElement();
			pNewElement->uiID = id;

			m_vecpUIElements.push_back(pNewElement);

			UpdateVectorIterators();

			return pNewElement;
		}

		return (DXUIElement*)nullptr;
	}


	void CDXUIManager::RemoveElement(DXUIElement* pElement)
	{
		DXUIElementVectorIterator iter = m_iterpUIElementsBegin;
		while (iter != m_iterpUIElementsEnd)
		{
			if (*iter == pElement)
			{
				DX_SAFE_DELETE(pElement);
				m_vecpUIElements.erase(iter);
				break;
			}
			iter++;
		}

		UpdateVectorIterators();
	}


	IDXUIElement* CDXUIManager::GetElementAtPoint(DXPoint point, bool bActiveOnly, bool bVisibleOnly)
	{
		DXUIElementVectorIterator iter = m_vecpUIElements.begin();
		DXUIElementVectorIterator end = m_vecpUIElements.end();
		IDXUIElement* pReturnElement = nullptr;
		UINT zOrder = (UINT)-1;
		while (iter != end)
		{
			IDXUIElement* pElement = (*iter)->pElement;
			// If active only is specified and element is not active OR visible only is true
			// and element is not visible, then we continue to the next element.
			if ( (bActiveOnly && !pElement->m_bActive) || (bVisibleOnly && !pElement->m_bVisible) )
			{
				++iter;
				continue;
			}


			const DXRect* pRect = pElement->GetRect();
			if ( DX_POINT_IN_RECT(point, *pRect) )
			{
				if ((UINT)pElement->m_cZOrder < zOrder)
				{
					pReturnElement = pElement;
					zOrder = (UINT)pElement->m_cZOrder;
				}
			}
			++iter;
		}
		return pReturnElement;
	}


	HRESULT CDXUIManager::CreateResources()
	{
		HRESULT hr = S_OK;

		m_p3dDriver = CDX3DDriver::GetInstance();

		////////////////////////// TEXTURE ////////////////////////////
		CDXGraphicsDriver* pGraphics = CDXGraphicsDriver::GetInstance();
		DXTexture2DDesc descTex;
		descTex.bFileTexture = true;
		descTex.bindFlags = DX_BIND_SHADER_RESOURCE;
		descTex.gpuUsage = DX_GPU_READ_ONLY;
		wcscpy_s(descTex.textureData.szFileName, L"UIControls.dds");
		DX_V(pGraphics->CreateAndRecreateResource(DX_RESOURCE_TYPE_TEXTURE, &descTex, (IDXResource**)&m_pTexUIControls));
		if (FAILED(hr))
		{
			DX_DEBUG_OUTPUT0(L"ERROR!!! Unable to create texture.");
			return hr;
		}
		DX_RESOURCE_SET_DEBUG_NAME(m_pTexUIControls, "UI Controls");
		/////////////////////////////////////////////////////////////////////


		////////////////////////// VERTEX SHADER ////////////////////////////
		DXShaderDesc descShader;
		wcscpy_s(descShader.szFileName, DX_MAX_FILE_NAME, L"GUI.hlsl");
		wcscpy_s(descShader.szEntryPoint, DX_MAX_SHADER_ENTRY, L"VS");
		wcscpy_s(descShader.szShaderLevel, DX_MAX_SHADER_PROFILE, L"vs_4_0");
		descShader.type = DX_SHADER_VERTEX_SHADER;
		DX_V(pGraphics->CreateAndRecreateResource(DX_RESOURCE_TYPE_SHADER, &descShader, (IDXResource**)&m_pVSGUI));
		if (FAILED(hr))
		{
			DX_DEBUG_OUTPUT0(L"ERROR!!! Unable to create vertex shader.");
			return hr;
		}
		DX_RESOURCE_SET_DEBUG_NAME(m_pVSGUI, "UI VS");
		////////////////////////////////////////////////////////////////////



		////////////////////////// PIXEL SHADER ////////////////////////////
		wcscpy_s(descShader.szFileName, DX_MAX_FILE_NAME, L"GUI.hlsl");
		wcscpy_s(descShader.szEntryPoint, DX_MAX_SHADER_ENTRY, L"PS");
		wcscpy_s(descShader.szShaderLevel, DX_MAX_SHADER_PROFILE, L"ps_4_0");
		descShader.type = DX_SHADER_PIXEL_SHADER;
		DX_V(pGraphics->CreateAndRecreateResource(DX_RESOURCE_TYPE_SHADER, &descShader, (IDXResource**)&m_pPSGUI));
		if (FAILED(hr))
		{
			DX_DEBUG_OUTPUT0(L"ERROR!!! Unable to create pixel shader.");
			return hr;
		}
		DX_RESOURCE_SET_DEBUG_NAME(m_pPSGUI, "UI PS");
		////////////////////////////////////////////////////////////////////


		m_pSprite->Create(L"UI Sprite", m_pVSGUI, DX_MAX_UI_SPRITES_VERTICES);

		return hr;
	}


	void CDXUIManager::DestroyResources()
	{
		m_pSprite->Destroy();
		DX_SAFE_RELEASE(m_pTexUIControls);
		DX_SAFE_RELEASE(m_pVSGUI);
		DX_SAFE_RELEASE(m_pPSGUI);
	}


	void CDXUIManager::Update(float fDelta)
	{
		DXUIElementVectorIterator iter = m_vecpUIElements.begin();
		DXUIElementVectorIterator end = m_vecpUIElements.end();
		while (iter != end)
		{
			IDXUIElement* pElement = (*iter)->pElement;
			if (pElement->IsVisible())
			{
				pElement->Update(fDelta);
			}
			++iter;
		}
	}


	void CDXUIManager::Render()
	{
		DX_PIX_EVENT_BEGIN(0xff0000ff, L"UI Manager Render");

		static CDXGraphicsDriver* pGraphics = CDXGraphicsDriver::GetInstance();
		static CDXTextRenderer* pText = CDXTextRenderer::GetInstance();
		static DXBlendState* pBSTransparent = pGraphics->GetTransparentBlend();
		static DXDepthStencilState* pDSNoDepth = pGraphics->GetNoDepthState();
		static DXDepthStencilState* pDSTemp = m_p3dDriver->GetDepthStencilState();

		m_p3dDriver->SetVertexShader(m_pVSGUI);
		m_p3dDriver->SetPixelShader(m_pPSGUI);
		m_p3dDriver->SetPSTextures(0, 1, &m_pTexUIControls);
		m_p3dDriver->SetBlendState(pBSTransparent, nullptr);
		// Don't want to write to the depth stencil buffer.
		m_p3dDriver->SetDepthStencilState(pDSNoDepth);

		m_pSprite->BeginBatchRendering();
		for (DXUIElementVectorIterator iter=m_vecpUIElements.begin(); iter!=m_vecpUIElements.end(); iter++)
		{
			IDXUIElement* pElement = (*iter)->pElement;
			if (pElement->IsVisible())
			{
				DX_PIX_EVENT_BEGIN(0x00ff00ff, L"UI Element Render");
				pElement->Render();
				m_pSprite->Flush();
				DX_PIX_EVENT_END;

				if (pElement->m_bRenderText)
				{
					DX_PIX_EVENT_BEGIN(0x00ff00ff, L"UI Text Render");
					pText->BeginText(0, 0, true, true);
					pElement->RenderText();
					pText->EndText();
					DX_PIX_EVENT_END;
				}
			}
		}
		m_pSprite->EndBatchRendering();

		m_p3dDriver->SetDepthStencilState(pDSTemp);

		DX_PIX_EVENT_END;
	}


	bool CDXUIManager::ProcessMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		UNREFERENCED_PARAMETER(hWnd);

		bool bNoFurtherProcessing = false;

		// The current element under test.
		static IDXUIElement* pElement = nullptr;
		// The element that was under the mouse on the last mouse move message.
		static IDXUIElement* pLastElementUnderMouse = nullptr;
		// The element on which mouse button was pressed.
		static IDXUIElement* pPressedElement = nullptr;
		// The element that has mouse focus.
		static IDXUIElement* pMouseFocussedElement = nullptr;
		//static IDXUIElement* pKeyboardFocussedElement = nullptr;
		static IDXMouseHandler* pMouseHandler = nullptr;
		static IDXKeyboardHandler* pKeyboardHandler = nullptr;
		static DXPoint mousePoint;

		switch (uMsg)
		{
		case WM_MOUSEMOVE:
			mousePoint.x = (float)LOWORD(lParam);
			mousePoint.y = (float)HIWORD(lParam);

			// If an element has mouse focus. Allow it handle the message first.
			if (pMouseFocussedElement)
			{
				pMouseHandler = pMouseFocussedElement->GetMouseHandler();
				if (pMouseHandler)
				{
					if (pMouseHandler->OnMouseMove(mousePoint))
					{
						return true;
					}
				}
			}

			pElement = GetElementAtPoint(mousePoint);
			if (pElement) // Have an element under the mouse.
			{
				pMouseHandler = pElement->GetMouseHandler();
				if (pMouseHandler)
				{
					bNoFurtherProcessing = pMouseHandler->OnMouseMove(mousePoint);
					// If mouse was on some element in the last message. Tell the element
					// that mouse has left it. This is necssary for overlapping element in
					// which case the pElement never becomes NULL when mouse moves from one
					// element to another.
					if (pLastElementUnderMouse)
					{
						pMouseHandler = pLastElementUnderMouse->GetMouseHandler();
						if (pMouseHandler)
						{
							pMouseHandler->OnMouseMove(mousePoint);
						}
					}
				}
				// Save the record.
				pLastElementUnderMouse = pElement;
				return bNoFurtherProcessing;
			}
			// No element under mouse? Was there an element under cursor in the last message?
			else if (pLastElementUnderMouse)
			{
				// Yes? Then the cursor has left the element.
				pMouseHandler = pLastElementUnderMouse->GetMouseHandler();
				pLastElementUnderMouse = nullptr;
				if (pMouseHandler)
				{
					return pMouseHandler->OnMouseMove(mousePoint);
				}
			}
			break;


		case WM_LBUTTONDOWN:
			// Unfocus the elements with previous focus.
			if (m_pKeyboardFocussedElement)
			{
				m_pKeyboardFocussedElement->SetFocussed(false);
			}
			if (pMouseFocussedElement)
			{
				pMouseFocussedElement->SetFocussed(false);
			}

			mousePoint.x = (float)LOWORD(lParam);
			mousePoint.y = (float)HIWORD(lParam);
			// The element under the mouse cursor.
			pElement = GetElementAtPoint(mousePoint);
			pPressedElement = pElement;
			pMouseFocussedElement = nullptr;
			m_pKeyboardFocussedElement = nullptr;

			if (pElement)
			{
				// If the element can have mouse focus then this element receives the mouse
				// focus.
				pMouseFocussedElement = pElement->m_bMouseFocusable ? pElement : nullptr;
				pMouseHandler = pElement->GetMouseHandler();
				if (pMouseHandler)
				{
					// The element can set focus on itself if it wants to in this call.
					return pMouseHandler->OnLeftMouseDown(mousePoint);
				}
			}
			break;


		case WM_LBUTTONUP:
			mousePoint.x = (float)LOWORD(lParam);
			mousePoint.y = (float)HIWORD(lParam);
			// Button up. No more mouse focus.
			pMouseFocussedElement = nullptr;
			// If an element was pressed on the button down event?
			if (pPressedElement)
			{
				pMouseHandler = pPressedElement->GetMouseHandler();
				if (pMouseHandler)
				{
					bNoFurtherProcessing = pMouseHandler->OnLeftMouseUp(mousePoint);
					if (bNoFurtherProcessing)
					{
						return true;
					}
				}
				pPressedElement = nullptr;
				// Now the mouse button has been released, We look for an element under the cursor.
				// If found then mouse has just entered this element.
				pElement = GetElementAtPoint(mousePoint);
				if (pElement)
				{
					pMouseHandler = pElement->GetMouseHandler();
					if (pMouseHandler)
					{
						return pMouseHandler->OnMouseMove(mousePoint);
					}
				}
			}
			break;


		case WM_KEYDOWN:
			if (m_pKeyboardFocussedElement)
			{
				pKeyboardHandler = m_pKeyboardFocussedElement->GetKeyboardHandler();
				if (pKeyboardHandler)
				{
					return pKeyboardHandler->OnKeyDown((UINT)wParam);
				}
			}
			break;

		case WM_KEYUP:
			if (m_pKeyboardFocussedElement)
			{
				pKeyboardHandler = m_pKeyboardFocussedElement->GetKeyboardHandler();
				if (pKeyboardHandler)
				{
					return pKeyboardHandler->OnKeyUp((UINT)wParam);
				}
			}
			break;


		case WM_CHAR:
			if (m_pKeyboardFocussedElement)
			{
				pKeyboardHandler = m_pKeyboardFocussedElement->GetKeyboardHandler();
				if (pKeyboardHandler && pKeyboardHandler->m_bSendWCHARMessages)
				{
					return pKeyboardHandler->OnKey((WCHAR)wParam);
				}
			}
			break;

		default:
			break;
		}

		return bNoFurtherProcessing;
	}

} // DXFramework namespace end