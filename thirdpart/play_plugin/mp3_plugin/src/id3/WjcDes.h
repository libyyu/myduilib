#pragma once

enum    {ENCRYPT,DECRYPT};
typedef bool (*PSubKey)[16][48];

bool Des_Go(char *Out,char *In,long datalen,const char *Key,int keylen,int &out_length,bool Type = ENCRYPT);

//////////////////////////////////////////////////////////////////////////

static void DES(char Out[8], char In[8], const PSubKey pSubKey, bool Type,char *tmp_data);//标准DES加/解密
static bool SetKey(const char* Key, int len,char *tmp_data,PSubKey PS0,PSubKey PS1);// 设置密钥
static void SetSubKey(PSubKey pSubKey, const char Key[8],char *tmp_data);// 设置子密钥
static void F_func(bool In[32], const bool Ki[48],char *tmp_data);// f 函数
static void S_func(bool Out[32], const bool In[48]);// S 盒代替
static void Transform(bool *Out, bool *In, const char *Table, int len,char *tmp_data);// 变换
static void Xor(bool *InA, const bool *InB, int len);// 异或
static void RotateL(bool *In, int len, int loop,char *tmp_data);// 循环左移
static void ByteToBit(bool *Out, const char *In, int bits);// 字节组转换成位组
static void BitToByte(char *Out, const bool *In, int bits);// 位组转换成字节组