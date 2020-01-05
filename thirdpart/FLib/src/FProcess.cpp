#define FLIB_DLL
#include "FProcess.h"
#if FLIB_COMPILER_WINDOWS
#include <Windows.h>
#include <TLHelp32.h>
#endif

_FStdBegin

FProcess::Data::Data() :
#if FLIB_COMPILER_WINDOWS
	id(0), handle(NULL)
#else
	id(-1)
#endif
{ }

///Note on Windows: it seems not possible to specify which pipes to redirect.
///Thus, at the moment, if read_stdout==nullptr, read_stderr==nullptr and open_stdin==false,
///the stdout, stderr and stdin are sent to the parent process instead.
FProcess::FProcess(const Fstring& command, const Fstring& path/* = Fstring()*/,
	std::function<void(const char* bytes, size_t n)> read_stdout_ /*= nullptr*/,
	std::function<void(const char* bytes, size_t n)> read_stderr_ /*= nullptr*/,
	bool open_stdin_/* = false*/,
	size_t buffer_size_/* = 131072*/) NOEXCEPT :
	closed(true),
	read_stdout(std::move(read_stdout_)),
	read_stderr(std::move(read_stderr_)),
	open_stdin(open_stdin_),
	buffer_size(buffer_size_)
{
	open(command, path);
	async_read();
}

#if !FLIB_COMPILER_WINDOWS
/// Supported on Unix-like systems only.
FProcess::FProcess(std::function<void()> function,
	std::function<void(const char* bytes, size_t n)> read_stdout_ /*= nullptr*/,
	std::function<void(const char* bytes, size_t n)> read_stderr_ /*= nullptr*/,
	bool open_stdin_/* = false*/,
	size_t buffer_size_/* = 131072*/) NOEXCEPT :
	closed(true),
	read_stdout(std::move(read_stdout_)),
	read_stderr(std::move(read_stderr_)),
	open_stdin(open_stdin_),
	buffer_size(buffer_size_)
{
	open(function);
	async_read();
}
#endif

FProcess::~FProcess() NOEXCEPT
{
	close_fds();
	kill();
}

///Get the process id of the started process.
FProcess::id_type FProcess::get_id() const NOEXCEPT { return data.id; }
///Wait until process is finished, and return exit status.
int FProcess::get_exit_status()
{
#if FLIB_COMPILER_WINDOWS
	if (data.id == 0)
		return -1;

	DWORD exit_status;
	WaitForSingleObject(data.handle, INFINITE);
	if (!GetExitCodeProcess(data.handle, &exit_status))
		exit_status = -1;
	{
		std::lock_guard<std::mutex> lock(close_mutex);
		CloseHandle(data.handle);
		closed = true;
	}
	close_fds();

	return static_cast<int>(exit_status);
#else
	if (data.id <= 0)
		return -1;

	int exit_status;
	waitpid(data.id, &exit_status, 0);
	{
		std::lock_guard<std::mutex> lock(close_mutex);
		closed = true;
	}
	close_fds();

	if (exit_status >= 256)
		exit_status = exit_status >> 8;
	return exit_status;
#endif
}
///If process is finished, returns true and sets the exit status. Returns false otherwise.
bool FProcess::try_get_exit_status(int& exit_status)
{
#if FLIB_COMPILER_WINDOWS
	if (data.id == 0)
		return false;

	DWORD wait_status = WaitForSingleObject(data.handle, 0);

	if (wait_status == WAIT_TIMEOUT)
		return false;

	DWORD exit_status_win;
	if (!GetExitCodeProcess(data.handle, &exit_status_win))
		exit_status_win = -1;
	{
		std::lock_guard<std::mutex> lock(close_mutex);
		CloseHandle(data.handle);
		closed = true;
	}
	close_fds();

	exit_status = static_cast<int>(exit_status_win);
	return true;
#else
	if (data.id <= 0)
		return false;

	id_type p = waitpid(data.id, &exit_status, WNOHANG);
	if (p == 0)
		return false;

	{
		std::lock_guard<std::mutex> lock(close_mutex);
		closed = true;
	}
	close_fds();

	if (exit_status >= 256)
		exit_status = exit_status >> 8;

	return true;
#endif
}

