#pragma once
#include <stddef.h>
typedef unsigned long long uint64;
typedef long long int64;
uint64 network_plugin_clock();
uint64 network_plugin_diffclock(uint64 c1,uint64 c2);

class Flowrate
{
public:
	Flowrate();
	~Flowrate();
	Flowrate(const Flowrate& src);
	Flowrate& operator=(const Flowrate& src);

	void clear();
	uint64 update();

	uint64 get_last_update_clock()const;
	uint64 get_last_read_clock() const;
	uint64 get_last_write_clock() const;
	uint64 get_read_bytes() const;
	uint64 get_write_bytes() const;
	uint64 get_bytes_pre_sec() const;
	uint64 get_read_bytes_pre_sec()const;
	uint64 get_write_bytes_pre_sec()const;
	uint64 get_read_bytes_pre_millisec()const;
	uint64 put_read_bytes(uint64 bytes);
	uint64 pop_read_bytes(uint64 bytes);
	uint64 put_write_bytes(uint64 bytes);
	uint64 pop_write_bytes(uint64 bytes);

	bool is_timeout_with_read(uint64 offtime) const;
	bool is_timeout_with_write(uint64 offtime) const;
	bool is_timeout(uint64 offtime) const;

private:
	volatile uint64              _read_bytes;
	volatile uint64              _write_bytes;
	volatile int64               _read_bytes_pre_sec;
	volatile int64               _write_bytes_pre_sec;

	volatile uint64              _last_read_clock;
	volatile uint64              _last_write_clock;
	volatile uint64              _last_update_clock;
};

class SpeedConfig
{
public:
	SpeedConfig();
	~SpeedConfig();

	uint64 get_max_send_bytes() const;
	void   set_max_send_bytes(uint64);

	uint64 get_max_send_bytes_pre_sec() const;
	void   set_max_send_bytes_pre_sec(uint64);

	uint64 get_max_recv_bytes() const;
	void   set_max_recv_bytes(uint64);

	uint64 get_max_recv_bytes_pre_sec() const;
	void  set_max_recv_bytes_pre_sec(uint64);
private:
	uint64                      _max_send_bytes_pre_sec;
	uint64                      _max_send_bytes;
	uint64                      _max_recv_bytes_pre_sec;
	uint64                      _max_recv_bytes;
};

class Tag
{
public:
	Tag();
	~Tag();

	size_t getTag() const;
	void   setTag(size_t);
private:
	size_t   _tag;
};
