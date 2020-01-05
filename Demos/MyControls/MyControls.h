#ifndef MYCONTROL_H
#define MYCONTROL_H
#pragma once

#ifndef MYCONTROLS_EXPORTS_STATIC
#	if defined(MYCONTROLS_EXPORTS)
#		if	defined(_MSC_VER)
#			define MYCONTROLS_API __declspec(dllexport)
#		else
#			define MYCONTROLS_API 
#		endif
#	else
#		if defined(_MSC_VER)
#			define MYCONTROLS_API __declspec(dllimport)
#		else
#			define MYCONTROLS_API 
#		endif
#	endif
#else
#define MYCONTROLS_API 
#endif

#pragma warning (disable : 4511) // copy operator could not be generated
#pragma warning (disable : 4512) // assignment operator could not be generated
#pragma warning (disable : 4702) // unreachable code (bugs in Microsoft's STL)
#pragma warning (disable : 4786) // identifier was truncated
#pragma warning (disable : 4996) // function or variable may be unsafe (deprecated)
#pragma warning (disable : 4302)
#pragma warning (disable : 4251)
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS // eliminate deprecation warnings for VS2005
#endif

#ifndef _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_DEPRECATE
#endif




#include "UIlib.h"
#include "UIFadeAnimation.h"
#include "UIFadeButton.h"
#include "UIFadeOption.h"
#include "UIFadeCheckBox.h"
#include "UIAlbumButton.h"
#include "UILrcView.h"
#include "UITemplList.h"
#include "UIWkeWebkit.h"

#define  DUI_CTR_FADEBUTTON                        (_T("FadeButton"))
#define  DUI_CTR_FADEOPTION                        (_T("FadeOption"))
#define  DUI_CTR_FADECHECKBOX                      (_T("FadeCheckBox"))
#define  DUI_CTR_ALBUMBUTTON                       (_T("AlbumButton"))
#define  DUI_CTR_LRCVIEW                           (_T("LrcView"))
#define  DUI_CTR_TEMPLLIST                         (_T("TemplList"))
#define  DUI_CTR_TEMPLLIST_ITEM                    (_T("TemplListItem"))
//#define  DUI_CTR_WKEWEBKIT						   (_T("WkeWebkit"))

MYCONTROLS_API DuiLib::CControlUI* MyCreateControl(LPCTSTR pstrClass);
MYCONTROLS_API extern bool bIsRunByLua;
#endif//MYCONTROL_H