///Write to stdin.
bool FProcess::write(const char* bytes, size_t n)
{
	if (!open_stdin)
		throw std::invalid_argument("Can't write to an unopened stdin pipe. Please set open_stdin=true when constructing the process.");

#if FLIB_COMPILER_WINDOWS
	std::lock_guard<std::mutex> lock(stdin_mutex);
	if (stdin_fd)
	{
		DWORD written;
		BOOL bSuccess = WriteFile(*stdin_fd, bytes, static_cast<DWORD>(n), &written, nullptr);
		if (bSuccess && written != 0)
		{
			return true;
		}
	}
	return false;
#else
	std::lock_guard<std::mutex> lock(stdin_mutex);
	if (stdin_fd)
	{
		if (::write(*stdin_fd, bytes, n) >= 0)
		{
			return true;
		}
	}
	return false;
#endif
}
///Write to stdin. Convenience function using write(const char *, size_t).
bool FProcess::write(const std::string& data) { return write(data.c_str(), data.size()); }

///Close stdin. If the process takes parameters from stdin, use this to notify that all parameters have been sent.
void FProcess::close_stdin()
{
#if FLIB_COMPILER_WINDOWS
	std::lock_guard<std::mutex> lock(stdin_mutex);
	if (stdin_fd)
	{
		if (*stdin_fd != NULL)
			CloseHandle(*stdin_fd);
		stdin_fd.reset();
	}
#else
	std::lock_guard<std::mutex> lock(stdin_mutex);
	if (stdin_fd)
	{
		if (data.id > 0)
			close(*stdin_fd);
		stdin_fd.reset();
	}
#endif
}

///Kill the process. force=true is only supported on Unix-like systems.
void FProcess::kill(bool force/* = false*/) NOEXCEPT
{
#if FLIB_COMPILER_WINDOWS
	std::lock_guard<std::mutex> lock(close_mutex);
	if (data.id > 0 && !closed)
	{
		HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (snapshot)
		{
			PROCESSENTRY32 process;
			ZeroMemory(&process, sizeof(process));
			process.dwSize = sizeof(process);
			if (Process32First(snapshot, &process))
			{
				do
				{
					if (process.th32ParentProcessID == data.id)
					{
						HANDLE process_handle = OpenProcess(PROCESS_TERMINATE, FALSE, process.th32ProcessID);
						if (process_handle)
						{
							TerminateProcess(process_handle, 2);
							CloseHandle(process_handle);
						}
					}
				} while (Process32Next(snapshot, &process));
			}
			CloseHandle(snapshot);
		}
		TerminateProcess(data.handle, 2);
	}
#else
	std::lock_guard<std::mutex> lock(close_mutex);
	if (data.id > 0 && !closed)
	{
		if (force)
			::kill(-data.id, SIGTERM);
		else
			::kill(-data.id, SIGINT);
	}
#endif
}
///Kill a given process id. Use kill(bool force) instead if possible. force=true is only supported on Unix-like systems.
void FProcess::kill(FProcess::id_type id, bool force/* = false*/) NOEXCEPT
{
#if FLIB_COMPILER_WINDOWS
	if (id == 0)
		return;

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (snapshot)
	{
		PROCESSENTRY32 process;
		ZeroMemory(&process, sizeof(process));
		process.dwSize = sizeof(process);
		if (Process32First(snapshot, &process))
		{
			do
			{
				if (process.th32ParentProcessID == id)
				{
					HANDLE process_handle = OpenProcess(PROCESS_TERMINATE, FALSE, process.th32ProcessID);
					if (process_handle)
					{
						TerminateProcess(process_handle, 2);
						CloseHandle(process_handle);
					}
				}
			} while (Process32Next(snapshot, &process));
		}
		CloseHandle(snapshot);
	}
	HANDLE process_handle = OpenProcess(PROCESS_TERMINATE, FALSE, id);
	if (process_handle) TerminateProcess(process_handle, 2);
#else
	if (id <= 0)
		return;

	if (force)
		::kill(-id, SIGTERM);
	else
		::kill(-id, SIGINT);
#endif
}

