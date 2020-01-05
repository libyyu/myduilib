#ifdef _WIN32
#	include <winsock2.h>
#	include <windows.h>
#endif
#include "COnlineDecoder.h"
#include "Utility/log.h"

COnlineDecoder::COnlineDecoder()
	:_hWrite(INVALID_HANDLE_VALUE),
	_nFileSize(0),
	_nDownSize(0)
{

}

COnlineDecoder::~COnlineDecoder()
{

}
bool COnlineDecoder::open(const char* pszFileName,int nBegin,int nEnd)
{
	_hWrite = ::CreateFileA(pszFileName,GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	sLog("CDecoderBase::open fun,CreateFile to write,Error=%d",::GetLastError());
	_hRead  = ::CreateFileA(pszFileName,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	sLog("CDecoderBase::open fun,CreateFile to read Error=%d",::GetLastError());
	return (INVALID_HANDLE_VALUE != _hWrite) && (INVALID_HANDLE_VALUE != _hRead);
}
void COnlineDecoder::close()
{
	if(INVALID_HANDLE_VALUE != _hWrite)
	{
		::CloseHandle(_hWrite);
		_hWrite = INVALID_HANDLE_VALUE;
	}

	this->_nFileSize = 0;
	this->_nDownSize = 0;

	return __super::close();
}
bool COnlineDecoder::getInfo(struct player_audio_info* p_info) const
{
	assert(p_info);
	if(!p_info)
		return false;
	if(!isInitStream())
	{
		memset(p_info,0,sizeof(struct player_audio_info));
		return true;
	}

	return __super::getInfo(p_info);
}
bool COnlineDecoder::decodeOnce()
{
	if(!isInitStream())
	{
		if(isDownFinish())
		{
			if(!initStreamFormat())
				return false;
		}
		else if(_nDownSize >= MIN_OFFSET)
		{
			if(!initStreamFormat())
				return true;
		}
		else
		{
			return true;
		}
	}
	
	assert(isInitStream());
	if(_nDownSize - _pFmtCtx->pb->pos >= MIN_OFFSET || isDownFinish())
	{
		return __super::decodeOnce();
	}

	return true;
}
size_t COnlineDecoder::writeData(unsigned char *p_data,int n_buf_size)
{
	assert(_hWrite);
	if(!_hWrite)
		return 0;

	if(NULL == p_data)
	{
		/* 下载完毕 */
		if(0== n_buf_size) 
		{
			assert(_nDownSize == _nFileSize);
			::FlushFileBuffers(_hWrite);
			return 0;
		}

		/* 第一次写数据，仅通知文件大小 */

		//assert(_nDownSize == 0);
		//assert(_nFileSize == 0);
		_nFileSize = n_buf_size;
		assert(n_buf_size > 0);
		return 0;
	}

	/* 开始写音频数据 */
	int bytes = 0;
	while(bytes < n_buf_size)
	{
		DWORD dw = 0;
		::WriteFile(_hWrite,p_data+bytes,n_buf_size - bytes,&dw,NULL);
		if(dw < 0)
			break;
		bytes += dw;
		_nDownSize += dw;
	}
	return bytes;
}
size_t COnlineDecoder::downBytes() const
{
	return _nDownSize;
}
bool COnlineDecoder::isFinish() const
{
	return isDownFinish() && __super::isFinish();
}

__int64 COnlineDecoder::ReadPosition() const
{
	return __super::ReadPosition();
}

bool COnlineDecoder::isDownFinish() const
{
	return (_nDownSize >= _nFileSize) && (_nFileSize > 0);
}
int64_t COnlineDecoder::onSeek(int64_t offset, int whence)
{
	HANDLE h = this->_hRead;
	whence &= ~AVSEEK_FORCE;
	DWORD dwRet = 0;
	switch(whence)
	{
	case SEEK_SET:
		dwRet = ::SetFilePointer(h,offset,NULL,FILE_BEGIN);
		return  dwRet;
	case SEEK_CUR:
		dwRet = ::SetFilePointer(h,offset,NULL,FILE_CURRENT);
		return dwRet;
	case SEEK_END:
		dwRet = ::SetFilePointer(h,offset,NULL,FILE_END);
		return dwRet;
	case AVSEEK_SIZE:
		return  _nFileSize;
	default:
		break;
	}
	return -1;
	
}