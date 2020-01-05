#ifndef __FLIB_PROCESS_H__
#define __FLIB_PROCESS_H__
#pragma once
#include "FType.h"
#include <iostream>
#include <sstream> 
#include <functional>
#include <mutex>
#include <thread>
#include <memory>
#include <stdexcept>
#if FLIB_COMPILER_WINDOWS
#define NOEXCEPT noexcept
#else
#include <pthread.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#define NOEXCEPT
#endif

_FStdBegin

class F_DLL_API FProcess
{
public:
#if FLIB_COMPILER_WINDOWS
    typedef unsigned long id_type; //Process id type
    typedef void  *fd_type;         //File descriptor type
#else
    typedef pid_t id_type;
    typedef int fd_type;
#endif
private:
    class Data 
    {
    public:
		Data();
        id_type id;
#if FLIB_COMPILER_WINDOWS
        void *handle;
#endif
    };
public:
    ///Note on Windows: it seems not possible to specify which pipes to redirect.
    ///Thus, at the moment, if read_stdout==nullptr, read_stderr==nullptr and open_stdin==false,
    ///the stdout, stderr and stdin are sent to the parent process instead.
	FProcess(const Fstring& command, const Fstring& path = Fstring(),
		std::function<void(const char* bytes, size_t n)> read_stdout_ = nullptr,
		std::function<void(const char* bytes, size_t n)> read_stderr_ = nullptr,
		bool open_stdin_ = false,
		size_t buffer_size_ = 131072) NOEXCEPT;

#if !FLIB_COMPILER_WINDOWS
    /// Supported on Unix-like systems only.
	FProcess(std::function<void()> function,
		std::function<void(const char* bytes, size_t n)> read_stdout_ = nullptr,
		std::function<void(const char* bytes, size_t n)> read_stderr_ = nullptr,
		bool open_stdin_ = false,
		size_t buffer_size_ = 131072) NOEXCEPT;
#endif
	~FProcess() NOEXCEPT;
    
    ///Get the process id of the started process.
	id_type get_id() const NOEXCEPT;
    ///Wait until process is finished, and return exit status.
	int get_exit_status();
    ///If process is finished, returns true and sets the exit status. Returns false otherwise.
	bool try_get_exit_status(int& exit_status);
    ///Write to stdin.
	bool write(const char* bytes, size_t n);
    ///Write to stdin. Convenience function using write(const char *, size_t).
	bool write(const std::string& data);
    ///Close stdin. If the process takes parameters from stdin, use this to notify that all parameters have been sent.
	void close_stdin();
    
    ///Kill the process. force=true is only supported on Unix-like systems.
	void kill(bool force = false) NOEXCEPT;
    ///Kill a given process id. Use kill(bool force) instead if possible. force=true is only supported on Unix-like systems.
	static void kill(id_type id, bool force = false) NOEXCEPT;
  
private:
    Data data;
    bool closed;
    std::mutex close_mutex;
    std::function<void(const char* bytes, size_t n)> read_stdout;
    std::function<void(const char* bytes, size_t n)> read_stderr;
    std::thread stdout_thread, stderr_thread;
    bool open_stdin;
    std::mutex stdin_mutex;
    size_t buffer_size;
    
    std::unique_ptr<fd_type> stdout_fd, stderr_fd, stdin_fd;
	std::mutex stdoutlock, stderrlock;
#if FLIB_COMPILER_WINDOWS
    std::mutex create_process_mutex;
#endif//
	id_type open(const Fstring& command, const Fstring& path);
#if !FLIB_COMPILER_WINDOWS
	id_type open(std::function<void()> function) NOEXCEPT;
#endif
	void async_read() NOEXCEPT;
	void close_fds() NOEXCEPT;
};

_FStdEnd

#endif//__FLIB_PROCESS_H__
