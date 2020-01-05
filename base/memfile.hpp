#ifndef MEMFILE_HPP
#define MEMFILE_HPP
#include <stdlib.h>
#include <string>

class CMemFile{
public:
	typedef bool(*PMemFileOpenCallBack)(const char*, unsigned char**, unsigned int*);
	typedef void(*PMemFileReleaseCallBack)(unsigned char*);

	CMemFile(){
		m_pMemIOBuf = NULL;
		m_open_func = NULL;
		m_release_func = NULL;
		DiscardFile();
	}
	CMemFile(const char* strDiskFilePath){
		m_open_func = NULL;
		m_release_func = NULL;
		m_pMemIOBuf = NULL;
		m_uFileLength = 0;
		m_uCurReadPos = 0;
		m_strDiskFilePath[0] = 0x0;
		RedirectToFile(strDiskFilePath);
	}
	virtual ~CMemFile(){
		DiscardFile();
	}

	void SetOpenCallBack(PMemFileOpenCallBack pFunc){
		m_open_func = pFunc;
	}
	void SetReleaseCallBack(PMemFileReleaseCallBack pFunc){
		m_release_func = pFunc;
	}

public:
	bool RedirectToFile(const char* strDiskFilePath){
		if (!strDiskFilePath) return false;
		DiscardFile();
		sprintf(m_strDiskFilePath, "%s", strDiskFilePath);

		if (m_open_func)
		{
			unsigned int len;
			bool b = m_open_func(m_strDiskFilePath, &m_pMemIOBuf, &len);
			m_uFileLength = len;
			return b;
		}
		
		FILE* fp = NULL;
		fopen_s(&fp, m_strDiskFilePath, "rb");
		if (!fp) return false;

		unsigned long dwPos = ftell(fp);
		fseek(fp, 0, SEEK_END);
		unsigned long dwTail = ftell(fp);
		fseek(fp, dwPos, SEEK_SET);
		m_uFileLength = dwTail - dwPos;

		m_pMemIOBuf = new unsigned char[m_uFileLength];
		memset(m_pMemIOBuf, 0, m_uFileLength);
		fread(m_pMemIOBuf, 1, m_uFileLength, fp);
		fclose(fp);
		return true;
	}
	unsigned long GetFileLength() const{
		return m_uFileLength;
	}
	unsigned long GetReadPosition() const{
		return m_uCurReadPos;
	}
	unsigned int SetReadPosition(const unsigned long uNewReadPosition){
		if (m_uFileLength && uNewReadPosition <= m_uFileLength){
			unsigned long uOldReadPos = m_uCurReadPos;
			m_uCurReadPos = uNewReadPosition;
			return uOldReadPos;
		}
		return -1;
	}
	const char*  GetDiskFilePath() const{
		return m_strDiskFilePath;
	}
	const unsigned char*  GetReadPointer() const{
		return m_pMemIOBuf + m_uCurReadPos;
	}

public:
	virtual bool ReadUInt32(unsigned int* v){
		if (m_uCurReadPos + sizeof(unsigned int) > m_uFileLength){
			return false;
		}

		*v = *((unsigned int*)(m_pMemIOBuf + m_uCurReadPos));
		m_uCurReadPos += sizeof(unsigned int);
		return true;
	}
	virtual bool  ReadInt32(int* v){
		if (m_uCurReadPos + sizeof(int) > m_uFileLength){
			return false;
		}

		*v = *((int*)(m_pMemIOBuf + m_uCurReadPos));
		m_uCurReadPos += sizeof(int);
		return true;
	}
	virtual bool ReadString(const unsigned int len, std::string& v){
		if (m_uCurReadPos >= m_uFileLength || len > m_uFileLength - m_uCurReadPos){
			return false;
		}
		unsigned char *pb = m_pMemIOBuf + m_uCurReadPos, *pe = m_pMemIOBuf + m_uCurReadPos + len;
		m_uCurReadPos += len;
		v = std::string(pb, pe);
		return true;
	}
	virtual bool Readable() {
		return m_uCurReadPos < m_uFileLength;
	}

private:
	void DiscardFile(){
		m_uCurReadPos = 0;
		m_uFileLength = 0;
		m_strDiskFilePath[0] = 0x0;
		if (m_pMemIOBuf != NULL)
		{
			if (m_release_func)
				m_release_func(m_pMemIOBuf);
			else
				delete[] m_pMemIOBuf;

			m_pMemIOBuf = NULL;
		}
	}

private:
	unsigned long     m_uCurReadPos;
	unsigned long     m_uFileLength;
	unsigned char*   m_pMemIOBuf;
	char			 m_strDiskFilePath[256];
	PMemFileOpenCallBack  m_open_func;
	PMemFileReleaseCallBack  m_release_func;

};

#endif//MEMFILE_HPP