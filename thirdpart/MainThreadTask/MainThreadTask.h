#ifndef __MAINTHREADTASK__H__
#define __MAINTHREADTASK__H__

#pragma once
#ifdef MAINTHREADTASK_STATIC_EXPORT
#	define MAINTHREADTASK_API
#else
#	ifdef _MSC_VER
#		ifdef MAINTHREADTASK_DLL_EXPORT
#			define MAINTHREADTASK_API __declspec(dllexport)
#		else
#			define MAINTHREADTASK_API __declspec(dllimport)
#			pragma comment(lib,"MainThreadTask.lib")
#		endif
#	else
#		define MAINTHREADTASK_API
#	endif
#endif



class MAINTHREADTASK_API  MainThreadTask
{
public:
	virtual ~MainThreadTask() {}
	virtual void doTask() = 0;
};

class MAINTHREADTASK_API MainThreadTaskManager
{
	MainThreadTaskManager();
public:
	bool addTask(MainThreadTask * task);
	void tick();
	void flush();
		
public:
	static MainThreadTaskManager& instance();
	
};


#endif //__MAINTHREADTASK__H__