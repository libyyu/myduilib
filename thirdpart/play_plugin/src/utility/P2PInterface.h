//
//  P2PInterface.h
//  P2PLib
//
//  Created by teym on 12-8-30.
//  Copyright (c) 2012年 teym. All rights reserved.
//
#pragma once
#ifndef __P2PLib__P2PInterface__
#define __P2PLib__P2PInterface__


#ifdef _WIN32
#define XIAMI_EXPORT
#endif

#ifdef XIAMI_EXPORT
#ifdef P2P_EXPORT
#define  XIMAPI   __declspec(dllexport)
#else
#define   XIMAPI __declspec(dllimport)
#ifdef _DEBUG
#pragma comment(lib, "Debug\\p2plib.lib")
#else
#pragma comment(lib, "Release\\p2plib.lib")
#endif
#endif
#else
#define  XIMAPI   __attribute__ ((visibility("default")))
#define __stdcall
#endif

#include <cstddef>


#if defined(__cplusplus)
extern "C" {
#endif

	namespace P2PLibInterface
	{
		typedef struct _NetState //全局网络状态
		{
			size_t  sendSpeed; //发送速度 B/s
			size_t	recvSpeed; //接收速度 B/s
			size_t	connCount; //所有连接数
			_NetState() : sendSpeed(0), recvSpeed(0), connCount(0) {};
		} NetState;

		typedef void(* NetStatusCall)(NetState* pNetStatus);
		XIMAPI void GetNetState(NetStatusCall fun);

		enum _DownState
		{
			Unkown = 0,
			Downing,
			SchedulePause,
			Checking,
			Finish,
			Failed,
			Redown,
		};
		typedef struct _SCallback //资源下载信息
		{
			size_t resSize; //资源大小
			size_t state; //资源下载状态 	Unkown,Downing,SchedulePause, Checking, Finish, Failed, Redown
			size_t downloadedSize; //资源已下载大小
			size_t speed; //下载速度 B/s
			size_t seeds;//总共种子数
			size_t connCount;//已连接的种子数
			_SCallback() : resSize(0), state(0), downloadedSize(0), speed(0), seeds(0), connCount(0) {};
		} CallBackData;

		//XIMAPI void  GetDownResStatus(const char* pHashSequence, StatusCallBack fun);    //批量获取资源下载状态，pHashSequence分隔符是逗号
		XIMAPI void  StopResDown(const char* pHashSequence);    //批量暂停正下载资源，pHashSequence分隔符是逗号
		XIMAPI void  RemoveRes(const char* pHashSequence);    //批量删除已下载的资源，pHashSequence分隔符是逗号
		XIMAPI void  SetSpeedLimit(size_t downSpeed, size_t uploadSpeed);    //限制下载和上传速度，单位是B/s

		enum DownLevel
		{
			DownloadNormal,//normal download
			DownloadPost,
			DownloadHigh,//pause others
			DownloadHighest//pause others and first request from fc
		};
		struct DownUserInfo{
			char* _hash;
			size_t _reqBegin;
			size_t _reqLength;
			DownLevel _sourceLevel;
			size_t _timeout;
			int _redown;
			typedef void (* DownCallBack)(DownUserInfo* info, size_t begin, size_t length, void* buffer);
			DownCallBack  _dataCallback;
			typedef void (* ResInfoCall)(DownUserInfo* info,size_t resLength);
			ResInfoCall  _infoCallback;
			typedef void (* StatusCallBack)(DownUserInfo* info, CallBackData* pStatus);
			StatusCallBack _statusCallback;
			typedef void (* EndCallBack)(DownUserInfo* info, CallBackData* pStatus);
			EndCallBack	_endCallBack;
			DownUserInfo():_dataCallback(NULL),_infoCallback(NULL),_statusCallback(NULL),_endCallBack(NULL),_timeout(3 * 1000), _redown(-1){};
		};

		XIMAPI void StartDownload(DownUserInfo* info);
		XIMAPI void CancelDownload(DownUserInfo* info);
        
        //add by teym
        struct PublishInfo
        {
            char * _hash;
            char * _path;
            unsigned int _albumID;
            unsigned int _songID;
            unsigned int _artistID;
            unsigned int _userID;
            char *       _userName;
            char *       _userPwd;
            unsigned int _publishID;
            typedef void (* FinalCallBack)(PublishInfo*,int state,const char* errorMsg);
			FinalCallBack  _FinalCallback;
        };
        XIMAPI void publishResource(PublishInfo* resList,unsigned int count,const char* fcHost,unsigned short fcPort);
	}
#if defined(__cplusplus)
}
#endif
#endif
// defined(__P2PLib__P2PInterface__) 
