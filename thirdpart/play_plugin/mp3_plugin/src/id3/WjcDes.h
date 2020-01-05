#pragma once

enum    {ENCRYPT,DECRYPT};
typedef bool (*PSubKey)[16][48];

bool Des_Go(char *Out,char *In,long datalen,const char *Key,int keylen,int &out_length,bool Type = ENCRYPT);

//////////////////////////////////////////////////////////////////////////

static void DES(char Out[8], char In[8], const PSubKey pSubKey, bool Type,char *tmp_data);//��׼DES��/����
static bool SetKey(const char* Key, int len,char *tmp_data,PSubKey PS0,PSubKey PS1);// ������Կ
static void SetSubKey(PSubKey pSubKey, const char Key[8],char *tmp_data);// ��������Կ
static void F_func(bool In[32], const bool Ki[48],char *tmp_data);// f ����
static void S_func(bool Out[32], const bool In[48]);// S �д���
static void Transform(bool *Out, bool *In, const char *Table, int len,char *tmp_data);// �任
static void Xor(bool *InA, const bool *InB, int len);// ���
static void RotateL(bool *In, int len, int loop,char *tmp_data);// ѭ������
static void ByteToBit(bool *Out, const char *In, int bits);// �ֽ���ת����λ��
static void BitToByte(char *Out, const bool *In, int bits);// λ��ת�����ֽ���