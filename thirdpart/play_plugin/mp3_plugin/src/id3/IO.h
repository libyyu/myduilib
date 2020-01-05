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

//IO接口
class io_base
{
public:
	static const unsigned int DATA_COPY_BUF = 1024 * 8;			//数据拷贝缓冲区大小(8K)
public:
    //construction / destruction
    io_base() { }
    virtual ~io_base() { };

    // open / close
    virtual int open(const char* p_name, bool b_readonly) = 0;			//打开一个存在的文件
	virtual int open(const wchar_t * p_name, bool b_readonly) = 0;			//打开一个存在的文件 新加

    virtual int close() = 0;										//关闭文件
    
    // read / write, will change current position
	//return : 0 for success, -1 for failed
    virtual int read(void* p_buffer, unsigned int n_bytes_2_read, unsigned int* p_bytes_read) = 0;					//从当前位置读数据到缓冲区
    virtual int write(const void* p_buffer, unsigned int n_bytes_2_write, unsigned int* p_bytes_written) = 0;		//把缓冲区数据写入当前位置
    
    // seek
    virtual int seek(int n_distance, unsigned int n_move_mode) = 0;			//当前指针定位
    
    // creation / destruction
	virtual int create(const char* p_name) = 0;			//创建一个文件
	virtual int create(const wchar_t* p_name) = 0;			//创建一个文件
    virtual int destroy() = 0;										//删除一个文件

    // other functions
    virtual int set_EOF() = 0;

    // attributes
    virtual int get_position() = 0;										//取得当前位置
    virtual int get_size() = 0;												//取得文件大小
    virtual int get_name(char* p_buffer) = 0;						//取得文件名
	virtual int get_name(wchar_t* p_buffer) = 0;						//取得文件名

	//helper functions
	//在文件头部加入一段数据
	virtual int attach_header(unsigned char* p_buf, unsigned int n_len) = 0;
	//在文件尾部加入一段数据
	virtual int attach_tail(unsigned char* p_buf, unsigned int n_len) = 0;
	//在文件头部截除一段数据
	virtual int cut_header(unsigned int n_len) = 0;
	//在文件尾部截除一段数据
	virtual int cut_tail(unsigned int n_len) = 0;
};

}

#endif // #ifndef APE_IO_H
