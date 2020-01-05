#ifdef _MSC_VER
#ifndef NETWORK_DLL_EXPORT
#define NETWORK_DLL_EXPORT
#endif
#endif
#include "NetWork.h"
#include "./src/NetService.h"
#include "./src/Accepter.h"
#include "./src/SendReciver.h"
#include "./src/Connecter.h"

namespace network
{
	static bool init(unsigned int ioSrvCnt,unsigned int threadCntPreIoSrv)
	{
		return NetService::Init(ioSrvCnt,threadCntPreIoSrv);
	}
	static void asynrun()
	{
		theSrv->run(true);
	}
	static void run()
	{
		theSrv->run();
	}
	static void stop()
	{
		theSrv->stop();
	}

	static void setNetSendSpeed(unsigned int sendBytesPreSec)
	{
		spSendReciverT theSender(theSrv->getSendReciver());
		assert(theSender);
		if(theSender)
			theSender->set_max_send_bytes_pre_sec(sendBytesPreSec);
	}

	static void setNetRecvSpeed(unsigned int recvBytesPreSec)
	{
		spSendReciverT theReciver(theSrv->getSendReciver());
		assert(theReciver);
		if(theReciver)
			theReciver->set_max_recv_bytes_pre_sec(recvBytesPreSec);
	}
	static unsigned int getNetSendSpeed()
	{
		spSendReciverT theSender(theSrv->getSendReciver());
		assert(theSender);
		if(theSender)
			return (unsigned int)theSender->get_write_bytes_pre_sec();
		return 0;
	}

	static unsigned int getNetRecvSpeed()
	{
		spSendReciverT theReciver(theSrv->getSendReciver());
		assert(theReciver);
		if(theReciver)
			return (unsigned int)theReciver->get_read_bytes_pre_sec();
		return 0;
	}

	static hListener createListener(IListener* del,unsigned int clinetTmieoutMillsec,unsigned int nMaxLinkCnt)
	{
		assert(del);
		if(del)
		{
			spAccepterT ep(new Accepter(theSrv,(AccepterDelegate*)del,clinetTmieoutMillsec,nMaxLinkCnt));
			if(ep)
			{
				theSrv->addAccepter(ep);
				return ep.get();
			}
		}

		return NULL;
	}
	static bool listen(hListener h,unsigned int uport,const char* ip,bool breuseaddr)
	{
		assert(h);
		Accepter* ep = (Accepter*)h;
		if(ep)
		{
			return ep->listen(uport,ip,breuseaddr);
		}
		return false;
	}
	static void closeListener(hListener h)
	{
		assert(h);
		Accepter* ep = (Accepter*)h;
		if(ep)
			ep->close();
	}
	static bool asynsendallclient(hListener h,const void* pdata,unsigned int bytes)
	{
		assert(h);
		Accepter* ep = (Accepter*)h;
		if(ep)
			return ep->asynAllClient(pdata,bytes);
		return false;
	}
	static void destroyListener(hListener h)
	{
		assert(h);
		try
		{
			Accepter* ep = (Accepter*)h;
			if(ep)
			{
				ep->close();
				theSrv->removeAccepter(ep->shared_from_this());
			}
		}
		catch (...)
		{
		}
	}
	static hLinker createLinker(ILinker* del)
	{
		spClientT pclient(new clientT(theSrv,(SockDelegate*)del));
		theSrv->addClient(pclient);
		return pclient.get();
	}
	static bool closeLinker(hLinker h)
	{
		assert(h);
		try
		{
			clientT* pClient = (clientT*)h;
			if(pClient)
			{
				if(!pClient->is_open())
					return false;
				errcodeT ec;
				pClient->close(ec);
				return !ec;
			}
		}
		catch (...)
		{
		}
		return false;
	}
	static void destroyLinker(hLinker h)
	{
		assert(h);
		try
		{
			clientT* pclient = (clientT*)h;
			if(pclient)
			{
				theSrv->removeClient(pclient->shared_from_this());
			}
		}
		catch(...)
		{

		}
	}

	static bool connect(hLinker h,const char* ip,const char* sport,unsigned int timeoutsec /* = 10 */)
	{
		assert(h);
		clientT* pClient = (clientT*)h;
		if(pClient)
		{
			return Connecter::connect(pClient->shared_from_this(),ip,sport,timeoutsec);
		}
		return false;
	}
	static bool asynconnect(hLinker h,const char* ip,const char* sport,unsigned int timeoutsec)
	{
		assert(h);
		clientT* pClient = (clientT*)h;
		if(pClient)
		{
			return Connecter::asynConnect(pClient->shared_from_this(),ip,sport,timeoutsec);
		}
		return false;
	}

	static unsigned int send(hLinker h,const void* pbuffer,unsigned int bytes)
	{
		assert(h);
		assert(pbuffer);
		assert(bytes);
		clientT* pClient =(clientT*)h;
		if(pClient && pClient->is_open())
		{
			return pClient->send(boost::asio::buffer(pbuffer,bytes));
		}

		return 0;
	}
	static bool asynsend(hLinker h,const void* pbuffer,unsigned int bytes)
	{
		assert(h);
		assert(pbuffer);
		assert(bytes);
		clientT* pClient = (clientT*)h;
		if(pClient && pClient->is_open())
		{
			spSendReciverT sender(pClient->getNetSrv()->getSendReciver());
			assert(sender);
			if(sender)
				return sender->asynSend(pClient->shared_from_this(),pbuffer,bytes);
		}
		return false;
	}

