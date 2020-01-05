#pragma once
#include <tchar.h>
#include <vector>
#include <string>

namespace em_utility
{
#ifndef FILE_BEGIN
    #define FILE_BEGIN        0
#endif

#ifndef FILE_CURRENT
    #define FILE_CURRENT    1
#endif

#ifndef FILE_END
    #define FILE_END        2
#endif

//IO�ӿ�
class io_base
{
public:
	static const int NORMAL_SUCCESS = 0;
	static const int NORMAL_FAILED = -1;
    io_base() { }
    virtual ~io_base() { };
	virtual bool is_dir_exists(const TCHAR* p_dir) = 0;  //�ж�Ŀ¼�Ƿ����
	virtual int make_dir(const TCHAR* p_dir) = 0; //����һ��Ŀ¼
	virtual bool is_file_exists(const TCHAR* p_name) = 0; //�ļ��Ƿ����
    virtual int open(const TCHAR* p_name, bool b_readonly) = 0;			//��һ�����ڵ��ļ�
    virtual int close() = 0;										//�ر��ļ�
    // read / write, will change current position
	//return : 0 for success, -1 for failed
    virtual int read(void* p_buffer, unsigned int n_bytes_2_read, unsigned int* p_bytes_read) = 0;					//�ӵ�ǰλ�ö����ݵ�������
    virtual int write(const void* p_buffer, unsigned int n_bytes_2_write, unsigned int* p_bytes_written) = 0;		//�ѻ���������д�뵱ǰλ��
    // seek
    virtual int seek(int n_distance, unsigned int n_move_mode) = 0;			//��ǰָ�붨λ
    // creation / destruction
    virtual int create(const TCHAR* p_name) = 0;			//����һ���ļ�
	virtual int destroy() = 0;											//ɾ��һ���ļ�
    virtual int set_EOF() = 0;											//�ضϻ������ļ�
    virtual int get_position() = 0;									//ȡ�õ�ǰλ��
    virtual int get_size() = 0;											//ȡ���ļ���С
    virtual int get_name(TCHAR* p_buffer) = 0;				//ȡ���ļ���
	virtual bool is_open() = 0;										//�Ƿ��
	virtual int flush() = 0;												//����ͬ��
};

//λ����Ϣ
struct pos_info
{
public:
	pos_info(unsigned int n_begin = 0, unsigned int n_len = 0);
	virtual ~pos_info();
	unsigned int get_begin() const;
	unsigned int get_len() const;
	unsigned int get_end() const;
	void set_info(unsigned int n_begin, unsigned int n_len);
	bool valid() const;
	unsigned int size() const;
	bool is_sub_pos(const pos_info& pos) const;
	bool is_equal_pos(const pos_info& pos) const;
private:
	unsigned int mn_begin;		//���ݿ�ʼλ��
	unsigned int mn_len;				//���ݳ���
};

typedef std::vector<pos_info> position_container;
typedef position_container::iterator position_iter;

}
