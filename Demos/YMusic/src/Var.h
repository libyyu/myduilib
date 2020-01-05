#ifndef __Var_h
#define __Var_h

#pragma once

//����������
namespace varMainWindow
{

	//չ�����ֿ�
	extern const LPCTSTR kExpandMusicLib;
	//�������ֿ�
	extern const LPCTSTR kHideMusicLib;
	//����Ƥ��
	extern const LPCTSTR kChangeSkin;
	//���˵�
	extern const LPCTSTR kMainMenu;
	//����ģʽ
	extern const LPCTSTR kMiniMode;
	//��С��
	extern const LPCTSTR kShowMin;
	//��ԭ
	extern const LPCTSTR kRestoreSize;
	//���
	extern const LPCTSTR kShowMax;
	//�˳�
	extern const LPCTSTR kQuitApp;
	extern const LPCTSTR kLogin;
	extern const LPCTSTR kUserName;
	extern const LPCTSTR kUser_LogSwitch;
	//�����б�
	extern const LPCTSTR kPlayList;
	//�����б�
	extern const LPCTSTR kSongList;
	extern const LPCTSTR kAlbumLogo;
	//Tab����
	extern const LPCTSTR kTabLayout;
	extern const LPCTSTR kPrev;
	extern const LPCTSTR kPlay;
	extern const LPCTSTR kPause;
	extern const LPCTSTR kNext;
	extern const LPCTSTR kVolume;
	extern const LPCTSTR kVolumetab;
	extern const LPCTSTR kPlayModetab;
	extern const LPCTSTR kLrc;
	extern const LPCTSTR kVolumeslider;
	extern const LPCTSTR kAddPlayList;
	extern const LPCTSTR kTimeUsed;
	extern const LPCTSTR kTimeTotal;
	extern const LPCTSTR kPlayProgress;
	extern const LPCTSTR kSongName;
	extern const LPCTSTR kAuthorName;
	extern const LPCTSTR kCollect;
	extern const LPCTSTR kShare;
	extern const LPCTSTR kCount;
	extern const LPCTSTR kInfoLayout;
	extern const LPCTSTR kInfoLabel;
	extern const LPCTSTR kModeLayout;
	extern const LPCTSTR kListOrder;
	extern const LPCTSTR kListLoop;
	extern const LPCTSTR kSingle;
	extern const LPCTSTR kSingleLoop;
	extern const LPCTSTR kRandom;
	extern const LPCTSTR kSaveAsList;
	extern const LPCTSTR kGoBack;
	extern const LPCTSTR kGoNext;
	extern const LPCTSTR kGoDesk;
	extern const LPCTSTR kSearchBtn;
	extern const LPCTSTR kLrcViewTab;
	extern const LPCTSTR kSimilarSongs;
	extern const LPCTSTR kLrcView1;

};

//�����¼����
namespace varLoginWindow
{
	extern const LPCTSTR kCloseBtn;
	extern const LPCTSTR kUsername;
	extern const LPCTSTR kRegister;
	extern const LPCTSTR kPassword;
	extern const LPCTSTR kGetpassword;
	extern const LPCTSTR kInforesulttab;
	extern const LPCTSTR kRempassword;
	extern const LPCTSTR kAutologin;
	extern const LPCTSTR kResultLabel;
	extern const LPCTSTR kLoginbtn;
	extern const LPCTSTR kLoginlab;
};
namespace play_list_item
{
	extern const LPCTSTR kListItem;
	extern const LPCTSTR kListLabel;
	extern const LPCTSTR kListEdit;
};
namespace list_song_item
{
	extern const LPCTSTR kListItem;
	extern const LPCTSTR kSongNorItem;
	extern const LPCTSTR kSongExpItem;
	extern const LPCTSTR kNorMenu;
};
namespace varPlayListMenu
{
	extern const LPCTSTR kPlay;
	extern const LPCTSTR kRename;
	extern const LPCTSTR kRemove;
	extern const LPCTSTR kNewList;
};
namespace varSongListMenu
{
	extern const LPCTSTR kAddTo;
	extern const LPCTSTR kMoveTo;
	extern const LPCTSTR kPlay;
	extern const LPCTSTR kShare;
	extern const LPCTSTR kCollect;
	extern const LPCTSTR kCollectCheckBox;
	extern const LPCTSTR kModify;
	extern const LPCTSTR kDumplicate; //ȥ��
	extern const LPCTSTR kRemove;
	extern const LPCTSTR kClear;
	extern const LPCTSTR kProperty;
	extern const LPCTSTR kOpenDir;
	extern const LPCTSTR kAddFile;
	extern const LPCTSTR kAddFolder;
	extern const LPCTSTR kToNewList;
	extern const LPCTSTR kAdd;
};
namespace varSongItemMenu
{
	extern const LPCTSTR kPlay;
	extern const LPCTSTR kCollect;
	extern const LPCTSTR kShare;
	extern const LPCTSTR kRemove;
	extern const LPCTSTR kDownLoad;
};
namespace varPlayModeMenu
{
	extern const LPCTSTR kListOrder;
	extern const LPCTSTR kListLoop;
	extern const LPCTSTR kSingleLoop;
	extern const LPCTSTR kSingle;
	extern const LPCTSTR kRandom;
	extern const LPCTSTR kCheckBox;
	extern const LPCTSTR kUserData;
};
namespace varLrcViewMenu
{
	extern const LPCTSTR kSearchLrc;
	extern const LPCTSTR kLrcModify;
	extern const LPCTSTR kLrcAdvance;
	extern const LPCTSTR kLrcBack;
	extern const LPCTSTR kLrcReset;
	extern const LPCTSTR kLrcLocal;
	extern const LPCTSTR kLrcEdit;
	extern const LPCTSTR kLrcSetting;
	extern const LPCTSTR kLrcFolder;
	extern const LPCTSTR kLrcDesk;
};
namespace varMainMenu
{
	extern const LPCTSTR kExit;
	extern const LPCTSTR kLogin;
	extern const LPCTSTR kLogout;
	extern const LPCTSTR kSetting;
	extern const LPCTSTR kKeepTop;
	extern const LPCTSTR kDeskLrc;
	extern const LPCTSTR kLockLrc;
	extern const LPCTSTR kSkinWin;
};
extern const LPCTSTR UnKnown;
extern const UINT    nMaxSongNumber;
#endif//__Var_h