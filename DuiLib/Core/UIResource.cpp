#include "StdAfx.h"
#include "UIResource.h"
#include "Utils/unzip.h"

#ifndef TRACE
#define TRACE
#endif


namespace DuiLib
{
	///////////////////////////////////////////////////////////////////////////////////////
	////
	CResource::CResource():m_pData(NULL), m_dwSize(0)
	{}
	CResource::~CResource()
	{
		Release();
	}

	void CResource::Release()
	{
		if (NULL != m_pData)
		{
			free(m_pData);
			m_pData = NULL;
		}
		m_dwSize = 0;
	}

	BOOL CResource::IsValid() const
	{
		return m_pData != NULL && m_dwSize > 0;
	}
	LPBYTE CResource::GetData() const
	{
		return m_pData;
	}
	DWORD CResource::GetLength() const
	{
		return m_dwSize;
	}

	BOOL CResource::_Failed(LPCTSTR pstrError, LPCTSTR pstrLocation)
	{
		// Register last error
		TRACE(_T("Resource Error: %s"), pstrError);
		if (pstrLocation != NULL) TRACE(pstrLocation);
		return FALSE; // Always return 'false'
	}

	BOOL CResource::LoadFromMem(BYTE* pByte, DWORD dwSize)
	{
		m_pData = static_cast<LPBYTE>(malloc(dwSize));
		::CopyMemory(m_pData, pByte, dwSize);
		m_dwSize = dwSize;
		return TRUE;
	}

	BOOL CResource::LoadFromData(LPCTSTR pstrData)
	{
		Release();
		SIZE_T cchLen = _tcslen(pstrData) + 1;
		m_pData = static_cast<LPBYTE>(malloc(cchLen * sizeof(TCHAR)));
		::CopyMemory(m_pData, pstrData, cchLen * sizeof(TCHAR));
		m_dwSize = cchLen * sizeof(TCHAR);
		return TRUE;
	}

	BOOL CResource::LoadFromFile(LPCTSTR pstrFilename)
	{
		Release();

		HANDLE hFile = ::CreateFile(pstrFilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE) return _Failed(_T("Error opening file"));
		DWORD dwSize = ::GetFileSize(hFile, NULL);
		if (dwSize == 0) return _Failed(_T("File is empty"));
		if (dwSize > 4096 * 1024) return _Failed(_T("File too large"));

		DWORD dwRead = 0;
		BYTE* pByte = new BYTE[dwSize];
		::ReadFile(hFile, pByte, dwSize, &dwRead, NULL);
		::CloseHandle(hFile);

		if (dwRead != dwSize) {
			delete[] pByte;
			Release();
			return _Failed(_T("Could not read file"));
		}
		BOOL ret = LoadFromMem(pByte, dwSize);
		delete[] pByte;
		return ret;
	}

	BOOL CResource::LoadFromFileDirectly(LPCTSTR pstrPath, LPCTSTR pstrFilename)
	{
		CDuiString sFile = Path::CombinePath(pstrPath, pstrFilename);
		return LoadFromFile(sFile);
	}

	BOOL CResource::LoadFromZipFile(HANDLE handle, LPCTSTR pstrFilename)
	{
		HZIP hz = (HZIP)handle;
		ZIPENTRY ze;
		int i;
		if (FindZipItem(hz, pstrFilename, true, &i, &ze) != 0) return _Failed(_T("Could not find ziped file"));
		DWORD dwSize = ze.unc_size;
		if (dwSize == 0) return _Failed(_T("File is empty"));
		if (dwSize > 4096 * 1024) return _Failed(_T("File too large"));
		BYTE* pByte = new BYTE[dwSize];
		int res = UnzipItem(hz, i, pByte, dwSize);
		if (res != ZR_OK && res != ZR_MORE) {
			delete[] pByte;
			return _Failed(_T("Could not unzip file"));
		}
		BOOL ret = LoadFromMem(pByte, dwSize);
		delete[] pByte;

		return ret;
	}

	BOOL CResource::LoadFromZipFile(LPCTSTR pstrZip, LPCTSTR pstrFilename, LPCTSTR pstrPassword)
	{
		Release();

		HZIP hz = NULL;
		hz = OpenZip(pstrZip, pstrPassword ? Convert::ToUTF8(pstrPassword).c_str() : NULL);
		if (hz == NULL) return _Failed(_T("Error opening zip file"));

		BOOL ret = LoadFromZipFile(hz, pstrFilename);
		CloseZip(hz);
		return ret;
	}