#if FLIB_COMPILER_WINDOWS
///Simple HANDLE wrapper to close it automatically from the destructor.
class FHandle
{
public:
	FHandle() NOEXCEPT : handle(INVALID_HANDLE_VALUE) { }
	~FHandle() NOEXCEPT {
		close();
	}
	void close() NOEXCEPT {
		if (handle != INVALID_HANDLE_VALUE)
			CloseHandle(handle);
	}
	HANDLE detach() NOEXCEPT {
		HANDLE old_handle = handle;
		handle = INVALID_HANDLE_VALUE;
		return old_handle;
	}
	operator HANDLE() const NOEXCEPT { return handle; }
	HANDLE* operator&() NOEXCEPT { return &handle; }
private:
	HANDLE handle;
};
#endif//FLIB_COMPILER_WINDOWS

FProcess::id_type FProcess::open(const Fstring& command, const Fstring& path)
{
#if FLIB_COMPILER_WINDOWS
	if (open_stdin)
		stdin_fd = std::unique_ptr<fd_type>(new fd_type(NULL));
	if (read_stdout)
		stdout_fd = std::unique_ptr<fd_type>(new fd_type(NULL));
	if (read_stderr)
		stderr_fd = std::unique_ptr<fd_type>(new fd_type(NULL));

	FHandle stdin_rd_p;
	FHandle stdin_wr_p;
	FHandle stdout_rd_p;
	FHandle stdout_wr_p;
	FHandle stderr_rd_p;
	FHandle stderr_wr_p;

	SECURITY_ATTRIBUTES security_attributes;

	security_attributes.nLength = sizeof(SECURITY_ATTRIBUTES);
	security_attributes.bInheritHandle = TRUE;
	security_attributes.lpSecurityDescriptor = nullptr;

	std::lock_guard<std::mutex> lock(create_process_mutex);
	if (stdin_fd)
	{
		if (!CreatePipe(&stdin_rd_p, &stdin_wr_p, &security_attributes, 0) ||
			!SetHandleInformation(stdin_wr_p, HANDLE_FLAG_INHERIT, 0))
			return 0;
	}
	if (stdout_fd)
	{
		if (!CreatePipe(&stdout_rd_p, &stdout_wr_p, &security_attributes, 0) ||
			!SetHandleInformation(stdout_rd_p, HANDLE_FLAG_INHERIT, 0))
			return 0;
	}
	if (stderr_fd)
	{
		if (!CreatePipe(&stderr_rd_p, &stderr_wr_p, &security_attributes, 0) ||
			!SetHandleInformation(stderr_rd_p, HANDLE_FLAG_INHERIT, 0)) {
			return 0;
		}
	}

	PROCESS_INFORMATION process_info;
	STARTUPINFO startup_info;

	ZeroMemory(&process_info, sizeof(PROCESS_INFORMATION));

	ZeroMemory(&startup_info, sizeof(STARTUPINFO));
	startup_info.cb = sizeof(STARTUPINFO);
	startup_info.hStdInput = stdin_rd_p;
	startup_info.hStdOutput = stdout_wr_p;
	startup_info.hStdError = stderr_wr_p;
	if (stdin_fd || stdout_fd || stderr_fd)
		startup_info.dwFlags |= STARTF_USESTDHANDLES;

	Fstring process_command = command;
#ifdef MSYS_PROCESS_USE_SH
	size_t pos = 0;
	while ((pos = process_command.find(_T('\\'), pos)) != Fstring::npos)
	{
		process_command.replace(pos, 1, _T("\\\\\\\\"));
		pos += 4;
	}
	pos = 0;
	while ((pos = process_command.find(_T('\"'), pos)) != Fstring::npos)
	{
		process_command.replace(pos, 1, _T("\\\""));
		pos += 2;
	}
	process_command.insert(0, "sh -c \"");
	process_command += "\"";
#endif//MSYS_PROCESS_USE_SH

	BOOL bSuccess = CreateProcess(nullptr,
		process_command.empty() ? nullptr : &process_command[0],
		nullptr,
		nullptr,
		TRUE,
		0,
		nullptr,
		path.empty() ? nullptr : path.c_str(),
		&startup_info,
		&process_info);

	if (!bSuccess)
		return 0;
	else
		CloseHandle(process_info.hThread);

	if (stdin_fd)* stdin_fd = stdin_wr_p.detach();
	if (stdout_fd)* stdout_fd = stdout_rd_p.detach();
	if (stderr_fd)* stderr_fd = stderr_rd_p.detach();

	closed = false;
	data.id = process_info.dwProcessId;
	data.handle = process_info.hProcess;
	return process_info.dwProcessId;
#else
	return open([&command, &path]
		{
			if (!path.empty())
			{
				auto path_escaped = path;
				size_t pos = 0;
				//Based on https://www.reddit.com/r/cpp/comments/3vpjqg/a_new_platform_independent_process_library_for_c11/cxsxyb7
				while ((pos = path_escaped.find(_T('\''), pos)) != std::string::npos)
				{
					path_escaped.replace(pos, 1, _T("'\\''"));
					pos += 4;
				}
				execl("/bin/sh", "sh", "-c", ("cd '" + path_escaped + "' && " + command).c_str(), NULL);
			}
			else
				execl("/bin/sh", "sh", "-c", command.c_str(), NULL);
		});
#endif
}

