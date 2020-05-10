#include "flib.h"
#include <iostream>
#include <Windows.h>

static _FStd(FAutoFile) fGlobalLog("log.txt");

void test_buffer()
{
	F_CONSOLE_TRACE
	F_LOGFILE_TRACE(fGlobalLog)

	_FStd(FBuffer) buffer;
	buffer << 1 << "hello world" << true;
	int v;
	char buf[100] = { 0 };
	bool b;
	buffer >> v >> buf >> b;
	printf("v=%d,buf=%s, b = %d\n", v, buf, b);
}

void test_string()
{
	F_CONSOLE_TRACE
	F_LOGFILE_TRACE(fGlobalLog)

	_FStd(FString) fString;
	fString += _T(" nihao ");
	fString.Trim();
	fString << 456 << _FStd(endl);
	const Fchar* pBuf = fString;
	std::cout << fString << std::endl;
	printf("fString = %s, length=%d,\n", pBuf, fString.GetLength());
}

void thread_foo()
{
	int i = 0;
	while (++i < 10)
		F_CONSOLE(DEBUG) << i << "this is a thread function output threadid:" << _FStd(FGetCurrentThreadId()) << _FStd(endl);
}

void test_thread()
{
	F_CONSOLE_TRACE
	F_LOGFILE_TRACE(fGlobalLog)

	_FStd(FThread) thread(&thread_foo);
	thread.start();
	thread.join();

	printf("--- ThreadGroup ---\r\n");
	_FStd(FThreadGroup) tg;
	_FStd(FThread) t1(&thread_foo);
	tg.addThread(&t1);
	tg.createThread(&thread_foo);
	tg.createThread(&thread_foo);
	tg.startAll();
	tg.joinAll();
}

void test_socket()
{
	F_CONSOLE_TRACE
	F_LOGFILE_TRACE(fGlobalLog)

	_FStd(FSocket) sock;
	_FStd(FSockAddr) addr(80, "libyyu.com");

	sock.Create();
	bool ret = sock.Connect(&addr);
	if (!ret) {
		F_CONSOLE(ERROR) << "Connect Return " << (F_ERRNO) << _FStd(endl);
	}
	else {
		F_CONSOLE(DEBUG) << "Connect Success " << _FStd(endl);
	}
}

void test_base64()
{
	F_CONSOLE_TRACE
	F_LOGFILE_TRACE(fGlobalLog)
	const char* str = "helloworld";
	std::string code = F_BASE64_ENCODE(str);
	F_CONSOLE(DEBUG) << "base64:" << code.c_str() << "," << F_BASE64_DECODE(code.c_str()).c_str() << _FStd(endl);
}

void test_md5()
{
	F_CONSOLE_TRACE
	F_LOGFILE_TRACE(fGlobalLog)

	char buff[200];
	_FStd(FMD5String)("hello world", buff);
	F_CONSOLE(DEBUG) << "md5:" << buff << _FStd(endl);
	F_LOGFILE(DEBUG, fGlobalLog) << "md5:" << buff << _FStd(endl);
	F_LOGFILE(DEBUG, fGlobalLog) << "test md5" << _FStd(endl);
}

void test_file()
{
	F_CONSOLE_TRACE
	F_LOGFILE_TRACE(fGlobalLog)

	_FStd(FFile) file;
	int ret = file.Create("hello.txt");
	if (file)
	{
		int32 i32 = 32;
		int16 i16 = 16;
		int64 i64 = 64;
		file.Write(&i32, sizeof(int32));
		file.Write(&i16, sizeof(int16));
		file.Write(&i64, sizeof(int64));
		size_t len = strlen("Hello World C++");
		file.Write(&len, sizeof(len));
		file.Write("Hello World C++", len);
		file.Close();
	}
	else 
		F_CONSOLE(WARN) << F_FORMAT("file is invalid\n");
	file.Open("hello.txt", true);
	if (file)
	{
		long nSize = file.GetSize();
		char* bytes = new char[nSize + 1];
		long sz = file.GetSize();
		file.ReadAll(bytes);
		bytes[nSize] = 0x0;
		_FStd(FBuffer) br((const uint8*)(char*)bytes, sz);
		int32 i32;
		int16 i16;
		int64 i64;
		char ib[1024] = { 0 };
		br >> i32 >> i16 >> i64 >> ib;
		F_CONSOLE(WARN) << i32 << "," << i16 << "," << i64 << "," << ib << _FStd(endl);
		delete[] bytes;
	}
}

void test_plugin()
{
	F_CONSOLE_TRACE
	F_LOGFILE_TRACE(fGlobalLog)

	_FStd(FPlugin) plugin("C:\\Windows\\System32\\ws2_32.dll");
	typedef void*(*pf_gethostbyname)(const char*);
	pf_gethostbyname pf = plugin.Get<pf_gethostbyname>("gethostbyname");
	if (!pf) {
		F_CONSOLE(ERROR) << "Failed Find Function gethostbyname. error = " << (F_ERRNO) << _FStd(endl);
	}
}

void test_v1(_FStd(FValue)*value)
{
	std::string v("helloA");
	value->set(v.c_str());
}

void test_v2(_FStd(FValue) *value)
{
	enum eT
	{
		A,
		B,
	};
	_FStd(flib_enum_t) e((int)(eT::A), "eT::A");
	value->set(&e);
}

void test_value()
{
	F_CONSOLE_TRACE
	F_LOGFILE_TRACE(fGlobalLog)

	_FStd(FValue) value;
	test_v1(&value);
	std::cout << value.get_string() << std::endl;
	_FStd(FValue)* v = value.clone();
	std::cout << value.get_string() << std::endl;
	std::cout << v->get_string() << std::endl;
	test_v2(v);
	//v->get_string();
	auto p = v->get_enum();
	delete v;
}

int main()
{
	F_CONSOLE_TRACE
	F_LOGFILE_TRACE(fGlobalLog)
	F_LOGFILE(DEBUG, fGlobalLog) << F_FORMAT("ModulePath = %s", _FStd(FGetModulePath())) << _FStd(endl);

	F_SOCKET_STARTUP
	{
		test_buffer();
		test_string();
		test_thread();
		test_socket();
		test_base64();
		test_md5();
		test_file();
		test_plugin();
		test_value();
	}

	char ch;
	while ((ch = getchar()) == 'q')
		break;

    
    F_SOCKET_CLEANUP
    return 0;
}