#include "DXStdafx.h"
#include "DXSprite.h"
#include "DXMouseHandler.h"
#include "DXUICheckBox.h"


namespace DXFramework
{ // DXFramework namespace begin

	CDXUICheckBox::CDXUICheckBox()	:	m_bChecked(false),
										m_bShowCheck(false),
										m_f4CheckColor(0, 0, 0, 0), // Alpha zero initially coz unchecked.
										m_f4NormalTint(1.0f, 1.0f, 1.0f, 1.0f),
										m_f4MouseOverTint(1.5f, 1.5f, 1.5f, 1.0f),
										m_f4DisabledTint(0.5f, 0.5f, 0.5f, 1.0f),
										m_rectTextureCheck(0.104f, 0.205f, 0.210f, 0.318f)
	{
		m_bRenderText = false;
		m_rectTexture = DXRect(0.006f, 0.217f, 0.1f, 0.311f);
		m_f4Color = m_f4NormalTint;
		m_pMouseHandler = new CDXUICheckBoxMouseHandler(this);
	}


	CDXUICheckBox::~CDXUICheckBox()
	{
		DX_SAFE_DELETE(m_pMouseHandler);
	}


	HRESULT CDXUICheckBox::Create(float fX, float fY, float fSide, bool bChecked)
	{
		HRESULT hr = S_OK;

		m_rect = DXRect(fX, fY, fX+fSide, fY+fSide);
		m_bChecked = bChecked;
		m_bShowCheck = bChecked;
		m_f4CheckColor.w = m_bChecked ? 1.0f : 0;

		return hr;
	}


	void CDXUICheckBox::Render()
	{
		m_pSprite->BatchRenderSprite(&m_rect, &m_rectTexture, &m_f4Color, 0);
		if (m_bShowCheck)
		{
			m_pSprite->BatchRenderSprite(&m_rect, &m_rectTextureCheck, &m_f4CheckColor, 0);
		}
	}


	void CDXUICheckBox::Update(float fDelta)
	{
		// It's too slow otherwise.
		float fRate = 10.0f * fDelta;

		XMVECTOR vecRequiredColor;
		switch (m_State)
		{
		case DX_UIELEMENT_STATE_MOUSEOVER:
			vecRequiredColor = XMLoadFloat4(&m_f4MouseOverTint);
			break;

		case DX_UIELEMENT_STATE_DISABLED:
			vecRequiredColor = XMLoadFloat4(&m_f4DisabledTint);
			break;

		default:
			vecRequiredColor = XMLoadFloat4(&m_f4NormalTint);
			break;
		}

		// All this because we want to fade in and fade out the check instead of just making
		// it come and disappear.
		float fCheckAlpha = m_bChecked ? 1.0f : 0;
		if (!XMScalarNearEqual(m_f4CheckColor.w, fCheckAlpha, 0.01f))
		{
			m_bShowCheck = true;
			// Linear interpolation of the check alpha.
			m_f4CheckColor.w = m_f4CheckColor.w + fRate * (fCheckAlpha - m_f4CheckColor.w);
		}
		else
		{
			m_bShowCheck = m_bChecked;
		}

		// Linear interpolation of the box color.
		XMStoreFloat4(&m_f4Color, XMVectorLerp(XMLoadFloat4(&m_f4Color), vecRequiredColor, fRate));
	}

} // DXFramework namespace end