#if !FLIB_COMPILER_WINDOWS
FProcess::id_type FProcess::open(std::function<void()> function) NOEXCEPT
{
	if (open_stdin)
		stdin_fd = std::unique_ptr<fd_type>(new fd_type);
	if (read_stdout)
		stdout_fd = std::unique_ptr<fd_type>(new fd_type);
	if (read_stderr)
		stderr_fd = std::unique_ptr<fd_type>(new fd_type);

	int stdin_p[2], stdout_p[2], stderr_p[2];

	if (stdin_fd && pipe(stdin_p) != 0)
		return -1;
	if (stdout_fd && pipe(stdout_p) != 0)
	{
		if (stdin_fd) { close(stdin_p[0]); close(stdin_p[1]); }
		return -1;
	}
	if (stderr_fd && pipe(stderr_p) != 0)
	{
		if (stdin_fd) { close(stdin_p[0]); close(stdin_p[1]); }
		if (stdout_fd) { close(stdout_p[0]); close(stdout_p[1]); }
		return -1;
	}

	id_type pid = fork();

	if (pid < 0)
	{
		if (stdin_fd) { close(stdin_p[0]); close(stdin_p[1]); }
		if (stdout_fd) { close(stdout_p[0]); close(stdout_p[1]); }
		if (stderr_fd) { close(stderr_p[0]); close(stderr_p[1]); }
		return pid;
	}
	else if (pid == 0)
	{
		if (stdin_fd) dup2(stdin_p[0], 0);
		if (stdout_fd) dup2(stdout_p[1], 1);
		if (stderr_fd) dup2(stderr_p[1], 2);
		if (stdin_fd) { close(stdin_p[0]); close(stdin_p[1]); }
		if (stdout_fd) { close(stdout_p[0]); close(stdout_p[1]); }
		if (stderr_fd) { close(stderr_p[0]); close(stderr_p[1]); }

		//Based on http://stackoverflow.com/a/899533/3808293
		int fd_max = static_cast<int>(sysconf(_SC_OPEN_MAX)); // truncation is safe
		for (int fd = 3; fd < fd_max; fd++) close(fd);

		setpgid(0, 0);
		//TODO: See here on how to emulate tty for colors: http://stackoverflow.com/questions/1401002/trick-an-application-into-thinking-its-stdin-is-interactive-not-a-pipe
		//TODO: One solution is: echo "command;exit"|script -q /dev/null

		if (function) function();
		_exit(EXIT_FAILURE);
	}

	if (stdin_fd) close(stdin_p[0]);
	if (stdout_fd) close(stdout_p[1]);
	if (stderr_fd) close(stderr_p[1]);

	if (stdin_fd)* stdin_fd = stdin_p[1];
	if (stdout_fd)* stdout_fd = stdout_p[0];
	if (stderr_fd)* stderr_fd = stderr_p[0];

	closed = false;
	data.id = pid;
	return pid;
}
#endif

