#ifdef _WIN32
#	include <winsock2.h>
#	include <windows.h>
#endif
#include "LocalPlayer.h"
#include "utility/log.h"

CLocalPlayer::CLocalPlayer()
{

}

CLocalPlayer::~CLocalPlayer()
{

}
unsigned long CLocalPlayer::GetDownloadBytes() const
{
	return GetFileBytes();
}

void CLocalPlayer::OnPlay(
	const std::string& wsFile,
	const std::string& wsUrl,
	int nFileType,
	int nBegin,
	int nEnd)
{
	sLog("CLocalPlayer::OnPlayBegin<%s>",wsFile.c_str());
	const bool isOnline = false;
	decoder_plugin*  pDecoderPlugin = NULL;
	decoder_handle   hDecoder = this->OpenDecoder(wsFile,isOnline,&pDecoderPlugin,nFileType,nBegin,nEnd);
	assert(pDecoderPlugin);
	assert(hDecoder);

	if(!hDecoder || !pDecoderPlugin)
	{
		this->OnIPlayerCallBack(XPET_ERROR,(void*)wsFile.c_str(),NULL);
		return;
	}
	std::shared_ptr<void> spScopedDecoder(hDecoder,pDecoderPlugin->close);
	
	this->DefaultPlayPro(wsFile,pDecoderPlugin,spScopedDecoder,nBegin,nEnd);
	sLog("CLocalPlayer::OnPlayEnd<%s>",wsFile.c_str());
}