#ifndef APE_IO_H
#define APE_IO_H

namespace em_mp3_tag
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
	static const unsigned int DATA_COPY_BUF = 1024 * 8;			//���ݿ�����������С(8K)
public:
    //construction / destruction
    io_base() { }
    virtual ~io_base() { };

    // open / close
    virtual int open(const char* p_name, bool b_readonly) = 0;			//��һ�����ڵ��ļ�
	virtual int open(const wchar_t * p_name, bool b_readonly) = 0;			//��һ�����ڵ��ļ� �¼�

    virtual int close() = 0;										//�ر��ļ�
    
    // read / write, will change current position
	//return : 0 for success, -1 for failed
    virtual int read(void* p_buffer, unsigned int n_bytes_2_read, unsigned int* p_bytes_read) = 0;					//�ӵ�ǰλ�ö����ݵ�������
    virtual int write(const void* p_buffer, unsigned int n_bytes_2_write, unsigned int* p_bytes_written) = 0;		//�ѻ���������д�뵱ǰλ��
    
    // seek
    virtual int seek(int n_distance, unsigned int n_move_mode) = 0;			//��ǰָ�붨λ
    
    // creation / destruction
	virtual int create(const char* p_name) = 0;			//����һ���ļ�
	virtual int create(const wchar_t* p_name) = 0;			//����һ���ļ�
    virtual int destroy() = 0;										//ɾ��һ���ļ�

    // other functions
    virtual int set_EOF() = 0;

    // attributes
    virtual int get_position() = 0;										//ȡ�õ�ǰλ��
    virtual int get_size() = 0;												//ȡ���ļ���С
    virtual int get_name(char* p_buffer) = 0;						//ȡ���ļ���
	virtual int get_name(wchar_t* p_buffer) = 0;						//ȡ���ļ���

	//helper functions
	//���ļ�ͷ������һ������
	virtual int attach_header(unsigned char* p_buf, unsigned int n_len) = 0;
	//���ļ�β������һ������
	virtual int attach_tail(unsigned char* p_buf, unsigned int n_len) = 0;
	//���ļ�ͷ���س�һ������
	virtual int cut_header(unsigned int n_len) = 0;
	//���ļ�β���س�һ������
	virtual int cut_tail(unsigned int n_len) = 0;
};

}

#endif // #ifndef APE_IO_H
