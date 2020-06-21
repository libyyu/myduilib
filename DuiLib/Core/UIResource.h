#ifndef __UIRESOURCE_H__
#define __UIRESOURCE_H__

#pragma once

namespace DuiLib
{
	class UILIB_API CResource
	{
	public:
		CResource();
		~CResource();

		void Release();
		BOOL IsValid() const;
		LPBYTE GetData() const;
		DWORD GetLength() const;

		BOOL LoadFromData(LPCTSTR pstrData);
		BOOL LoadFromMem(BYTE* pByte, DWORD dwSize);
		BOOL LoadFromFile(LPCTSTR pstrFilename);
		BOOL LoadFromFileDirectly(LPCTSTR pstrPath, LPCTSTR pstrFilename);
		BOOL LoadFromZipFile(LPCTSTR pstrZip, LPCTSTR pstrFilename);
		BOOL LoadFromZipFile(HANDLE hz, LPCTSTR pstrFilename);
		HANDLE LoadFromZipResource(HINSTANCE hInst, LPCTSTR lpName, LPCTSTR lpType);

		BOOL FindFile(LPCTSTR pstrFilename);
		BOOL FindFileFromZip(LPCTSTR pstrZip, LPCTSTR pstrFilename);
		BOOL FindFileFromResource(LPCTSTR pstrFilename, LPCTSTR type);
		BOOL FindFileFromZipResource(LPCTSTR pstrFilename);
	protected:
		BOOL _Failed(LPCTSTR pstrError, LPCTSTR pstrLocation = NULL);
	private:
		DWORD m_dwSize;
		LPBYTE m_pData;
	};

	class UILIB_API CResourceManager
	{
		CResourceManager();
	public:
		static CResourceManager* Instance();
		BOOL FindAsset(LPCTSTR filename, LPCTSTR type);
		BOOL LoadAsset(LPCTSTR filename, LPCTSTR type, BYTE** ppBuffer, DWORD* size);
		void FreeAsset(BYTE* pBuffer);
	};
}

#endif//__UIRESOURCE_H__