#pragma once

#ifdef _MSC_VER
#ifndef _XIAMI_ENCRYPT_API_
#define _XIAMI_ENCRYPT_API_  __declspec(dllimport)
#endif
#else
#define _XIAMI_ENCRYPT_API_
#endif
#include <string>
using namespace std;
typedef unsigned long DWORD;
#ifndef _WIN32
typedef bool  BOOL;
#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif
#endif
#include "vlong.h"


#define LEVEL	32	
//当LEVEL=32表示1024位,LEVEL=64表示2048,LEVEL=128表示4096位的RSA

#define VL	LEVEL * 2
/*一次加密的数据块大小应该和m的比特相同，即使比它小就生成的密文都是m的比特
加/解密的数据VL*char字节，它最大长度就是LEVEL的长度，64
但是因为它要比m小，所以定LEVEL*DWORD比特既绝对安全又可以使可以构造的plain小于m*/
// Provides storage allocation and index checking
namespace  encrypt_mod
{

	struct PK	//公开密钥
	{
		DWORD m[LEVEL];	//公开密钥中要用大整数用DWORD表示
	};
	struct SK	//私人密钥
	{
		DWORD p[LEVEL / 2];
		DWORD q[LEVEL / 2];
	};

	#define DATA_LENGTH				1024 * 4

	struct MessageBuf
	{
		char text[LEVEL * 4-4];       //要加密的字节数

		DWORD nil[1];                 //把这个置为零
	};

    class _XIAMI_ENCRYPT_API_ Des_control
	{
	public:
		Des_control();
		~Des_control();
	public:
		char * Des_encrypt(char* buf_encrypt,int length,int &out_length);//encrypt
		char * Des_decrypt(char* buf_decrypt,int length,int &out_length);//decrypt
	public:
		std::string Des_get_key();
		void Des_create_key();
		void Des_set_key(const std::string& s_key);
		void Des_delete_buf(char *p_buf);
	private:
		string m_p_des_key; //des key
	};

	class _XIAMI_ENCRYPT_API_ public_key
	{
	public:
		public_key();	//构造函数为了使得requires为1，派生类private_key也会调用这个基类中的构造函数的
		void	encrypt(DWORD *dollop);	//加密消息块
		//要求plain必须小于m
		vlong	encrypt( const vlong& plain );	// Requires 0 <= plain < m
		void	PK_to_vlong(PK pk);
		void	vlong_to_PK(PK &pk);
		char*    encrypt(char *in_buf,int n_in_len,int& n_out_len) ;
	public:
		void	set_requires(int req);
		int		get_requires();
		void	delete_buf(char *p_buf);
		int		requires;	//判断加密的数是否小于m
		vlong	m, e;
	};

	class _XIAMI_ENCRYPT_API_ private_key : public public_key
	{/*private_key是从public_key派生出来的类*/
	public:	
		void	create();	//生成m, e, p, q; 安全级别是2048位

		void	decrypt(DWORD *dollop);	//解密消息块
		//要求plain必须小于m
		char*  decrypt(char *in_buf,int n_in_len,int &n_out_len); 
        
		vlong	decrypt( const vlong& cipher );// Requires 0 <= cipher < m
		void	SK_to_vlong(SK sk);
		void	vlong_to_SK(SK &sk);

		void delete_buf(char *p_buf);
	public:

		vlong	p, q;
		// r1 and r2 should be null terminated random strings
		// each of length around 35 characters (for a 500 bit modulus)
	};    






}

