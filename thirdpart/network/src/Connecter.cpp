#include "Connecter.h"
#include "SendReciver.h"

void onConnectHandler(spstrandT& thestrand,spSockContextT& s,sptimerT timer,resolverT::iterator& it,const errcodeT& ec)
{
	if(ec && (++it) != resolverT::iterator())
	{
		assert(!s->is_open());
		s->async_connect(*it,thestrand->wrap(boost::bind(onConnectHandler,thestrand,s,timer,it,_1)));
		return;
	}

	errcodeT e;
	timer->cancel(e);
	s->onConnected(ec.value());
}
void onTimeoutHandler(spstrandT& thestrand,spSockContextT& s,sptimerT timer,const errcodeT& ec)
{
	if(!ec)
	{
		errcodeT e;
		s->close(e);
	}
}
void onResolveHandler(spSockContextT& s,spresolverT& rlv,const errcodeT &ec, resolverT::iterator it,size_t timeout)
{
	if(!ec)
	{
		spstrandT thestrand(new strandT(s->get_io_service()));
		sptimerT timer(new timerT(s->get_io_service()));
		timer->expires_from_now(boost::posix_time::seconds(timeout));
		timer->async_wait(thestrand->wrap(boost::bind(onTimeoutHandler,thestrand,s,timer,_1)));

		s->async_connect(*it,thestrand->wrap(boost::bind(onConnectHandler,thestrand,s,timer,it,_1)));	
	}
	else
	{
		s->onConnected(ec.value());
	}
}


bool Connecter::connect(const spSockContextT& s,const char* peer,const char* sport,size_t timeout /* = 10 */)
{
	assert(s);
	assert(peer);
	assert(sport);
	assert(!s->is_open());

	if(!s || !peer || !sport || s->is_open())
		return false;

	resolverT rlv(s->get_io_service());
	resolverT::query qry(peer,sport);

	resolverT::iterator it = rlv.resolve(qry);
	resolverT::iterator end;
	
// 	for (;it != end;++it)
// 	{
// 		errcodeT ec;
// 		s->close(ec);
// 		s->open(it->endpoint().protocol(),ec);
// 		bool mode = s->native_non_blocking();
// 		s->native_non_blocking(true,ec); 
// 		s->connect(*it,ec);
// 		fd_set fdWrite;  
// 		FD_ZERO(&fdWrite);  
// 		FD_SET(s->native_handle(), &fdWrite);  
// 		timeval tv = {timeout};  
// 		if (select(0, NULL, &fdWrite, NULL, &tv) <= 0   || !FD_ISSET(s->native_handle(), &fdWrite))  
// 		{  
// 			s->close(ec);
// 			continue;
// 		} 
// 
// 		s->native_non_blocking(mode,ec);
// 		 return true;
// 	}
// 	return false;

	errcodeT ec = boost::asio::error::host_not_found;
	for (;ec && it != end;++it)
	{
		errcodeT e;
		s->close(e);
		s->connect(*it,ec);
	}

	return !ec;
}

bool Connecter::asynConnect(const spSockContextT& s,const char* peer,const char* sport,size_t timeout /* = 10 */)
{
	assert(s);
	assert(peer);
	assert(sport);
	assert(!s->is_open());

	if(!s || !peer || !sport || s->is_open())
		return false;

	resolverT::query query(peer,sport);
	spresolverT rlv(new resolverT(s->get_io_service()));

	rlv->async_resolve(query,boost::bind(onResolveHandler,s,rlv,_1,_2,timeout));
	return true;
}
