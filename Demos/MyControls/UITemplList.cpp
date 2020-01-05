#include "MyControls.h"

namespace DuiLib
{
	CTemplListItemUI::CTemplListItemUI() :CListContainerElementUI()
	{
	}

	LPCTSTR CTemplListItemUI::GetClass() const
	{
		return DUI_CTR_TEMPLLIST_ITEM;
	}

	LPCTSTR CTemplListItemUI::GetTemplateClassName()
	{
		return DUI_CTR_TEMPLLIST_ITEM;
	}

	LPVOID CTemplListItemUI::GetInterface(LPCTSTR pstrName)
	{
		if (_tcscmp(pstrName, DUI_CTR_TEMPLLIST_ITEM) == 0)
			return static_cast<CTemplListItemUI*>(this);
		return CListContainerElementUI::GetInterface(pstrName);
	}

	void CTemplListItemUI::OnCreate()
	{

	}


	CTemplListUI::CTemplListUI():CListUI(), m_pCallback(NULL)
	{

	}

	CTemplListUI::~CTemplListUI()
	{

	}

	LPCTSTR CTemplListUI::GetClass() const
	{
		return DUI_CTR_TEMPLLIST;
	}

	LPVOID CTemplListUI::GetInterface(LPCTSTR pstrName)
	{
		if (_tcscmp(pstrName, DUI_CTR_TEMPLLIST) == 0)
			return static_cast<CTemplListUI*>(this);
		return CListUI::GetInterface(pstrName);
	}

	void CTemplListUI::SetTemplateXML(LPCTSTR pstrName)
	{
		m_template_xml = pstrName;
	}

	void CTemplListUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if (_tcscmp(pstrName, _T("template_xml")) == 0) SetTemplateXML(pstrValue);
		else if (_tcscmp(pstrName, _T("count")) == 0) SetCount(_ttoi(pstrValue));
		else CListUI::SetAttribute(pstrName, pstrValue);
	}

	void CTemplListUI::SetCount(UINT count)
	{
		int old_count = this->GetCount();
		if (count > old_count)
		{
			for (int i = old_count; i < count; ++i)
			{
				ASSERT(m_template_xml.GetLength() > 0);
				if(m_template_xml.IsEmpty()) continue;
				CDialogBuilder builder;
				CControlUI* pListItem = builder.Create(m_template_xml.GetData(), 0, m_pCallback, m_pManager, NULL);
				BOOL valid = !!pListItem;// && _tcscmp(pListItem->GetClass(), DUI_CTR_TEMPLLIST_ITEM) == 0 ? TRUE : FALSE;
				ASSERT(valid);
				static_cast<CTemplListItemUI*>(pListItem)->OnCreate();
				if(valid) this->Add(pListItem);
			}
		}
		else if(count < old_count)
		{
			for (int i = old_count; i > count; --i)
			{
				this->RemoveAt(i);
			}
		}
	}

	CTemplListItemUI* CTemplListUI::AddOne()
	{
		ASSERT(m_template_xml.GetLength() > 0);
		if (m_template_xml.IsEmpty()) return NULL;
		CDialogBuilder builder;
		CControlUI* pListItem = builder.Create(m_template_xml.GetData(), 0, m_pCallback, m_pManager, NULL);
		BOOL valid = !!pListItem;//&& _tcscmp(pListItem->GetClass(), DUI_CTR_TEMPLLIST_ITEM) == 0 ? TRUE : FALSE;
		ASSERT(valid);
		static_cast<CTemplListItemUI*>(pListItem)->OnCreate();
		if (valid) this->Add(pListItem);
		return static_cast<CTemplListItemUI*>(pListItem);
	}
}