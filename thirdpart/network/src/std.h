#pragma once
#include <stdlib.h>
#include <iostream>
#include <assert.h>
#include <vector>
#include <set>
#include <list>
#include <deque>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp> 
#include <boost/thread.hpp>
#include <boost/filesystem.hpp>
#include <boost/asio.hpp>
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>
#include "templateTool/templateTool.h"
#include "flib.h"

typedef boost::asio::io_service           ioSrvT;
typedef boost::shared_ptr<ioSrvT>         spioSrvT;
typedef ioSrvT::work                      workT;
typedef boost::shared_ptr<workT>          spworkT;
#if BOOST_VERSION >= 107000
typedef boost::asio::io_context::strand   strandT;
#else
typedef boost::asio::strand               strandT;
#endif
typedef boost::shared_ptr<strandT>        spstrandT;
typedef boost::system::error_code         errcodeT;
typedef boost::asio::deadline_timer       timerT;
typedef boost::shared_ptr<timerT>         sptimerT;
typedef boost::asio::ip::tcp::resolver    resolverT;
typedef boost::shared_ptr<resolverT>      spresolverT;
typedef boost::asio::ip::tcp::endpoint    endpointT;
typedef boost::shared_ptr<endpointT>      spendpointT;
typedef boost::thread                     threadT;
typedef boost::shared_ptr<threadT>        spthreadT;
typedef boost::mutex                      lockT;
typedef boost::mutex::scoped_lock         lockerT;

typedef boost::shared_ptr<_FStd(FBuffer)>        spBufferT;
typedef enum {BUFFER_NORMAL,BUFFER_SHARED} emBufferT;
typedef std::deque<std::pair<emBufferT,spBufferT> > spBufferListT;
typedef spBufferListT::iterator spBufferListItetT;
// Remove pointless warning messages
#ifdef _MSC_VER
#pragma warning (disable : 4511) // copy operator could not be generated
#pragma warning (disable : 4512) // assignment operator could not be generated
#pragma warning (disable : 4702) // unreachable code (bugs in Microsoft's STL)
#pragma warning (disable : 4786) // identifier was truncated
#pragma warning (disable : 4996) // function or variable may be unsafe (deprecated)
#pragma warning (disable : 4355) //add by rz.li


#ifndef _WIN32_WINNT
#define _WIN32_WINNT  _WIN32_WINNT_WINXP//0x0501
#endif

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS // eliminate deprecation warnings for VS2005
#endif
#endif // _MSC_VER
#ifdef __BORLANDC__
#pragma option -w-8027		   // function not expanded inline
#endif
