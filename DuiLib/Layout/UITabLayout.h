#ifndef __UITABLAYOUT_H__
#define __UITABLAYOUT_H__

#pragma once

namespace DuiLib
{
	class UILIB_API CTabLayoutUI : public CContainerUI, public CUIAnimation
	{
		DECLARE_CLASS(CTabLayoutUI)
	public:
		CTabLayoutUI();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		bool Add(CControlUI* pControl);
		bool AddAt(CControlUI* pControl, int iIndex);
		bool Remove(CControlUI* pControl, bool bDoNotDestroy=false);
		void RemoveAll();
		int GetCurSel() const;
		bool SelectItem(int iIndex, bool bTriggerEvent=true);
		bool SelectItem(CControlUI* pControl,  bool bTriggerEvent=true);

		void SetPos(RECT rc, bool bNeedInvalidate = true);

	protected:
		virtual bool SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
	public:
		void AnimationSwitch();
		void DoEvent(TEventUI& event);
		void OnTimer(int nTimerID);
		
		enum AnimationDirection
		{
			None,
			Vertical,
			Horizontal,
		};
		AnimationDirection GetAnimationDirection() const { return m_nAnimationDirection; }

	public://For Designer
		virtual void GetPropertyList(std::vector<UIPropertyGrid>& property_list);

	protected:
		virtual void OnAnimationStart(INT nAnimationID, BOOL bFirstLoop) {}
		virtual void OnAnimationStep(INT nTotalFrame, INT nCurFrame, INT nAnimationID);
		virtual void OnAnimationStop(INT nAnimationID);
	protected:
		enum
		{
			TAB_ANIMATION_ID = 1,

			TAB_ANIMATION_ELLAPSE = 10,
			TAB_ANIMATION_FRAME_COUNT = 15,
		};
		int m_iCurSel;

		AnimationDirection m_nAnimationDirection;
		int m_nPositiveDirection;
		RECT m_rcCurPos;
		RECT m_rcItemOld;
		CControlUI* m_pCurrentControl;
		bool m_bControlVisibleFlag;

		

		
	};
}
#endif // __UITABLAYOUT_H__
