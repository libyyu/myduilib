#include "StdAfx.h"
#include "UIResource.h"

#ifndef TRACE
#define TRACE
#endif

///////////////////////////////////////////////////////////////////////////////////////
DECLARE_HANDLE(HZIP);	// An HZIP identifies a zip file that has been opened
typedef DWORD ZRESULT;
typedef struct
{
	int index;                 // index of this file within the zip
	char name[MAX_PATH];       // filename within the zip
	DWORD attr;                // attributes, as in GetFileAttributes.
	FILETIME atime, ctime, mtime;// access, create, modify filetimes
	long comp_size;            // sizes of item, compressed and uncompressed. These
	long unc_size;             // may be -1 if not yet known (e.g. being streamed in)
} ZIPENTRY;
typedef struct
{
	int index;                 // index of this file within the zip
	TCHAR name[MAX_PATH];      // filename within the zip
	DWORD attr;                // attributes, as in GetFileAttributes.
	FILETIME atime, ctime, mtime;// access, create, modify filetimes
	long comp_size;            // sizes of item, compressed and uncompressed. These
	long unc_size;             // may be -1 if not yet known (e.g. being streamed in)
} ZIPENTRYW;
#define OpenZip OpenZipU
#define CloseZip(hz) CloseZipU(hz)
extern HZIP OpenZipU(void *z, unsigned int len, DWORD flags);
extern ZRESULT CloseZipU(HZIP hz);
#ifdef _UNICODE
#define ZIPENTRY ZIPENTRYW
#define GetZipItem GetZipItemW
#define FindZipItem FindZipItemW
#else
#define GetZipItem GetZipItemA
#define FindZipItem FindZipItemA
#endif
extern ZRESULT GetZipItemA(HZIP hz, int index, ZIPENTRY *ze);
extern ZRESULT GetZipItemW(HZIP hz, int index, ZIPENTRYW *ze);
extern ZRESULT FindZipItemA(HZIP hz, const TCHAR *name, bool ic, int *index, ZIPENTRY *ze);
extern ZRESULT FindZipItemW(HZIP hz, const TCHAR *name, bool ic, int *index, ZIPENTRYW *ze);
extern ZRESULT UnzipItem(HZIP hz, int index, void *dst, unsigned int len, DWORD flags);
///////////////////////////////////////////////////////////////////////////////////////

namespace DuiLib
{
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
		int res = UnzipItem(hz, i, pByte, dwSize, 3);
		if (res != 0x00000000 && res != 0x00000600) {
			delete[] pByte;
			return _Failed(_T("Could not unzip file"));
		}
		BOOL ret = LoadFromMem(pByte, dwSize);
		delete[] pByte;

		return ret;
	}

	BOOL CResource::LoadFromZipFile(LPCTSTR pstrZip, LPCTSTR pstrFilename)
	{
		Release();

		HZIP hz = NULL;
		hz = OpenZip((void*)pstrZip, 0, 2);
		if (hz == NULL) return _Failed(_T("Error opening zip file"));

		BOOL ret = LoadFromZipFile(hz, pstrFilename);
		CloseZip(hz);
		return ret;
	}

	HANDLE CResource::LoadFromZipResource(HINSTANCE hInst, LPCTSTR lpName, LPCTSTR lpType)
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

		HANDLE hz = (HANDLE)OpenZip(lpResourceZIPBuffer, dwSize, 3);
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
	BOOL CResource::FindFileFromZip(LPCTSTR pstrZip, LPCTSTR pstrFilename)
	{
		HZIP hz = NULL;
		hz = OpenZip((void*)pstrZip, 0, 2);
		if (hz == NULL) return FALSE;

		ZIPENTRY ze;
		int i;
		if (FindZipItem(hz, pstrFilename, true, &i, &ze) != 0)
		{
			CloseZip(hz);
			return FALSE;
		}
		CloseZip(hz);
		return TRUE;
	}

	BOOL CResource::FindFileFromResource(LPCTSTR pstrFilename, LPCTSTR type)
	{
		if (!type){
			return FALSE;
		}
		HRSRC hResource = ::FindResource(CPaintManagerUI::GetResourceDll(), pstrFilename, type);
		if (hResource == NULL) return false;
		HGLOBAL hGlobal = ::LoadResource(CPaintManagerUI::GetResourceDll(), hResource);
		if (hGlobal == NULL) {
			FreeResource(hResource);
			return FALSE;
		}
		::FreeResource(hResource);
		return TRUE;
	}

	BOOL CResource::FindFileFromZipResource(LPCTSTR pstrFilename)
	{
		if (!CPaintManagerUI::IsCachedResourceZip())
		{
			HINSTANCE hResource = CPaintManagerUI::GetResourceDll();
			STRINGorID ResourceID(CPaintManagerUI::GetResourceID());

			HANDLE handle = LoadFromZipResource(hResource, ResourceID.m_lpstr, _T("ZIPRES"));
			if (!handle) return FALSE;
			//CPaintManagerUI::SetResourceZip(handle);

			HZIP hz = (HZIP)handle;
			ZIPENTRY ze;
			int i;
			if (FindZipItem(hz, pstrFilename, true, &i, &ze) != 0)
			{
				CloseZip(hz);
				return FALSE;
			}
			CloseZip(hz);
			return TRUE;
		}
		else
		{
			HANDLE handle = CPaintManagerUI::GetResourceZipHandle();
			HZIP hz = (HZIP)handle;
			ZIPENTRY ze;
			int i;
			if (FindZipItem(hz, pstrFilename, true, &i, &ze) != 0)
			{
				return FALSE;
			}
			return TRUE;
		}
	}

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
			return ResLoader.FindFileFromZip(szFullZipName, szFileName);
		}
		else if (CPaintManagerUI::GetResourceType() == UILIB_RESOURCETYPE::UILIB_RESOURCE)
		{
			return ResLoader.FindFileFromResource(szFileName, type);
		}
		else if (CPaintManagerUI::GetResourceType() == UILIB_RESOURCETYPE::UILIB_ZIPRESOURCE)
		{
			return ResLoader.FindFileFromZipResource(szFileName);
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
			if (ResLoader.LoadFromZipFile(szFullZipName, szFileName))
				bResult = TRUE;
		}
		else if (CPaintManagerUI::GetResourceType() == UILIB_RESOURCETYPE::UILIB_RESOURCE)
		{
			if (!type)
			{
				TRACE(_T("(LoadImage)Failed LoadImage %s, type is null."), szFileName);
			}
			else
			{
				HRSRC hResource = ::FindResource(CPaintManagerUI::GetResourceDll(), szFileName, type);
				if (hResource)
				{
					HGLOBAL hGlobal = ::LoadResource(CPaintManagerUI::GetResourceDll(), hResource);
					if (hGlobal) 
					{
						if (ResLoader.LoadFromMem((BYTE*)::LockResource(hGlobal), ::SizeofResource(CPaintManagerUI::GetResourceDll(), hResource))) 
						{
							bResult = TRUE;
						}
					}
					::FreeResource(hResource);
				}
			}
		}
		else if (CPaintManagerUI::GetResourceType() == UILIB_RESOURCETYPE::UILIB_ZIPRESOURCE)
		{
			if (!CPaintManagerUI::IsCachedResourceZip())
			{
				HINSTANCE hCurInst = CPaintManagerUI::GetResourceDll();
				STRINGorID resourceID(CPaintManagerUI::GetResourceID());

				HANDLE handle = ResLoader.LoadFromZipResource(hCurInst, resourceID.m_lpstr, _T("ZIPRES"));
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
			//读不到图片, 则直接去读取szFileName指向的路径
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