	BOOL CResource::LoadFromResourceFile(HINSTANCE hInst, LPCTSTR pstrFilename, LPCTSTR type)
	{
		if (!type) {
			return FALSE;
		}
		HRSRC hResource = ::FindResource(hInst, pstrFilename, type);
		if (hResource == NULL) return FALSE;
		HGLOBAL hGlobal = ::LoadResource(hInst, hResource);
		if (hGlobal == NULL) {
			FreeResource(hResource);
			return FALSE;
		}
		if (!LoadFromMem((BYTE*)::LockResource(hGlobal), ::SizeofResource(hInst, hResource))) {
			FreeResource(hResource);
			return FALSE;
		}

		::FreeResource(hResource);
		return TRUE;
	}

	BOOL CResource::LoadFromZipResourceFile(HINSTANCE hInst, LPCTSTR lpName, LPCTSTR pstrFilename, LPCTSTR lpType/* = _T("ZIPRES")*/, LPCTSTR pstrPassword/* = NULL*/)
	{
		HANDLE handle = LoadFromResourceZip(hInst, lpName, lpType, pstrPassword);
		if (!handle) return FALSE;
		
		return LoadFromZipFile(handle, pstrFilename);
	}

	HANDLE CResource::LoadFromResourceZip(HINSTANCE hInst, LPCTSTR lpName, LPCTSTR lpType, LPCTSTR pstrPassword)
	{
		HRSRC hResource = ::FindResource(hInst, lpName, lpType);
		if (hResource == NULL)
		{
			_Failed(_T("Could not find ziped resource"));
			return NULL;
		}
		DWORD dwSize = 0;
		HGLOBAL hGlobal = ::LoadResource(hInst, hResource);
		if (hGlobal == NULL)
		{
			_Failed(_T("Could not load ziped resource"));
			::FreeResource(hResource);
			return NULL;
		}
		dwSize = ::SizeofResource(hInst, hResource);
		if (dwSize == 0)
		{
			_Failed(_T("ziped resource is invalid"));
			return NULL;
		}
		LPBYTE lpResourceZIPBuffer = new BYTE[dwSize];
		if (lpResourceZIPBuffer != NULL)
		{
			::CopyMemory(lpResourceZIPBuffer, (LPBYTE)::LockResource(hGlobal), dwSize);
		}
		::FreeResource(hResource);

		HANDLE hz = (HANDLE)OpenZip(lpResourceZIPBuffer, dwSize, pstrPassword ? Convert::ToUTF8(pstrPassword).c_str() : NULL);
		
		delete[] lpResourceZIPBuffer;
		return hz;
	}

	BOOL CResource::FindFile(LPCTSTR pstrFilename)
	{
		HANDLE hFile = ::CreateFile(pstrFilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE) return FALSE;
		::CloseHandle(hFile);
		return TRUE;
	}
	BOOL CResource::FindFileFromZip(LPCTSTR pstrZip, LPCTSTR pstrFilename, LPCTSTR pstrPassword/* = NULL*/)
	{
		HZIP hz = NULL;
		hz = OpenZip(pstrZip, pstrPassword ? Convert::ToUTF8(pstrPassword).c_str() : NULL);
		if (hz == NULL) return FALSE;

		BOOL ret = FindFileFromZip(hz, pstrFilename);
		CloseZip(hz);
		return ret;
	}
	BOOL CResource::FindFileFromZip(HANDLE handle, LPCTSTR pstrFilename)
	{
		HZIP hz = (HZIP)handle;
		if (hz == NULL) return FALSE;

		ZIPENTRY ze;
		int i;
		if (FindZipItem(hz, pstrFilename, true, &i, &ze) != 0)
		{
			return FALSE;
		}
		return TRUE;
	}
	BOOL CResource::FindFileFromResource(HINSTANCE hInst, LPCTSTR pstrFilename, LPCTSTR type)
	{
		if (!type){
			return FALSE;
		}
		HRSRC hResource = ::FindResource(hInst, pstrFilename, type);
		if (hResource == NULL) return false;
		HGLOBAL hGlobal = ::LoadResource(hInst, hResource);
		if (hGlobal == NULL) {
			FreeResource(hResource);
			return FALSE;
		}
		::FreeResource(hResource);
		return TRUE;
	}

	BOOL CResource::FindFileFromZipResource(HINSTANCE hInst, LPCTSTR lpName, LPCTSTR pstrFilename, LPCTSTR lpType/* = _T("ZIPRES")*/, LPCTSTR pstrPassword/* = NULL*/)
	{
		HANDLE handle = LoadFromResourceZip(hInst, lpName, lpType, pstrPassword);
		if (!handle) return FALSE;
		HZIP hz = (HZIP)handle;
		ZIPENTRY ze;
		int i;
		if (FindZipItem(hz, pstrFilename, true, &i, &ze) != 0)
		{
			return FALSE;
		}
		return TRUE;
	}
	