void FProcess::async_read() NOEXCEPT
{
#if FLIB_COMPILER_WINDOWS
	if (data.id == 0)
		return;

	if (stdout_fd)
	{
		stdout_thread = std::thread([this]()
			{
				DWORD n;
				std::unique_ptr<char[]> buffer(new char[buffer_size]);
				for (;;)
				{
					std::lock_guard<std::mutex> lock(stdoutlock);
					BOOL bSuccess = ReadFile(*stdout_fd, static_cast<CHAR*>(buffer.get()), static_cast<DWORD>(buffer_size), &n, nullptr);
					if (!bSuccess || n == 0) break;
					read_stdout(buffer.get(), static_cast<size_t>(n));
				}
			});
	}
	if (stderr_fd)
	{
		stderr_thread = std::thread([this]()
			{
				DWORD n;
				std::unique_ptr<char[]> buffer(new char[buffer_size]);
				for (;;)
				{
					std::lock_guard<std::mutex> lock(stderrlock);
					BOOL bSuccess = ReadFile(*stderr_fd, static_cast<CHAR*>(buffer.get()), static_cast<DWORD>(buffer_size), &n, nullptr);
					if (!bSuccess || n == 0) break;
					read_stderr(buffer.get(), static_cast<size_t>(n));
				}
			});
	}
#else
	if (data.id <= 0)
		return;

	if (stdout_fd)
	{
		stdout_thread = std::thread([this]()
			{
				auto buffer = std::unique_ptr<char[]>(new char[buffer_size]);
				ssize_t n;
				while ((n = read(*stdout_fd, buffer.get(), buffer_size)) > 0)
				{
					std::lock_guard<std::mutex> lock(stdoutlock);
					read_stdout(buffer.get(), static_cast<size_t>(n));
				}
			});
	}
	if (stderr_fd)
	{
		stderr_thread = std::thread([this]()
			{
				auto buffer = std::unique_ptr<char[]>(new char[buffer_size]);
				ssize_t n;
				while ((n = read(*stderr_fd, buffer.get(), buffer_size)) > 0)
				{
					std::lock_guard<std::mutex> lock(stderrlock);
					read_stderr(buffer.get(), static_cast<size_t>(n));
				}
			});
	}
#endif
}
void FProcess::close_fds() NOEXCEPT
{
	if (stdout_thread.joinable())
		stdout_thread.join();
	if (stderr_thread.joinable())
		stderr_thread.join();
	if (stdin_fd)
		close_stdin();
#if FLIB_COMPILER_WINDOWS
	if (stdout_fd)
	{
		if (*stdout_fd != NULL) CloseHandle(*stdout_fd);
		stdout_fd.reset();
	}
	if (stderr_fd)
	{
		if (*stderr_fd != NULL) CloseHandle(*stderr_fd);
		stderr_fd.reset();
	}
#else
	if (stdout_fd)
	{
		if (data.id > 0)
			close(*stdout_fd);
		stdout_fd.reset();
	}
	if (stderr_fd)
	{
		if (data.id > 0)
			close(*stderr_fd);
		stderr_fd.reset();
	}
#endif
}

_FStdEnd
