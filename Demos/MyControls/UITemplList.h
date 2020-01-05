/*
	模板列表
*/

#ifndef UITEMPLLIST_H
#define UITEMPLLIST_H
#pragma once

namespace DuiLib
{
	class MYCONTROLS_API CTemplListItemUI : public CListContainerElementUI
	{
		DECLARE_CLASS(CTemplListItemUI)
	public:
		CTemplListItemUI();
		LPCTSTR GetTemplateClassName();
		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		virtual void OnCreate();
	};

	class MYCONTROLS_API CTemplListUI : public CListUI
	{
		DECLARE_CLASS(CTemplListUI)
	public:
		CTemplListUI();
		virtual ~CTemplListUI();
		
		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		void SetTemplateXML(LPCTSTR pstrName);
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		void SetCount(UINT count);
		CTemplListItemUI* AddOne();
		template<class T>
		T* AddOneItem()
		{
			return static_cast<T*>(AddOne());
		}

		void SetCreateBuilderCallback(IDialogBuilderCallback* pCallbackfunc) { m_pCallback = pCallbackfunc; }
	private:
		CDuiString m_template_xml;
		IDialogBuilderCallback* m_pCallback;
	};
}

#endif//UITEMPLLIST_H