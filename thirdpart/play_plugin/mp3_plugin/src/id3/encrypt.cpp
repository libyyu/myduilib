// encrypt.cpp : Defines the entry point for the DLL application.
//

#ifdef _MSC_VER
#define _XIAMI_ENCRYPT_API_  __declspec(dllexport)
#include <Windows.h>
#endif
#include "encrypt.h"
#include <time.h>

#include "WjcDes.h"
#include "prime.h"
namespace  encrypt_mod
{

Des_control::Des_control()
{
}

Des_control::~Des_control()
{

}
char*  Des_control::Des_encrypt(char* buf_encrypt,int length,int &out_length)
{
	int des_one=8; //des 特定值
	int quotient, remainder, i; 
	quotient =  length / des_one; 
	remainder = length % des_one; 
	quotient = remainder?++quotient:quotient;               

	char *buf=new char[quotient*des_one];

	Des_Go(buf, buf_encrypt, length, m_p_des_key.c_str(), m_p_des_key.size(),out_length, ENCRYPT);

	return buf;

}
char* Des_control::Des_decrypt(char* buf_decrypt,int length,int &out_length)
{
	char *buf=new char[length];

	Des_Go(buf,buf_decrypt,length, m_p_des_key.c_str(), m_p_des_key.size(), out_length,DECRYPT);

	return buf;
}
std::string Des_control::Des_get_key()
{
	return m_p_des_key;
}
void  Des_control::Des_set_key(const std::string& s_key)
{
	m_p_des_key = s_key;
}

void Des_control::Des_delete_buf(char *p_buf)
{
	if (p_buf)
	{
		delete [] p_buf;
	}
}
void Des_control::Des_create_key() 
{
	const int SIZE_CHAR = 32;  
	const char CCH[] = "_0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_";
	srand((unsigned)time(NULL));
	char ch[SIZE_CHAR + 1] = {0};

	for (int i = 0; i < SIZE_CHAR; ++i)
	{
		int x = rand() % (sizeof(CCH) - 1);  

		ch[i] = CCH[x];
	}	
	m_p_des_key=ch;
	int aa=-1;

}
/*------------- RSA mod -------------------*/
void from_str(vlong &x, const char * s )
{
	//vlong x = 0;
	while (*s)
	{
		x = x * 256 + (unsigned char)*s;
		s += 1;
	}
	//return x;
}

public_key::public_key()
{
	/*在这里必须初始化requires为1，否则无法进行加解密
	在Debug版中它初始化就不会为0，而在Release版中它初始化为0
	这就会出现只有Release版才会出的错*/
	requires = true;
}

void private_key::create()
{
	char prand[2][LEVEL * 4/*必须不小于LEVEL*2 */], tc;
	DWORD i, j;
	DWORD validate[LEVEL];	//验证m的最高位是否为0
	prime_factory pf;
	vlong tmp;

start:	//条件不成立，重新再生成p和q
	srand((unsigned)time(NULL));
	for(i = 0; i < 2; i++)
	{
		for(j = 0; j < (LEVEL * 2); j++)
		{
			tc = (char)(0x41+rand()%0xAF);
			prand[i][j] = tc;
		}
		prand[i][j]=0;
	}
	// Choose primes
	vlong v_prand_1=0;
	vlong v_prand_2=0;
	from_str(v_prand_1,prand[0]);
	from_str(v_prand_2,prand[1]);
	p = pf.find_prime( v_prand_1 );//计算生成两个大奇数 p, q
	q = pf.find_prime( v_prand_2 );
	if ( p > q )
	{
		tmp = p;
		p = q;
		q = tmp;
	}

	// Calculate public key
	m = p * q;	//当m的最高位是非0时，要加密的大整数最高位是0，就满足加密的条件了
	m.store(validate, LEVEL);
	//如果这个m不能满足条件，就重新生成p和q，直到条件成立，其实这也不能完全解决问题还是要有requires
	if(validate[LEVEL - 1] == 0x00000000) goto start;

	e = 65537;	//如果这个固定的e不能满足条件，就重新生成p和q，直到条件成立
	if( gcd(p-1,e) != 1 || gcd(q-1,e) != 1 ) goto start;

	/*while ( gcd(p-1,e) != 1 || gcd(q-1,e) != 1 )
	{
	e += 2;
	}*/
	set_requires(TRUE);
}


vlong public_key::encrypt( const vlong& plain )
{
	if(plain >= m)
	{
		requires = false;
#ifdef _WIN32
		::MessageBoxA(NULL,"加密的数必须小于m", "public_key::encrypt",MB_ICONSTOP);
#endif
	}
	if(!requires)
	{	//只有出错一次就永远不能进行加密，只有再把requires=true
		return m;	
	}

	return modexp( plain, e, m );
}

vlong private_key::decrypt( const vlong& cipher )
{
	if(cipher >= m)
	{
		requires = false;
		//::MessageBox(NULL, "加密的数必须小于m", "private_key::decrypt", MB_ICONSTOP);
	}
	if(!requires)
	{	//只有出错一次就永远不能进行加密，只有再把requires=true
		//	::MessageBox(NULL,"我错了","private_key::decrypt",MB_ICONSTOP);

		return m;
	}

	// Calculate values for performing decryption
	// These could be cached, but the calculation is quite fast
	vlong d = modinv( e, (p-1)*(q-1) );
	vlong u = modinv( p, q );
	vlong dp = d % (p-1);
	vlong dq = d % (q-1);

	// Apply chinese remainder theorem
	vlong a = modexp( cipher % p, dp, p );
	vlong b = modexp( cipher % q, dq, q );
	if ( b < a ) b += q;
	return a + p * ( ((b-a)*u) % q );
}


void public_key::PK_to_vlong(PK pk)
{
	m.load(pk.m, LEVEL);
	e = 65537;	//一个固定的e
	set_requires(TRUE);
}

void private_key::SK_to_vlong(SK sk)
{
	p.load(sk.p, LEVEL / 2);
	q.load(sk.q, LEVEL / 2);

	m = p * q;
	e = 65537;	//一个固定的e
	set_requires(TRUE);
}

void public_key::vlong_to_PK(PK &pk)
{
	//把公开密钥转换成可用格式
	m.store(pk.m, LEVEL);	
}

void private_key::vlong_to_SK(SK &sk)
{
	//把私人密钥转换成可用格式
	p.store(sk.p, LEVEL / 2);
	q.store(sk.q, LEVEL / 2);
}

int public_key::get_requires()
{
	return requires;
}
void	public_key::delete_buf(char *p_buf)
{
	if (p_buf)
	{
		delete [] p_buf;
	}
	
}
void public_key::set_requires(int req)
{
	requires = req;
}

void public_key::encrypt(DWORD * dollop)
{

	vlong m, c;
	c.load( dollop, LEVEL);
	m = encrypt(c);
	m.store( dollop, LEVEL);

}

char* public_key::encrypt(char *in_buf,int n_in_len,int& n_out_len) 
{	 
	int n_len=VL*2;
	int n_text_len=VL*2-4;

	int quotient, remainder, i; 
	quotient =  (n_in_len ) / (n_text_len); 
	remainder = (n_in_len ) % (n_text_len); 
	quotient = remainder?++quotient:quotient; 


	n_out_len=quotient*n_len;

	char *out_buf=new char[n_out_len];

	memset(out_buf,0x0,n_out_len);

	MessageBuf message_buf;
	for(i = 0;i < quotient;i++) 
	{	//把消息包分成若干块 

		message_buf.nil[0]=0;         ///可以保证 小于M. 如果是解密就没有关系

		memset(message_buf.text,0x0,n_text_len);

		memcpy(message_buf.text,in_buf+i*n_text_len,n_text_len);

		//用公钥加密消息快 
		encrypt((unsigned long *)&message_buf);	

		memcpy(out_buf + i * n_len,&message_buf,n_len);     


	} 

	return out_buf;

} 
void private_key::decrypt(DWORD * dollop)
{
	vlong m, c;
	m.load( dollop, LEVEL/*必须是dollop的长度，否则会出错*/);
	c = decrypt(m);
	c.store( dollop, LEVEL);
}
void private_key::delete_buf(char *p_buf)
{
	if (p_buf)
	{
		delete [] p_buf;
	}
}
char * private_key::decrypt(char *in_buf,int n_in_len,int& n_out_len) 
{ 
	int n_len=VL*2;
	int n_text_len=VL*2-4;

	int quotient, remainder, i; 

	quotient = (n_in_len ) / n_len; 
	remainder = (n_in_len ) % n_len; 
	quotient = remainder?++quotient:quotient; 

	n_out_len=quotient*n_text_len;

	char * out_buf=new char[n_out_len];	

	memset(out_buf,0x0,n_out_len);
	MessageBuf message_buf;
	for(i = 0;i < quotient;i++) 
	{	
		//把消息包分成若干块 
		message_buf.nil[0]=0;

		memset(message_buf.text,0x0,n_text_len);

		memcpy(&message_buf,in_buf+i*n_len,n_len);

		decrypt((unsigned long *)&message_buf); 

		memcpy(out_buf + i * n_text_len,message_buf.text,n_text_len);


	} 
	return out_buf;
} 




}