	static unsigned int recv(hLinker h,void* pbuffer,unsigned int bytes)
	{
		assert(h);
		assert(pbuffer);
		assert(bytes);

		clientT* pClient = (clientT*)h;
		if(pClient)
		{
			errcodeT ec;
			return pClient->receive(boost::asio::buffer(pbuffer,bytes),0,ec);
		}
		return 0;
	}
	static bool isOpen(hLinker h)
	{
		assert(h);
		clientT* pClient = (clientT*)h;
		if(pClient)
			return pClient->is_open();
		return false;
	}
	static bool setRecvbufSize(hLinker h,int nsize)
	{
		assert(h);
		clientT* pClient = (clientT*)h;
		if(pClient)
		{
			boost::asio::socket_base::receive_buffer_size option(nsize);
			errcodeT ec;
			pClient->set_option(option,ec);
// 			if(!ec)
// 			{
// 				boost::asio::socket_base::receive_low_watermark op(nsize);
// 				pClient->set_option(op,ec);
// 			}
			return !ec;
		}
		return false;
	}
	static bool setSendbufSize(hLinker h,int nsize)
	{
		assert(h);
		clientT* pClient = (clientT*)h;
		if(pClient)
		{
			boost::asio::socket_base::send_buffer_size option(nsize);
			errcodeT ec;
			pClient->set_option(option,ec);
// 			if(!ec)
// 			{
// 				boost::asio::socket_base::send_low_watermark op(nsize);
// 				pClient->set_option(op,ec);
// 			}
			return !ec;
		}
		return false;
	}
	static bool waitReadable(hLinker h,unsigned int ntimeout)
	{
		assert(h);
		clientT* pCient = (clientT*)h;
		if(pCient)
		{
			return SendReciver::waitReadable(pCient->shared_from_this(),ntimeout,NULL);
		}
		return false;
	}
	static bool waitWriteable(hLinker h,unsigned int ntimeout)
	{
		assert(h);
		clientT* pCient = (clientT*)h;
		if(pCient)
		{
			return SendReciver::waitWriteable(pCient->shared_from_this(),ntimeout,NULL);
		}
		return false;
	}

	static void setTag(hLinker h,size_t t)
	{
		assert(h);
		clientT* pClinet = (clientT*)h;
		if(pClinet)
			pClinet->setTag(t);
	}
	static size_t getTag(hLinker h)
	{
		assert(h);
		clientT* pClient = (clientT*)h;
		if(pClient)
			return pClient->getTag();
		return 0;
	}

	static void setTimeout(hLinker h,unsigned int millsec)
	{
		assert(h);
		clientT* pClient = (clientT*)h;
		if(pClient)
			pClient->setTimeOut(millsec);
	}

	static bool isTimeout(hLinker h)
	{
		assert(h);
		clientT* pClient = (clientT*)h;
		if(pClient)
			return pClient->isTimeOut();
		return false;
	}

	static void setSendSpeed(hLinker h,unsigned int bytesPreSec)
	{
		assert(h);
		clientT* pClient = (clientT*)h;
		if(pClient)
			pClient->set_max_send_bytes_pre_sec(bytesPreSec);
	}

	static void setRecvSpeed(hLinker h,unsigned int byetsPerSec)
	{
		assert(h);
		clientT* pClient = (clientT*)h;
		if(pClient)
			pClient->set_max_recv_bytes_pre_sec(byetsPerSec);
	}

	static unsigned int getSendSpeed(hLinker h)
	{
		assert(h);
		clientT* pClient = (clientT*)h;
		if(pClient)
			return (unsigned int)pClient->get_write_bytes_pre_sec();
		return 0;
	}

	static unsigned int getRecvSpeed(hLinker h)
	{
		assert(h);
		clientT* pClient = (clientT*)h;
		if(pClient)
			return (unsigned int)pClient->get_read_bytes_pre_sec();
		return 0;
	}
}


#ifdef   __cplusplus
extern "C" {
#endif

	NETWORK_API plugin_network_handler* get_plugin_network()
	{
		static plugin_network_handler* h = NULL;
		if(!h)
		{
			using namespace network;
			h                = new plugin_network_handler;
			h->nsize         = sizeof(plugin_network_handler);
			h->nver          = 1;
			h->init          = init;
			h->asynrun       = asynrun;
			h->run           = run;
			h->stop          = stop;
			h->setNetSendSpeed = setNetSendSpeed;
			h->setNetRecvSpeed = setNetRecvSpeed;
			h->getNetSendSpeed = getNetSendSpeed;
			h->getNetRecvSpeed = getNetRecvSpeed;

			h->createListener  = createListener;
			h->listen          = listen;
			h->closeListener   = closeListener;
			h->asynsendall     = asynsendallclient;
			h->destroyListener = destroyListener;

			h->createLinker    = createLinker;
			h->destroyLinker   = destroyLinker;
			h->closeLinker     = closeLinker;

			h->connect         = connect;
			h->asynconnect     = asynconnect;
			h->send            = send;
			h->asynsend        = asynsend;
			h->recv            = recv;
			h->setsendbufsize  = setSendbufSize;
			h->setrecvbufsize  = setRecvbufSize;

			h->isOpen          = isOpen;
			h->waitReadable    = waitReadable;
			h->waitWriteable   = waitWriteable;

			h->tag             = getTag;
			h->setTag          = setTag;

			h->setTimeout      = setTimeout;
			h->isTmimeout      = isTimeout;

			h->setSendSpeed    = setSendSpeed;
			h->setRecvSpeed    = setRecvSpeed;
			h->getSendSpeed    = getSendSpeed;
			h->getRecvSpeed    = getRecvSpeed;
		}

		return h;
	}
#ifdef   __cplusplus
}
#endif
