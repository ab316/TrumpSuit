#include "DXStdafx.h"
#include "DXSprite.h"
#include "DXMouseHandler.h"
#include "DXUIRadioButton.h"


namespace DXFramework
{ // DXFramework namespace begin

	CDXUIRadioButton::CDXUIRadioButton()	:	m_uiRadioId(0),
												m_bChecked(false),
												m_bShowCheck(false),
												m_pRadioButtonGroup(nullptr),
												m_f4CheckColor(0, 0, 0, 0), // Alpha zero initially.
												m_f4NormalTint(1.0f, 1.0f, 1.0f, 1.0f),
												m_f4MouseOverTint(1.5f, 1.5f, 1.5f, 1.0f),
												m_f4DisabledTint(0.5f, 0.5f, 0.5f, 1.0f),
												m_rectTextureCheck(0.3185f, 0.213f, 0.4195f, 0.314f)
	{
		m_bRenderText = false;
		m_rectTexture = DXRect(0.209f, 0.212f, 0.318f, 0.315f);
		m_f4Color = m_f4NormalTint;

		m_pMouseHandler = new CDXUIRadioButtonMouseHandler(this);
	}


	CDXUIRadioButton::~CDXUIRadioButton()
	{
		DX_SAFE_DELETE(m_pMouseHandler);
		m_pRadioButtonGroup->m_mapRadioButtons.erase(m_uiRadioId);
	}


	HRESULT CDXUIRadioButton::Create(DXRadioButtonGroup* pGroup, float fCentreX, float fCentreY, float fRadius)
	{
		HRESULT hr = S_OK;

		if (!pGroup)
		{
			return E_INVALIDARG;
		}

		m_rect = DXRect(fCentreX-fRadius, fCentreY-fRadius, fCentreX+fRadius, fCentreY+fRadius);
		m_pRadioButtonGroup = pGroup;

		if (m_pRadioButtonGroup->m_mapRadioButtons.size())
		{
			DXRadioButtonMapIterator iterLast = m_pRadioButtonGroup->m_mapRadioButtons.end();
			--iterLast;
			m_uiRadioId = iterLast->first + 1;
		}
		else
		{
			m_uiRadioId = 0;
		}
		m_pRadioButtonGroup->m_mapRadioButtons.insert(DXRadioButtonMap::value_type(m_uiRadioId, this));

		return hr;
	}


	void CDXUIRadioButton::Render()
	{
		m_pSprite->BatchRenderSprite(&m_rect, &m_rectTexture, &m_f4Color, 0);
		if (m_bShowCheck)
		{
			m_pSprite->BatchRenderSprite(&m_rect, &m_rectTextureCheck, &m_f4CheckColor, 0);
		}
	}


	void CDXUIRadioButton::Update(float fDelta)
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