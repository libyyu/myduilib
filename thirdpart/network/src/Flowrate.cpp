#include "Flowrate.h"
#include <assert.h>
#include <algorithm>
#include <math.h>
#include <stdlib.h>
#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#else
#include <unistd.h>
#include <sys/time.h>
#endif


uint64 network_plugin_clock()
{
#ifdef _WIN32
	return GetTickCount();
#else
	struct timeval  tm;
	gettimeofday(&tm,NULL);
	return abs((tm.tv_sec % 10000000)*1000 + tm.tv_usec/1000);
#endif
}

uint64 network_plugin_diffclock(uint64 c1,uint64 c2)
{
#ifdef _WIN32
	return _abs64(c1-c2);
#else
	return fabs(c1-c2);
#endif
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
Flowrate::Flowrate()
	:_read_bytes(0),
	_write_bytes(0),
	_read_bytes_pre_sec(0),
	_write_bytes_pre_sec(0),
	_last_read_clock(0),
	_last_write_clock(0),
	_last_update_clock(0)
{

}
Flowrate::~Flowrate()
{

}
Flowrate::Flowrate(const Flowrate& src)
{
	this->_read_bytes          = src._read_bytes;
	this->_write_bytes         = src._write_bytes;
	this->_read_bytes_pre_sec  = src._read_bytes_pre_sec;
	this->_write_bytes_pre_sec = src._write_bytes_pre_sec;
	this->_last_read_clock     = src._last_read_clock;
	this->_last_write_clock    = src._last_write_clock;
	this->_last_update_clock   = src._last_update_clock;
}
Flowrate& Flowrate::operator=(const Flowrate& src)
{
	this->_read_bytes          = src._read_bytes;
	this->_write_bytes         = src._write_bytes;
	this->_read_bytes_pre_sec  = src._read_bytes_pre_sec;
	this->_write_bytes_pre_sec = src._write_bytes_pre_sec;
	this->_last_read_clock     = src._last_read_clock;
	this->_last_write_clock    = src._last_write_clock;
	this->_last_update_clock   = src._last_update_clock;
	return *this;
}

void Flowrate::clear()
{
	this->_read_bytes          = 0;
	this->_write_bytes         = 0;
	this->_read_bytes_pre_sec  = 0;
	this->_write_bytes_pre_sec = 0;
	this->_last_read_clock     = 0;
	this->_last_write_clock    = 0;
	this->_last_update_clock   = 0;
}
uint64 Flowrate::update()
{
	uint64 nresult = _read_bytes_pre_sec + _write_bytes_pre_sec;
	_read_bytes_pre_sec = 0;
	_write_bytes_pre_sec = 0;

	_last_update_clock = network_plugin_clock();
	return nresult;
}

uint64 Flowrate::get_last_update_clock()const{return _last_update_clock;}
uint64 Flowrate::get_last_read_clock() const{return _last_read_clock;}
uint64 Flowrate::get_last_write_clock() const{return _last_write_clock;}
uint64 Flowrate::get_read_bytes() const {return _read_bytes;}
uint64 Flowrate::get_write_bytes() const {return _write_bytes;}
uint64 Flowrate::get_bytes_pre_sec() const 
{
	int64 bytes = _read_bytes_pre_sec + _write_bytes_pre_sec;
	return (bytes > 0 ? bytes : 0);
	uint64 diff_time = network_plugin_diffclock(network_plugin_clock(),_last_update_clock);
	return (diff_time > 0) ? ((_read_bytes_pre_sec + _write_bytes_pre_sec)/diff_time)*(1000) : 0;
}
uint64 Flowrate::get_read_bytes_pre_sec()const 
{
	return (_read_bytes_pre_sec > 0 ? _read_bytes_pre_sec : 0);
	uint64 diff_time = network_plugin_diffclock(network_plugin_clock(),_last_update_clock);
	return (diff_time > 0) ? (_read_bytes_pre_sec/diff_time)*(1000) : 0;
}
uint64 Flowrate::get_write_bytes_pre_sec()const
{
	return (_write_bytes_pre_sec > 0 ? _write_bytes_pre_sec : 0);
	uint64 diff_time = network_plugin_diffclock(network_plugin_clock(),_last_update_clock);
	return (diff_time > 0) ? (_write_bytes_pre_sec/diff_time)*(1000) : 0;
}
uint64 Flowrate::get_read_bytes_pre_millisec()const
{
	uint64 diff_time = network_plugin_diffclock(network_plugin_clock(),_last_update_clock);
	return (diff_time > 0) ? (_read_bytes_pre_sec/diff_time) : 0;
}

uint64 Flowrate::put_read_bytes(uint64 bytes)
{
	uint64 oldReadBytes = _read_bytes;
	_read_bytes += bytes;
	_read_bytes_pre_sec += bytes;
	
	_last_read_clock = network_plugin_clock();

	return oldReadBytes;
}
uint64 Flowrate::pop_read_bytes(uint64 bytes)
{
	assert(_read_bytes >= bytes);
	uint64 oldReadBytes = _read_bytes;
	_read_bytes -= bytes;
	return oldReadBytes;
}

uint64 Flowrate::put_write_bytes(uint64 bytes)
{
	uint64 oldWriteBytes = _write_bytes;
	_write_bytes += bytes;
	_write_bytes_pre_sec += bytes;

	_last_write_clock = network_plugin_clock();

	return oldWriteBytes;
}
uint64 Flowrate::pop_write_bytes(uint64 bytes)
{
	assert(_write_bytes >= bytes);
	uint64 oldWriteBytes = _write_bytes;
	_write_bytes -= bytes;
	return oldWriteBytes;
}

bool Flowrate::is_timeout_with_read(uint64 offtime) const
{
	assert(offtime);
	if(0 == offtime || 0 == _last_read_clock)
		return false;
	return network_plugin_diffclock(network_plugin_clock(),_last_read_clock) >= offtime;
}

bool Flowrate::is_timeout_with_write(uint64 offtime) const
{
	assert(offtime);
	if(0 == offtime || 0 == _last_write_clock)
		return false;
	return network_plugin_diffclock(network_plugin_clock(),_last_write_clock) >= offtime;
}

bool Flowrate::is_timeout(uint64 offtime) const
{
	assert(offtime);
	if(0 == offtime)
		return false;
	uint64 lasttime = std::max<volatile uint64>(_last_read_clock,_last_write_clock);
	if(0 == lasttime)
		return false;
	return network_plugin_diffclock(network_plugin_clock(),lasttime) >= offtime;
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
SpeedConfig::SpeedConfig()
	:_max_send_bytes_pre_sec(0),
	_max_send_bytes(0),
	_max_recv_bytes_pre_sec(0),
	_max_recv_bytes(0)
{

}

SpeedConfig::~SpeedConfig()
{

}
uint64 SpeedConfig::get_max_send_bytes() const
{
	return _max_send_bytes;
}
void   SpeedConfig::set_max_send_bytes(uint64 bytes)
{
	_max_send_bytes = bytes;
}

uint64 SpeedConfig::get_max_send_bytes_pre_sec() const
{
	return _max_send_bytes_pre_sec;
}
void   SpeedConfig::set_max_send_bytes_pre_sec(uint64 bytes)
{
	_max_send_bytes_pre_sec = bytes;
}

uint64 SpeedConfig::get_max_recv_bytes() const
{
	return _max_recv_bytes;
}
void   SpeedConfig::set_max_recv_bytes(uint64 bytes)
{
	 _max_recv_bytes = bytes;
}

uint64 SpeedConfig::get_max_recv_bytes_pre_sec() const
{
	return _max_recv_bytes_pre_sec;
}
void SpeedConfig::set_max_recv_bytes_pre_sec(uint64 bytes)
{
	_max_recv_bytes_pre_sec = bytes;
}

/************************************************************************/
/*                                                                      */
/************************************************************************/

Tag::Tag():_tag(0)
{

}
Tag::~Tag()
{

}

size_t Tag::getTag() const
{
	return _tag;
}

void Tag::setTag(size_t t)
{
	_tag = t;
}
