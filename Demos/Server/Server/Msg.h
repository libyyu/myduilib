#ifndef CMSG1_H
#define CMSG1_H

#pragma once
namespace MSGTYPE
{
	static const TCHAR* SEP = _T("_");

	static const TCHAR* NULLMSG = _T("NULLMSG");
	static const TCHAR* NOMALMSG = _T("NOMALMSG");//一般聊天消息
	static const TCHAR* SYSTEMMSG = _T("SYSTEMMSG");//系统消息
	static const TCHAR* LOADMSG = _T("LOADMSG");//请求登录消息
	static const TCHAR* ERRORMSG = _T("ERRORMSG");//错误消息

	static const TCHAR* SERVERDOWN = _T("SERVERDOWN");//服务端关闭
	static const TCHAR* FAIL = _T("FAIL");//失败
	static const TCHAR* SUCCESS = _T("SUCCESS");//成功
	static const TCHAR* NOID = _T("NOID");//未注册
	static const TCHAR* REGI = _T("REGI");//注册
	static const TCHAR* OFLINE = _T("OFFLINE");//未登录
	static const TCHAR* LOAD = _T("LOAD");//登录
	static const TCHAR* CLOSE = _T("CLOSE");//客户端退出
	static const TCHAR* CHANGE = _T("CHANGE");//客户端改名
	static const TCHAR* USERLIST = _T("USERLIST");//用户列表
	static const TCHAR* CHAT = _T("CHAT");//聊天信息
};
#define  ONLINE   _T("ONLINE")
#define  OFFLINE  _T("OFFLINE")
struct User
{
	CDuiString m_loadName;
	CDuiString m_password;
	CDuiString m_callName;
	CDuiString m_userState;
	CDuiString m_userIp;
	CDuiString m_userPort;
	User()
	{
		m_loadName = m_callName = m_password = m_userIp = m_userPort = _T("");
		m_userState = OFFLINE;
	}
};
class CMsg : public  CObject
{
    public:
		CDuiString m_strText;
	    CDuiString m_msgType;
		
		struct NomalMsg
		{
			CDuiString who;
			CDuiString toWho;
			CDuiString msgItem;
			CDuiString sndTime;
		} m_nomalMsg;
		struct SystemMsg
		{
			CDuiString type;
			CDuiString name;
			CDuiString msgItem;
			CDuiString sndTime;
		} m_systemMsg;

		struct LOADMSG
		{
			CDuiString name;
			CDuiString password;
		} m_loadMsg;
		
	public:
		CMsg(CDuiString& preMsg);
		CMsg();
	    void Init();
 	    CDuiString WhatType();
// 	//	void SetMsg();
// //	private:
 		void BuildNomal();
 		void BuildLoad();
 		void BuileSystem();

};
#endif