	///////////////////////////////////////////////////////////////////////////////////////
	////
	CResourceManager* CResourceManager::Instance()
	{
		static CResourceManager instance;
		return &instance;
	}

	CResourceManager::CResourceManager()
	{}

	BOOL CResourceManager::FindAsset(LPCTSTR szFileName, LPCTSTR type)
	{
		CResource ResLoader;
		if (CPaintManagerUI::GetResourceType() == UILIB_RESOURCETYPE::UILIB_FILE)
		{
			CDuiString sFile = Path::CombinePath(CPaintManagerUI::GetResourcePath(), szFileName);
			return ResLoader.FindFile(sFile.GetData());
		}
		else if (CPaintManagerUI::GetResourceType() == UILIB_RESOURCETYPE::UILIB_ZIP)
		{
			CDuiString szFullZipName = Path::CombinePath(CPaintManagerUI::GetResourcePath(), CPaintManagerUI::GetResourceZip());
			return ResLoader.FindFileFromZip(szFullZipName, szFileName, CPaintManagerUI::GetResourceZipPwd().GetData());
		}
		else if (CPaintManagerUI::GetResourceType() == UILIB_RESOURCETYPE::UILIB_RESOURCE)
		{
			return ResLoader.FindFileFromResource(CPaintManagerUI::GetResourceDll(), szFileName, type);
		}
		else if (CPaintManagerUI::GetResourceType() == UILIB_RESOURCETYPE::UILIB_ZIPRESOURCE)
		{
			STRINGorID resID(CPaintManagerUI::GetResourceID());
			return ResLoader.FindFileFromZipResource(CPaintManagerUI::GetResourceDll(), resID.m_lpstr, szFileName, _T("ZIPRES"), CPaintManagerUI::GetResourceZipPwd().GetData());
		}
		return FALSE;
	}

	BOOL CResourceManager::LoadAsset(LPCTSTR szFileName, LPCTSTR type, BYTE** ppBuffer, DWORD* size)
	{
		BOOL bResult = FALSE;
		CResource ResLoader;
		if (CPaintManagerUI::GetResourceType() == UILIB_RESOURCETYPE::UILIB_FILE)
		{
			if (ResLoader.LoadFromFileDirectly(CPaintManagerUI::GetResourcePath(), szFileName))
				bResult = TRUE;
		}
		else if (CPaintManagerUI::GetResourceType() == UILIB_RESOURCETYPE::UILIB_ZIP)
		{
			CDuiString szFullZipName = Path::CombinePath(CPaintManagerUI::GetResourcePath(), CPaintManagerUI::GetResourceZip());
			if (ResLoader.LoadFromZipFile(szFullZipName, szFileName, CPaintManagerUI::GetResourceZipPwd().GetData()))
				bResult = TRUE;
		}
		else if (CPaintManagerUI::GetResourceType() == UILIB_RESOURCETYPE::UILIB_RESOURCE)
		{
			if (ResLoader.LoadFromResourceFile(CPaintManagerUI::GetResourceDll(), szFileName, type))
				bResult = TRUE;
		}
		else if (CPaintManagerUI::GetResourceType() == UILIB_RESOURCETYPE::UILIB_ZIPRESOURCE)
		{
			if (!CPaintManagerUI::IsCachedResourceZip())
			{
				HINSTANCE hCurInst = CPaintManagerUI::GetResourceDll();
				STRINGorID resourceID(CPaintManagerUI::GetResourceID());

				HANDLE handle = ResLoader.LoadFromResourceZip(hCurInst, resourceID.m_lpstr, _T("ZIPRES"), CPaintManagerUI::GetResourceZipPwd().GetData());
				if (handle)
				{
					CPaintManagerUI::SetResourceZip(handle);
					if (ResLoader.LoadFromZipFile(handle, szFileName)) bResult = TRUE;
				}
			}
			else
			{
				HANDLE handle = CPaintManagerUI::GetResourceZipHandle();
				if (ResLoader.LoadFromZipFile(handle, szFileName)) bResult = TRUE;
			}
		}
		if (!bResult)
		{
			//������ͼƬ, ��ֱ��ȥ��ȡszFileNameָ���·��
			if (!ResLoader.LoadFromFile(szFileName))
				return FALSE;
		}
		if (!ResLoader.IsValid())
			return FALSE;

		const LPBYTE data = ResLoader.GetData();
		DWORD dwSize = ResLoader.GetLength();
		BYTE* pBuffer = new BYTE[dwSize];
		memset(pBuffer, 0, dwSize);
		memcpy(pBuffer, data, dwSize);
		*size = dwSize;
		*ppBuffer = pBuffer;
		return TRUE;

	}
	void CResourceManager::FreeAsset(BYTE* pBuffer)
	{
		if (pBuffer)
		{
			delete[] pBuffer;
		}
	}
}