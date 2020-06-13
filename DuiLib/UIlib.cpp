// Copyright (c) 2010-2011, duilib develop team(www.duilib.com).
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or 
// without modification, are permitted provided that the 
// following conditions are met.
//
// Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// Redistributions in binary form must reproduce the above 
// copyright notice, this list of conditions and the following
// disclaimer in the documentation and/or other materials 
// provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND 
// CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
// INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
// BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//
// DirectUI - UI Library
//
// Written by Bjarke Viksoe (bjarke@viksoe.dk)
// Copyright (c) 2006-2007 Bjarke Viksoe.
//
// This code may be used in compiled form in any way you desire. These
// source files may be redistributed by any means PROVIDING it is 
// not sold for profit without the authors written consent, and 
// providing that this notice and the authors name is included. 
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability if it causes any damage to you or your
// computer whatsoever. It's free, so don't hassle me about it.
//
// Beware of bugs.
//
//


#include "stdafx.h"
#include "UIlib.h"


HANDLE ghModuleDuiLib = INVALID_HANDLE_VALUE;
BOOL APIENTRY DllMain(HANDLE hModule, DWORD  dwReason, LPVOID /*lpReserved*/)
{
    switch( dwReason ) {
   case DLL_PROCESS_ATTACH:
   {
	   ghModuleDuiLib = hModule;
	   DuiLib::CreateLogger(NULL);
#ifdef UILIB_STACK_DUMP
	   DuiLib::HPLUGINS hDll = DuiLib::CSystem::Instance()->LoadPlugin(_T("StackDump.dll"));
	   if (hDll)
	   {
		   typedef void(*AttachBuglyFunc)(const char*, int);
		   AttachBuglyFunc pFunc = (AttachBuglyFunc)GetProcAddress(hDll, _T("AttachBugly"));
		   if (pFunc)
		   {
			   pFunc(NULL, 0);
		   }
		   else
		   {
			   DuiLib::CSystem::Instance()->FreePlugin(hDll);
		   }
	   }
#endif //UILIB_STACK_DUMP
	   break;
   }
   case DLL_THREAD_ATTACH:
   case DLL_THREAD_DETACH:
	   break;
   case DLL_PROCESS_DETACH:
	   ghModuleDuiLib = INVALID_HANDLE_VALUE;
       ::DisableThreadLibraryCalls((HMODULE)hModule);
#ifdef UILIB_STACK_DUMP
	   DuiLib::CSystem::Instance()->FreePlugin(_T("StackDump.dll"));
#endif//UILIB_STACK_DUMP

#ifdef USE_CUSTOM_MEMORY
	   FMemory::DumpMemoryBlocks();
#endif //USE_CUSTOM_MEMORY
	   delete DuiLib::duiLogger;
	   DuiLib::duiLogger = NULL;
       break;
    }
    return TRUE;
}

