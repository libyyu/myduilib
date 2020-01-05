#ifndef _URL_H_
#define _URL_H_
#pragma once
#include <cstdio>
#include <cstdlib>
#include <string>
namespace url
{
	std::string urlencode(const std::string& szToEncode)
	{
		std::string src = szToEncode;
		char hex[] = "0123456789ABCDEF";
		std::string dst;

		for (size_t i = 0; i < src.size(); ++i)
		{
			unsigned char cc = src[i];
			if (isascii(cc))
			{
				if (cc == ' ')
				{
					dst += "%20";
				}
				else
					dst += cc;
			}
			else
			{
				unsigned char c = static_cast<unsigned char>(src[i]);
				dst += '%';
				dst += hex[c / 16];
				dst += hex[c % 16];
			}
		}
		return dst;
	}

	std::string urldecode(const std::string& szToDecode)
	{
		std::string result;
		int hex = 0;
		for (size_t i = 0; i < szToDecode.length(); ++i)
		{
			switch (szToDecode[i])
			{
			case '+':
				result += ' ';
				break;
			case '%':
				if (isxdigit(szToDecode[i + 1]) && isxdigit(szToDecode[i + 2]))
				{
					std::string hexStr = szToDecode.substr(i + 1, 2);
					hex = strtol(hexStr.c_str(), 0, 16);
					//字母和数字[0-9a-zA-Z]、一些特殊符号[$-_.+!*'(),] 、以及某些保留字[$&+,/:;=?@]
					//可以不经过编码直接用于URL
					if (!((hex >= 48 && hex <= 57) || //0-9
						(hex >= 97 && hex <= 122) ||   //a-z
						(hex >= 65 && hex <= 90) ||    //A-Z
													   //一些特殊符号及保留字[$-_.+!*'(),]  [$&+,/:;=?@]
						hex == 0x21 || hex == 0x24 || hex == 0x26 || hex == 0x27 || hex == 0x28 || hex == 0x29
						|| hex == 0x2a || hex == 0x2b || hex == 0x2c || hex == 0x2d || hex == 0x2e || hex == 0x2f
						|| hex == 0x3A || hex == 0x3B || hex == 0x3D || hex == 0x3f || hex == 0x40 || hex == 0x5f
						))
					{
						result += char(hex);
						i += 2;
					}
					else result += '%';
				}
				else {
					result += '%';
				}
				break;
			default:
				result += szToDecode[i];
				break;
			}
		}
		return result;
	}

	std::string urlencode_ex(const std::string& szToEncode)
	{
		char const* in_str = szToEncode.c_str();
		int in_str_len = strlen(in_str);
		int out_str_len = 0;
		std::string out_str;
		register unsigned char c;
		unsigned char* to, * start;
		unsigned char const* from, * end;
		unsigned char hexchars[] = "0123456789ABCDEF";

		from = (unsigned char*)in_str;
		end = (unsigned char*)in_str + in_str_len;
		start = to = (unsigned char*)malloc(3 * in_str_len + 1);

		while (from < end)
		{
			c = *from++;

			if (c == ' ')
			{
				*to++ = '+';
			}
			else if ((c < '0' && c != '-' && c != '.') ||
				(c < 'A' && c > '9') ||
				(c > 'Z' && c < 'a' && c != '_') ||
				(c > 'z'))
			{
				to[0] = '%';
				to[1] = hexchars[c >> 4];
				to[2] = hexchars[c & 15];
				to += 3;
			}
			else
			{
				*to++ = c;
			}
		}
		*to = 0;

		out_str_len = to - start;
		out_str = (char*)start;
		free(start);
		return out_str;
	}

	enum emUrlT
	{
		URLT_NULL,  //或者地址错误
		URLT_FTP,
		URLT_HTTP,
		URLT_HTTPS,
	};

	emUrlT      ParseURL(const std::string& url, std::string& host, std::string& path, unsigned int& port)
	{
		static const char* lpHttp = "http://";
		static const char* lpHttps = "https://";
		static const char* lpftp = "ftp://";
		
		std::string szURL = url;
		std::replace(szURL.begin(), szURL.end(), '\\', '/');
		
#ifdef _MSC_VER
#define local_compare _strnicmp
#else
#define local_compare strncasecmp
#endif

		size_t  nSize = 0;
		emUrlT urltype = emUrlT::URLT_NULL;
		//判断协议
		if (szURL.size() >strlen(lpHttp) && 0 == local_compare(lpHttp, szURL.c_str(), strlen(lpHttp)))
		{
			nSize = strlen(lpHttp);
			urltype = emUrlT::URLT_HTTP;
		}
		else if (szURL.size() >strlen(lpHttps) && 0 == local_compare(lpHttps, szURL.c_str(), strlen(lpHttps)))
		{
			nSize = strlen(lpHttps);
			urltype = emUrlT::URLT_HTTPS;
		}
		else if (szURL.size() >strlen(lpftp) && 0 == local_compare(lpftp, szURL.c_str(), strlen(lpftp)))
		{
			nSize = strlen(lpftp);
			urltype = emUrlT::URLT_FTP;
		}
		else
		{
			urltype = emUrlT::URLT_NULL;
		}
		host = szURL.substr(0, nSize);
#undef local_compare
		//找端口
		port = 80;
		size_t port_begin = 0;
		size_t page_begin = nSize;
		for (size_t i = nSize; i < szURL.size(); i++)
		{
			if (':' == szURL[i])
			{
				port_begin = i;
			}
			else if('/' == szURL[i])
			{
				page_begin = i;
				break;
			}
		}
		if(port_begin)
		{
			std::string strport = szURL.substr(port_begin, page_begin);
		}
		//找页面
		path = szURL.substr(page_begin);

		printf("host = %s, path = %s, port = %d\n", host.c_str(), path.c_str(), port);

		return urltype;
	}
}

#endif
