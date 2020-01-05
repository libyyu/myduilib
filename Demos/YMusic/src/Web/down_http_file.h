#pragma once
#include <queue>
#include <vector>
#include <atlstr.h>
#include "Utility/mt_helper.h"


namespace em_utility
{
class down_http_file
{
public:
#ifdef _UNICODE 
	typedef wchar_t              chartype;
	typedef std::wstring         stringtype;
	typedef const chartype* pcchar_type;
#else
	typedef char                 chartype;
	typedef std::string          stringtype;
	typedef const chartype* pcchar_type;
#endif
	enum task_type_e
	{
		TT_NORMAL, //һ����ļ�����
	};

	struct down_task
	{
		stringtype s_url;
		stringtype s_local_file_down;			//����ʱ�ı����ļ���
		stringtype s_local_file_normal;		//���غ�����ı����ļ��������ֵΪ�գ������غ���Ҫ������
		HWND h_notify_wnd;
		UINT n_notify_msg;
		WPARAM n_wparam;
		LPARAM n_lparam;

		task_type_e task_type;
		down_task()
		{
			task_type = TT_NORMAL;
			h_notify_wnd = NULL;
			n_notify_msg = 0;
			n_wparam = 0;
			n_lparam = 0;
		}
	};
	enum down_result_e
	{
		dr_success = 0,
		dr_open_url_failed = 1,
		dr_open_file_failed,
		dr_read_file_failed,
		dr_net_open_failed,
		dr_exit
	};
	static const TCHAR COMMON_HEADER[];
	static const unsigned int DEF_BUF_SIZE = 16384;
public:
	down_http_file();
	~down_http_file();
	bool start_thread();
	void stop_thread();
	void add_task(const down_task& task);
	down_result_e down_file_sync(const stringtype& s_url, const stringtype& s_local_file);
    bool find_task(const down_task& task);
	static down_result_e http_file_down(const stringtype& s_url, const stringtype& s_local_file);
private:
	static DWORD WINAPI thread_proc(void* p_param);
	void _thread_proc();
	static DWORD WINAPI httpfileload_proc(void* p_param);
	void _httpfileload_proc(down_task* obj);

	typedef std::deque<down_task> task_container;
	task_container m_down_list;
	HANDLE mh_thread;
	signal_event m_exit_signal;
	signal_event m_task_signal;
	em_utility::critical_session m_lock;
	bool mb_exit;
};

}
