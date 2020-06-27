#include "stdafx.h"
#include "UIProgress.h"

namespace DuiLib
{
	REGIST_DUICONTROL(CProgressUI)
	CProgressUI::CProgressUI() : CLabelUI(), m_bHorizontal(true), m_nMin(0), m_nMax(100), m_nValue(0)
	{
		m_uTextStyle = DT_SINGLELINE | DT_CENTER;
		SetFixedHeight(12);
	}

	LPCTSTR CProgressUI::GetClass() const
	{
		return DUI_CTR_PROGRESS;
	}

	LPVOID CProgressUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, DUI_CTR_PROGRESS) == 0 ) return static_cast<CProgressUI*>(this);
		return CLabelUI::GetInterface(pstrName);
	}

	bool CProgressUI::IsHorizontal()
	{
		return m_bHorizontal;
	}

	void CProgressUI::SetHorizontal(bool bHorizontal)
	{
		if( m_bHorizontal == bHorizontal ) return;

		m_bHorizontal = bHorizontal;
		Invalidate();
	}

	int CProgressUI::GetMinValue() const
	{
		return m_nMin;
	}

	void CProgressUI::SetMinValue(int nMin)
	{
		m_nMin = nMin;
		Invalidate();
	}

	int CProgressUI::GetMaxValue() const
	{
		return m_nMax;
	}

	void CProgressUI::SetMaxValue(int nMax)
	{
		m_nMax = nMax;
		Invalidate();
	}

	int CProgressUI::GetValue() const
	{
		return m_nValue;
	}

	void CProgressUI::SetValue(int nValue)
	{
		m_nValue = nValue;
		if (m_nValue > m_nMax) m_nValue = m_nMax;
		if (m_nValue < m_nMin) m_nValue = m_nMin;
		Invalidate();
	}

	LPCTSTR CProgressUI::GetForeImage() const
	{
		return m_diFore.sDrawString;
	}

	void CProgressUI::SetForeImage(LPCTSTR pStrImage)
	{
		if( m_diFore.sDrawString == pStrImage && m_diFore.pImageInfo != NULL ) return;
		m_diFore.Clear();
		m_diFore.sDrawString = pStrImage;
		Invalidate();
	}

	bool CProgressUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if (_tcscmp(pstrName, _T("foreimage")) == 0) { SetForeImage(pstrValue); return true; }
		else if (_tcscmp(pstrName, _T("hor")) == 0) { SetHorizontal(_tcscmp(pstrValue, _T("true")) == 0); return true; }
		else if (_tcscmp(pstrName, _T("min")) == 0) { SetMinValue(_ttoi(pstrValue)); return true; }
		else if (_tcscmp(pstrName, _T("max")) == 0) { SetMaxValue(_ttoi(pstrValue)); return true; }
		else if (_tcscmp(pstrName, _T("value")) == 0) { SetValue(_ttoi(pstrValue)); return true; }
		else return CLabelUI::SetAttribute(pstrName, pstrValue);
	}

	void CProgressUI::PaintStatusImage(HDC hDC)
	{
		if( m_nMax <= m_nMin ) m_nMax = m_nMin + 1;
		if( m_nValue > m_nMax ) m_nValue = m_nMax;
		if( m_nValue < m_nMin ) m_nValue = m_nMin;

		RECT rc = {0};
		if( m_bHorizontal ) {
			rc.right = (m_nValue - m_nMin) * (m_rcItem.right - m_rcItem.left) / (m_nMax - m_nMin);
			rc.bottom = m_rcItem.bottom - m_rcItem.top;
		}
		else {
			rc.top = (m_rcItem.bottom - m_rcItem.top) * (m_nMax - m_nValue) / (m_nMax - m_nMin);
			rc.right = m_rcItem.right - m_rcItem.left;
			rc.bottom = m_rcItem.bottom - m_rcItem.top;
		}
		m_diFore.rcDestOffset = rc;
		if( DrawImage(hDC, m_diFore) ) return;
	}

	void CProgressUI::GetPropertyList(std::vector<UIPropertyGrid>& property_list)
	{
		__super::GetPropertyList(property_list);

		property_list.push_back(UIPropertyGrid("Progress", "Progress"));
		UIPropertyGrid& property = property_list.back();
		std::vector< UIPropertyGridItem >& items = property.items;

		items.push_back(UIPropertyGridItem(PropertyType::PT_Image, "ForeImage", "指定进度条的前景图片"));
		items.push_back(UIPropertyGridItem(PropertyType::PT_Size, "MinMax", "MinMax"));
		{
			UIPropertyGridItem& item = items.back();
			item.childs.push_back(UIPropertyGridItem(PropertyType::PT_Number, "Min", "指定进度条的最小值", _variant_t(0)));
			item.childs.push_back(UIPropertyGridItem(PropertyType::PT_Number, "Max", "指定进度条的最大值", _variant_t(100)));
		}
		items.push_back(UIPropertyGridItem(PropertyType::PT_Number, "Value", "指定进度条的值", _variant_t(0)));
		items.push_back(UIPropertyGridItem(PropertyType::PT_Boolean, "Hor", "指示进度条是否水平", _variant_t(bool(false))));
	}
}
