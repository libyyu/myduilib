#ifndef CMSG1_H
#define CMSG1_H

#pragma once
namespace MSGTYPE
{
	static const TCHAR* SEP = _T("_");

	static const TCHAR* NULLMSG = _T("NULLMSG");
	static const TCHAR* NOMALMSG = _T("NOMALMSG");//һ��������Ϣ
	static const TCHAR* SYSTEMMSG = _T("SYSTEMMSG");//ϵͳ��Ϣ
	static const TCHAR* LOADMSG = _T("LOADMSG");//�����¼��Ϣ
	static const TCHAR* ERRORMSG = _T("ERRORMSG");//������Ϣ

	static const TCHAR* SERVERDOWN = _T("SERVERDOWN");//����˹ر�
	static const TCHAR* FAIL = _T("FAIL");//ʧ��
	static const TCHAR* SUCCESS = _T("SUCCESS");//�ɹ�
	static const TCHAR* NOID = _T("NOID");//δע��
	static const TCHAR* REGI = _T("REGI");//ע��
	static const TCHAR* OFLINE = _T("OFFLINE");//δ��¼
	static const TCHAR* LOAD = _T("LOAD");//��¼
	static const TCHAR* CLOSE = _T("CLOSE");//�ͻ����˳�
	static const TCHAR* CHANGE = _T("CHANGE");//�ͻ��˸���
	static const TCHAR* USERLIST = _T("USERLIST");//�û��б�
	static const TCHAR* CHAT = _T("CHAT");//������Ϣ
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