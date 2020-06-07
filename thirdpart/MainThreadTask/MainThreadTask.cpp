#include "MainThreadTask.h"
#include <mutex>
#include <deque>

static bool m_empty = true;
static bool m_shutdown = false;
static std::mutex			m_lock;
static std::deque<MainThreadTask*>	m_tasks;

MainThreadTaskManager& MainThreadTaskManager::instance()
{
	static MainThreadTaskManager s_inst;
	return s_inst;
}

MainThreadTaskManager::MainThreadTaskManager()
{}

bool MainThreadTaskManager::addTask(MainThreadTask * task)
{
	std::lock_guard<std::mutex> lk(m_lock);
	if(m_shutdown)
		return false;
	m_tasks.push_back(task);
	m_empty = false;
	return true;
}

void MainThreadTaskManager::tick()
{
	if(m_empty)
		return;
	m_lock.lock();
	MainThreadTask * task = m_tasks.front();
	m_tasks.pop_front();
	if(m_tasks.empty())
		m_empty = true;
	m_lock.unlock();
	task->doTask();
}

void MainThreadTaskManager::flush()
{
	m_lock.lock();
	while(!m_tasks.empty())
	{
		MainThreadTask * task = m_tasks.front();
		m_tasks.pop_front();
		task->doTask();
	}
	m_shutdown = true;
	m_lock.unlock();
}