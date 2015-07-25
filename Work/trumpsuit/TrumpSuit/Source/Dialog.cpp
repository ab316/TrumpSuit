#include "stdafx.h"
#include "Dialog.h"
#include "DXUIElement.h"

using namespace DXFramework;

IDialog::IDialog()
{
}


IDialog::~IDialog()
{
}


void IDialog::AddElement(IDXUIElement* pElement)
{
	m_vecpElements.push_back(pElement);
}


void IDialog::RemoveElement(IDXUIElement* pElement)
{
	auto iter = m_vecpElements.begin();
	auto end = m_vecpElements.end();

	while (iter != end)
	{
		if ((*iter) == pElement)
		{
			m_vecpElements.erase(iter);
		}

		++iter;
	}
}


void IDialog::Show(bool bShow)
{
	auto iter = m_vecpElements.begin();
	auto end = m_vecpElements.end();

	while (iter != end)
	{
		(*iter)->SetVisible(bShow);
		++iter;
	}
}