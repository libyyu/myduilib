// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Build:   Oct/14/2013
// Author:  FnLove2010
// Email:   libyyu@qq.com
// Blog:    ......
//
// 温馨提示：
// 此邮箱每天都收到很多垃圾邮件，邮件名请不要随意乱取，应尽量与垃圾邮件区分开来，幸好QQ的过滤功能还算强大~O(∩_∩)O~
// 此邮箱仅供【错误指正和技术交流】，若是技术问题请直接在文章评论区提问，否则恕不回复，这样做是为了让后面的小伙伴也能看到解决方案~O(∩_∩)O~
//
// 使用协议：WTFPL
// 鉴于在天朝神马协议都被无视，甚至很多大公司都不遵守，故索性采用了【DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE】协议~O(∩_∩)O~
//
// 代码安全性：
// 此项目为仅作为毕设使用，为了方便大家编译，没用三方库，因此写了一些简单的函数，很多逻辑判断也只是保证正常操作，实际使用请自行保证代码安全~O(∩_∩)O~
// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------


#ifndef __AVPlayer_H__
#define __AVPlayer_H__

#pragma once

#ifdef _MSC_VER
#ifdef BUILD_AS_DLL
#ifdef PLAYCENTER_EXPORTS
#define PLAYCENTER_API __declspec(dllexport)
#else
#define PLAYCENTER_API __declspec(dllimport)
#pragma comment(lib,"playcenter.lib")
#endif
#else
#define PLAYCENTER_API
#endif
#else
#define PLAYCENTER_API
typedef long long               __int64;
#endif

#include <iostream>

typedef void* H_HWND;
typedef void (* pfnCallback) (void *data);  // VLC事件的回调函数指针

class CAVPlayer;
class IAVPlayerCallBack
{
public:
	virtual void OnPlaying(CAVPlayer* player, void* event) = 0;         // 设置文件头读取完毕时的回调函数
	virtual void OnPosChanged(CAVPlayer* player, void* event) = 0;      // 设置文件位置改变时的回调函数
	virtual void OnEndReached(CAVPlayer* player, void* event) = 0;      // 设置文件头读取完毕时的回调函数
};

class PLAYCENTER_API CAVPlayer
{
    // VLC的事件管理
	friend void On_VLC_Event_Callback(int eventtype, void* event, void* data);

public:
    CAVPlayer(void);
    ~CAVPlayer(void);

	void SetCallback(IAVPlayerCallBack* pCallback) { m_pCallback = pCallback; }

    bool PlayFile(const std::string &strPath);  // 播放路径为strPath的文件
	bool PlayURL(const std::string& strURL);  // 
    int  Play ();                           // 播放
    void Pause();                           // 暂停
    void Stop ();                           // 停止

    void Volume(int iVol);                  // 音量设置为iVol
    void VolumeIncrease();                  // 音量增大
    void VolumeReduce();                    // 音量减小 

    void SeekTo(int iPos);                  // 跳到指定位置iPos
    void SeekForward(int seconds = 5);      // 快进
    void SeekBackward(int seconds = 5);     // 快退

    void SetHWND(H_HWND hwnd);                // 设置视频显示的窗口句柄
	H_HWND GetHWND();                         // 获取视频显示的窗口句柄

	int  GetStatus();

	bool    IsURL(const std::string &strPath);// 是网络地方还是本机地址
	bool    IsValid();						// 是否实例有效
    bool    IsOpen();                       // 文件是否打开
    bool    IsPlaying();                    // 文件是否正在播放
    int     GetPos();                       // 获取文件当前播放的位置
    __int64 GetTotalTime();                 // 获取总时间
    __int64 GetTime();                      // 获取时间
    int     GetVolume();                    // 获取音量

protected:
	void OnVLC_Event(int event_type, void *event);
private:
	IAVPlayerCallBack		*m_pCallback;
    void				    *m_pVLC_Inst;   // VLC实例
    void					*m_pVLC_Player; // VLC播放器
	H_HWND                  m_hWnd;         // 视频显示的窗口句柄
	int                     m_bStatus;      // 播放状态
	void				    *m_hPlugin;
private:
	
	void Init();                            // 初始化
	void Release();                         // 清理内存
};

#endif
