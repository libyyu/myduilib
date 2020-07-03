
/***********************************************************
 *	FileName: LrcView.h
 *  Author  : DFn Li
 *  Timer   : 2013/3/20 18:10
 *  E-Mail  : libyyu@qq.com
 *  Comment : show lrc to program panel
 *  Version : 1.0.0.1
 ***********************************************************/
#ifndef __UILRCVIEW_H__
#define __UILRCVIEW_H__
#pragma once
#include <functional>
#pragma warning(disable: 4251)
namespace DuiLib {
	class MYCONTROLS_API CLrcViewUI : public CContainerUI
	{
		DECLARE_CLASS(CLrcViewUI)
	public:
		struct MYCONTROLS_API LrcNode
		{
			int StartShowTime;//单位毫秒，与开始播放时的间隔值
			CDuiString sLyric;//歌词内容
			int length;
			int OldPos;//上次显示的位置
			LrcNode(int nStartShowTime, const CDuiString& s )
			{
				StartShowTime = nStartShowTime;
				sLyric = s;
				length = 0;
				OldPos = 0;
			}
			LrcNode(LrcNode& node)
			{
				StartShowTime = node.StartShowTime;
				sLyric = node.StartShowTime;
				length = node.StartShowTime;
				OldPos = node.OldPos;
			}
			LrcNode operator = (LrcNode& node)
			{
				StartShowTime = node.StartShowTime;
				sLyric = node.StartShowTime;
				length = node.StartShowTime;
				OldPos = node.OldPos;
				return (*this);
			}
			bool operator <(LrcNode &node)
			{
				return StartShowTime < node.StartShowTime;
			}
		};

		enum emStatus
		{
			none,
			stop,
			play,
			pause,
		};
		enum emEvent
		{
			forward,
			back,
			reset,
			gettime,
		};
		typedef std::function<void (CLrcViewUI*, emEvent, __int64*)> HanderT;

		CLrcViewUI();
		virtual ~CLrcViewUI();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		void SetIdleText(LPCTSTR s);
		void SetSearchButtonText(LPCTSTR s);
		void SetSearchButtonTooltip(LPCTSTR s);
		void SetSearchButtonTextColor(DWORD c);

		void SetLyrics(const std::vector<LrcNode>& v);
		void SetStatus(emStatus status);

		void SetLineHeight(size_t n) { m_nLineHeight = n; }

		void SetTimeOffset(float f) { m_fTimeOffset = f; }
		void ResetTimeOffset() { SetTimeOffset(0.0); }

		void SetTimeDelta(float f) { m_fTimeDelta = f; }

		void SetPlayedColor(COLORREF c) { m_cPlayedColor = c; }
		void SetPlayingColor(COLORREF c) { m_cPlayingColor = c; }
		void SetUnPlayColor(COLORREF c) { m_cUnPlayColor = c; }
		void SetLOGFONT(LOGFONT& f) { m_lrcFont = f; }

		void SetEventHandler(HanderT h) { m_handler = h; }
	public:
		virtual void SetPos(RECT rc, bool bNeedInvalidate = true);
		virtual void Move(SIZE szOffset, bool bNeedInvalidate = true);
		virtual void SetVisible(bool bVisible = true);
		virtual void SetInternVisible(bool bVisible = true);
		virtual void SetMouseEnabled(bool bEnable = true);
		void SetManager(CPaintManagerUI* pManager, CControlUI* pParent, bool bInit = true);
		CControlUI* FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags);
	protected:
		void DoInit();
		void DoEvent(TEventUI& event);
		bool DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);
		void PaintText(HDC hDC);
		void PaintTextRaw(HDC hDC, __int64 iTimeElapse);
		void EnableSearchButton(bool enable);
		virtual bool SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		void SetSearchButtonAttributeList(LPCTSTR pstrValue);
	private:
		HanderT           m_handler;
		CDuiString        m_sIdleText;
		CDuiString        m_sSearchButtonText;
		CDuiString        m_sSearchButtonTooltip;
		size_t            m_nLineHeight;
		float             m_fTimeDelta; //每次改变的数量，默认0.5：毫秒
		float             m_fTimeOffset;//单位：毫秒

		COLORREF          m_cPlayedColor;
		COLORREF          m_cPlayingColor;
		COLORREF          m_cUnPlayColor;
		LOGFONT           m_lrcFont;

		CButtonUI*        m_pSearchLrcBtn;
		emStatus          m_eStatus;
		std::vector<LrcNode> m_Lyrics;
	};
}
#pragma warning(default: 4251)
#endif//__UILRCVIEW_H__