#ifdef _MSC_VER
#include <Windows.h>
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "openal32.lib")
#endif
#include "playerQueue.h"
#include <cstdio>
#include <deque>
#include <cassert>
#include "al/al.h"
#include "al/alc.h"
#include "al/alext.h"
#include "alhelpers.h"

bool isHasError()
{
	ALenum err = alGetError();
	if(AL_NO_ERROR != err)
	{
		const char* errMsg = alGetString(err);
		printf("err:%s\n",errMsg);
		assert(false);
	}
	return AL_NO_ERROR != err;
}

class PlayerDeviceContext
{
public:
	static PlayerDeviceContext* Instance()
	{
		static PlayerDeviceContext _context;
		return &_context;
	}
public:
	PlayerDeviceContext()
		:pDevice(NULL),
		pContext(NULL),
		bEAX(false)
	{
		Init();
	}

	~PlayerDeviceContext()
	{
		UnInit();
	}


	bool Init()
	{
		if(pContext) return true;
		
	//	alSpeedOfSound(2.0f);
		char *devices;   
		devices = (char *)alcGetString(NULL, ALC_DEVICE_SPECIFIER);   
		while(*devices != NULL)
		{
			printf("devices:%s\n",devices);
			devices += strlen(devices) + 1;  
		}

		pDevice  = alcOpenDevice(alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER));
		assert(pDevice);
		if(pDevice)
		{
			pContext = alcCreateContext(pDevice, NULL);
			alcMakeContextCurrent(pContext);
		}
		bEAX = alIsExtensionPresent("EAX2.0");
		bool isFloat32  = alIsExtensionPresent("AL_EXT_FLOAT32");
		bool isMcFormat = alIsExtensionPresent("AL_EXT_MCFORMATS");
		bool isDouBle   = alIsExtensionPresent("AL_EXT_DOUBLE");
		bool isLoki     = alIsExtensionPresent("AL_LOKI_quadriphonic");
		if(isHasError())
		{
			UnInit();
			return false;
		}
		return true;  //clear all error code
	}

	void UnInit()
	{
		if(pContext)
			alcDestroyContext(pContext);
		if(pDevice)
			alcCloseDevice(pDevice);
		pContext = NULL;
		pDevice  = NULL;
	}
private:
	ALCdevice*    pDevice;
	ALCcontext*   pContext;
	ALboolean     bEAX;
};


#define  theContext    (PlayerDeviceContext::Instance())

/************************************************************************/
/*                                                                      */
/************************************************************************/

typedef struct _playerQueue_data
{
	typedef std::deque<ALuint> FreeBufferT;
	ALuint       uiSource;
	unsigned int uiBufferCnt;
	ALuint*      uiBufferHeader;
	FreeBufferT  uiFreeBuffers;
	double       durationTimes;

	struct _playerQueue_data()
	{
		uiSource       = 0;
		uiBufferCnt    = 0;
		uiBufferHeader = NULL;
		durationTimes  = 0.0;
	}

}PlayerQueueData;

unsigned int xiamiPlayer_format(int channels,int sampleBit)
{
	//	unsigned int uFormat = ((channels > 1) ? (pcm16 ? AL_FORMAT_STEREO16 : AL_FORMAT_STEREO8) : (pcm16 ? AL_FORMAT_MONO16 : AL_FORMAT_MONO8));
	typedef struct
	{
		int           _channels;
		int           _bit;
		unsigned int  _format;
	}FormatMap;

	static FormatMap formatMap[] =
	{
		{1,8,AL_FORMAT_MONO8},
		{1,16,AL_FORMAT_MONO16},
		{1,32,AL_MONO32F_SOFT},
		{2,8,AL_FORMAT_STEREO8},
		{2,16,AL_FORMAT_STEREO16},
		{2,32,AL_FORMAT_STEREO_FLOAT32},
		{4,8,AL_QUAD8_SOFT},
		{4,16,AL_QUAD16_SOFT},
		{4,32,AL_QUAD32F_SOFT},
		{6,8,AL_5POINT1_8_SOFT},
		{6,16,AL_5POINT1_16_SOFT},
		{6,32,AL_5POINT1_32F_SOFT},
		{7,8,AL_6POINT1_8_SOFT},
		{7,16,AL_6POINT1_16_SOFT},
		{7,32,AL_6POINT1_32F_SOFT},
		{8,8,AL_7POINT1_8_SOFT},
		{8,16,AL_7POINT1_16_SOFT},
		{8,32,AL_7POINT1_32F_SOFT}
	};

	for (int i=0;i<sizeof(formatMap)/sizeof(formatMap[0]);++i)
	{
		if(channels == formatMap[i]._channels
			&& sampleBit == formatMap[i]._bit)
			return formatMap[i]._format;
	}
	return AL_STEREO16_SOFT;
}
/************************************************************************/
/*                                                                      */
/************************************************************************/

PLAYERQ_DLL_API HPlayerQueue xiamiPlayerQueue_create(unsigned int nMaxBufferCnt)
{
	if(!theContext->Init())
		return NULL;
	PlayerQueueData * thePlayerQueue = new PlayerQueueData();
	if(!thePlayerQueue) return NULL;

	alGenSources(1, &thePlayerQueue->uiSource);
	if(isHasError())
	{
		assert(false);
		delete thePlayerQueue;
		thePlayerQueue = NULL;
	}

	if(0 >= nMaxBufferCnt)
		nMaxBufferCnt = 4;

	thePlayerQueue->uiBufferCnt = nMaxBufferCnt;
	thePlayerQueue->uiBufferHeader = (ALuint*)malloc(sizeof(ALuint)*nMaxBufferCnt);
	memset(thePlayerQueue->uiBufferHeader,0,sizeof(ALuint)*nMaxBufferCnt);
	alGenBuffers(nMaxBufferCnt,thePlayerQueue->uiBufferHeader);
	if(isHasError())
	{
		assert(false);
		free(thePlayerQueue->uiBufferHeader);
		delete thePlayerQueue;
		thePlayerQueue = NULL;
	}

	thePlayerQueue->uiFreeBuffers.clear();
	for (unsigned int i = 0;i<nMaxBufferCnt;++i)
		thePlayerQueue->uiFreeBuffers.push_back(thePlayerQueue->uiBufferHeader[i]);

	{
/*

		{
			ALfloat listenerPos[3],listenerVoc[3],listenerOre[6];
			alGetListenerfv(AL_POSITION, listenerPos);
			alGetListenerfv(AL_VELOCITY, listenerVoc);
			alGetListenerfv(AL_ORIENTATION,listenerOre);

			ALfloat sourcePos[3],sourceVol[3],sourceOre[6];
			alGetSourcefv(thePlayerQueue->uiSource,AL_POSITION,sourcePos);
			alGetSourcefv(thePlayerQueue->uiSource,AL_VELOCITY,sourceVol);
			alGetSourcefv(thePlayerQueue->uiSource,AL_ORIENTATION,sourceOre);

			printf("listenerPos:%f,%f,%f\n\
				   listenerVoc:%f,%f,%f\n\
				   listenerore:%f,%f,%f,%f,%f,%f\n\
				   sourcePos:%f,%f,%f\n\
				   sourceVoc:%f,%f,%f\n\
				   souceOre:%f,%f,%f,%f,%f,%f\n",
				   listenerPos[0],listenerPos[1],listenerPos[2],
				   listenerVoc[0],listenerVoc[1],listenerVoc[2],
				   listenerOre[0],listenerOre[1],listenerOre[2],listenerOre[3],listenerOre[4],listenerOre[5],
				   sourcePos[0],sourcePos[1],sourcePos[2],
				   sourceVol[0],sourceVol[1],sourceVol[2],
				   sourceOre[0],sourceOre[1],sourceOre[2],sourceOre[3],sourceOre[4],sourceOre[5]);
		}

		ALfloat lposition[] = { 0.0f,0.0f,0.0f};
		ALfloat lvelocity[] = { 0.f, 0.f, 0.f };
		ALfloat lorientation[] = { 0.f, 0.f, 0.f, 0.f, 0.f, 0.f };

		alListenerfv(AL_POSITION, lposition);
		alListenerfv(AL_VELOCITY, lvelocity);
		alListenerfv(AL_ORIENTATION, lorientation);

		ALCfloat sourcePos[] = {0.0f,0.0f,0.0f};
		ALCfloat sourceVol[] = {0.0f,0.0f,0.0f};
		alSourcefv(thePlayerQueue->uiSource,AL_POSITION,sourcePos);
		alSourcefv(thePlayerQueue->uiSource,AL_VELOCITY,sourceVol);
		// 	alSourcei(thePlayerQueue->uiSource,AL_LOOPING,AL_TRUE);
		// 	alSourcef(thePlayerQueue->uiSource,AL_GAIN,1);
		// 	alSourcef(thePlayerQueue->uiSource,AL_PITCH,1);
		*/
//  		
// 		alDistanceModel(AL_LINEAR_DISTANCE_CLAMPED);
// 
//  		
		/*
		{
			ALuint source[1] = {thePlayerQueue->uiSource};
			alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED);
			alSourcef(source[0],AL_GAIN, 1.0f);	                   // 設定衰減公式的參數
			alSourcef(source[0], AL_MAX_DISTANCE, 100.0);	    // 設定最大的距離
			alSourcef(source[0], AL_ROLLOFF_FACTOR, 1.0);	    // 衰減因素
			alSourcef(source[0], AL_REFERENCE_DISTANCE, 50.0);	   // 距離單位
			alSourcef(source[0], AL_MAX_GAIN, 1.0);	    // 最大增益
			alSourcef(source[0], AL_MIN_GAIN, 0.0);	    // 最小增益

			ALfloat spos[] = { -50.f, 0.f, 0.f};
			alSourcefv(source[0],AL_POSITION,spos);
		}*/
		{/*
			ALfloat l_pos[] = {0,0,5};
			ALfloat s_pos[] = {0,0,5}, s_vel[] = {0,0,1};
			ALfloat zeros[] = {0,0,0};
			alListenerfv(AL_POSITION, l_pos);
			alListenerfv(AL_VELOCITY, zeros);
			alSourcefv(thePlayerQueue->uiSource, AL_POSITION, s_pos);
			alSourcefv(thePlayerQueue->uiSource, AL_VELOCITY, s_vel);
			*/
		}
	}


	return (HPlayerQueue)thePlayerQueue;
}
PLAYERQ_DLL_API void xiamiPlayerQueue_destroy(HPlayerQueue playerQueue)
{
	PlayerQueueData* thePlayerQueue = static_cast<PlayerQueueData*>(playerQueue);
	if(thePlayerQueue)
	{
		if(PQS_STOPPED != xiamiPlayerQueue_state(playerQueue))
			xiamiPlayerQueue_stop(playerQueue);
		
		alDeleteSources(1,&thePlayerQueue->uiSource);
		alDeleteBuffers(thePlayerQueue->uiBufferCnt,thePlayerQueue->uiBufferHeader);
		free(thePlayerQueue->uiBufferHeader);

		delete playerQueue;
		playerQueue = NULL;
	}
}

PLAYERQ_DLL_API float xiamiPlayerQueue_volume(HPlayerQueue playerQueue)
{
	float fvolume = 1.0f;
	PlayerQueueData* thePlayerQueue = static_cast<PlayerQueueData*>(playerQueue);
	if(thePlayerQueue)
	{
		alGetSourcef(thePlayerQueue->uiSource,AL_GAIN,&fvolume);
	}

	return fvolume;
}
PLAYERQ_DLL_API void  xiamiPlayerQueue_setVolume(HPlayerQueue playerQueue,float f)
{
	PlayerQueueData* thePlayerQueue = static_cast<PlayerQueueData*>(playerQueue);
	if(thePlayerQueue)
	{
		alSourcef(thePlayerQueue->uiSource,AL_GAIN,f);
	}
}
PLAYERQ_DLL_API void  xiamiPlayerQueue_play(HPlayerQueue playerQueue)
{
	PlayerQueueData* thePlayerQueue = static_cast<PlayerQueueData*>(playerQueue);
	if(thePlayerQueue)
	{
		alSourcePlay(thePlayerQueue->uiSource);
	}
}
PLAYERQ_DLL_API void  xiamiPlayerQueue_pause(HPlayerQueue playerQueue)
{
	PlayerQueueData* thePlayerQueue = static_cast<PlayerQueueData*>(playerQueue);
	if(thePlayerQueue)
	{
		alSourcePause(thePlayerQueue->uiSource);
		isHasError();
	}
}
PLAYERQ_DLL_API void  xiamiPlayerQueue_stop(HPlayerQueue playerQueue)
{
	PlayerQueueData* thePlayerQueue = static_cast<PlayerQueueData*>(playerQueue);
	if(thePlayerQueue)
	{
		ALint st;
		alGetSourcei(thePlayerQueue->uiSource,AL_SOURCE_STATE,&st);
		alSourceStop(thePlayerQueue->uiSource);
// 		do 
// 		{
// 			
// 			ALuint uiBuffer = 0;
// 			alSourceUnqueueBuffers(thePlayerQueue->uiSource,1,&uiBuffer);
// 			if(uiBuffer)
// 				thePlayerQueue->uiFreeBuffers.push_back(uiBuffer);
// 			else
// 			{
// 				assert(thePlayerQueue->uiBufferCnt == thePlayerQueue->uiFreeBuffers.size());
// 			}
// 		} while (thePlayerQueue->uiBufferCnt != thePlayerQueue->uiFreeBuffers.size());
		thePlayerQueue->uiFreeBuffers.clear();
		for (unsigned int i = 0;i<thePlayerQueue->uiBufferCnt;++i)
			thePlayerQueue->uiFreeBuffers.push_back(thePlayerQueue->uiBufferHeader[i]);
		alSourceRewind(thePlayerQueue->uiSource);
		alSourcei(thePlayerQueue->uiSource,AL_BUFFER,0);
		thePlayerQueue->durationTimes = 0.0;
		assert(thePlayerQueue->uiBufferCnt == thePlayerQueue->uiFreeBuffers.size());
		assert(0 ==xiamiPlayerQueue_inBufferCnt(playerQueue));
		alGetError();//clear error
	}
}

PLAYERQ_DLL_API double xiamiPlayerQueue_duration(HPlayerQueue playerQueue)
{
	double duration = 0.0;
	PlayerQueueData* thePlayerQueue = static_cast<PlayerQueueData*>(playerQueue);
	if(thePlayerQueue)
	{
		duration = thePlayerQueue->durationTimes;
		xiamiPlayerQueue_idleBufferCnt(playerQueue);

		ALfloat f = 0.0f;
		alGetSourcef(thePlayerQueue->uiSource,AL_SEC_OFFSET,&f);
		duration += f;
		/*
		ALuint uisource = thePlayerQueue->uiSource;
		ALint bufferID = 0;
		alGetSourcei(uisource, AL_BUFFER, &bufferID);
		
		assert(alGetError() != AL_NO_ERROR);
		if(bufferID &&  AL_NO_ERROR !=  alGetError())
		{
			ALint bufferSize, frequency, bitsPerSample, channels;
			alGetBufferi(bufferID, AL_SIZE, &bufferSize);
			alGetBufferi(bufferID, AL_FREQUENCY, &frequency);
			alGetBufferi(bufferID, AL_CHANNELS, &channels);    
			alGetBufferi(bufferID, AL_BITS, &bitsPerSample);    
			ALfloat k = ((double)bufferSize)/(frequency*channels*(bitsPerSample/8));
			printf("f:%f,k:%f\r\n",f,k);
			
		}*/
	}

	return duration;
}

PLAYERQ_DLL_API PlayerQueueState xiamiPlayerQueue_state(HPlayerQueue playerQueue)
{
	PlayerQueueState state = PQS_STOPPED;
	PlayerQueueData* thePlayerQueue = static_cast<PlayerQueueData*>(playerQueue);
	if(thePlayerQueue)
	{
		int n;
		alGetSourcei(thePlayerQueue->uiSource,AL_SOURCE_STATE,&n);
		switch (n)
		{
		case AL_INITIAL:
			state = PQS_INIITAL;
			break;
		case AL_PLAYING:
			state = PQS_PLAYING;
			break;
		case AL_PAUSED:
			state = PQS_PAUSED;
			break;
		case AL_STOPPED:
			state = PQS_STOPPED;
			break;
		}
	}

	return state;
}


PLAYERQ_DLL_API unsigned long xiamiPlayerQueue_inBufferCnt(HPlayerQueue playerQueue)
{
	ALint size = 0;
	PlayerQueueData* thePlayerQueue = static_cast<PlayerQueueData*>(playerQueue);
	if(thePlayerQueue)
	{
		xiamiPlayerQueue_idleBufferCnt(playerQueue);
		alGetSourcei(thePlayerQueue->uiSource, AL_BUFFERS_QUEUED,&size);
	}
	return size;
}

PLAYERQ_DLL_API unsigned long xiamiPlayerQueue_idleBufferCnt(HPlayerQueue playerQueue)
{
	unsigned long nResult = 0;
	PlayerQueueData* thePlayerQueue = static_cast<PlayerQueueData*>(playerQueue);
	if(thePlayerQueue)
	{
		int iBuffersProcessed = 0;
		alGetSourcei(thePlayerQueue->uiSource, AL_BUFFERS_PROCESSED, &iBuffersProcessed);
		while(iBuffersProcessed--)
		{
			ALuint uiBuffer = 0;
			alSourceUnqueueBuffers(thePlayerQueue->uiSource,1,&uiBuffer);
			if(uiBuffer)
			{
				ALint bufferSize, frequency, bitsPerSample, channels;
				alGetBufferi(uiBuffer, AL_SIZE, &bufferSize);
				alGetBufferi(uiBuffer, AL_FREQUENCY, &frequency);
				alGetBufferi(uiBuffer, AL_CHANNELS, &channels);    
				alGetBufferi(uiBuffer, AL_BITS, &bitsPerSample);    

				thePlayerQueue->durationTimes += ((double)bufferSize)/(frequency*channels*(bitsPerSample/8));

				thePlayerQueue->uiFreeBuffers.push_back(uiBuffer);
			}
		}

		nResult = thePlayerQueue->uiFreeBuffers.size();
	}
	return nResult;
}



PLAYERQ_DLL_API bool xiamiPlayerQueue_pushBuffer(
	HPlayerQueue playerQueue,
	const void* pdata,
	unsigned long cnt,
	unsigned long freq,
	int channels,
	int sampleBit)
{
	PlayerQueueData* thePlayerQueue = static_cast<PlayerQueueData*>(playerQueue);
	if(thePlayerQueue->uiSource)
	{
		int n = xiamiPlayerQueue_idleBufferCnt(playerQueue);
		if(0 >= n)
			return false;

		if(!thePlayerQueue->uiFreeBuffers.empty())
		{
			
			unsigned int uiBuffer = thePlayerQueue->uiFreeBuffers.front();
			thePlayerQueue->uiFreeBuffers.pop_front();
			/*
		    unsigned int uFormat = xiamiPlayer_format(channels,sampleBit);
			alBufferData(uiBuffer,uFormat,pdata,cnt,freq);
			*/
			{
				ALenum ntype = AL_UNSIGNED_BYTE_SOFT;
				switch(sampleBit)
				{
				case 8:  ntype = AL_UNSIGNED_BYTE_SOFT; break;
				case 16: ntype = AL_SHORT_SOFT; break;
				case 24: ntype = AL_BYTE3_SOFT; break; //bug 
				case 32: ntype = AL_INT_SOFT; break;
				case 64: ntype = AL_DOUBLE_SOFT; break;
				default: assert(false);break;
				}

				ALenum nChannelsType = AL_MONO_SOFT;
				switch(channels)
				{
				case 1: nChannelsType = AL_MONO_SOFT; break;
				case 2: nChannelsType = AL_STEREO_SOFT; break;
				case 4: nChannelsType = AL_QUAD_SOFT; break;
				case 6: nChannelsType = AL_5POINT1_SOFT; break;
				case 7: nChannelsType = AL_6POINT1_SOFT; break;
				case 8: nChannelsType = AL_7POINT1_SOFT; break;
				default: assert(false); break;
				}

// 				if(24 == sampleBit)
// 				{
// 					alBufferData(uiBuffer,AL_FORMAT_STEREO16,pdata,cnt,freq);
// 					alSourceQueueBuffers(thePlayerQueue->uiSource,1,&uiBuffer);
// 					return !isHasError();
// 				}
				
				static LPALISBUFFERFORMATSUPPORTEDSOFT palIsBufferFormatSupportedSOFT = (LPALISBUFFERFORMATSUPPORTEDSOFT)alGetProcAddress("alIsBufferFormatSupportedSOFT");
				ALenum uFormat = GetFormat(nChannelsType, ntype, palIsBufferFormatSupportedSOFT);
				
			//	wrap_BufferSamples(uiBuffer,freq,AL_STEREO16_SOFT,BytesToFrames(cnt,nChannelsType,ntype),nChannelsType,ntype,pdata);
#ifdef AL_ALEXT_PROTOTYPES
				alBufferSamplesSOFT(uiBuffer,freq,uFormat,BytesToFrames(cnt,nChannelsType,ntype),nChannelsType,ntype,pdata);
#endif
				isHasError();
			}
			alSourceQueueBuffers(thePlayerQueue->uiSource,1,&uiBuffer);
			return !isHasError();
		}
	}

	return false;
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
typedef struct _recorder_data
{
	ALCdevice*        inputDevice;
	unsigned long     samplesNum;
	unsigned long     samplesGroupSize;

	_recorder_data()
	{
		inputDevice        = NULL;
		samplesNum         = 0;
		samplesGroupSize   = 0;
	}

}RecorderData;


PLAYERQ_DLL_API HRecorder xiamiRecorder_create(unsigned long freq,int channels,bool pcm16,unsigned int samples)
{
	if(!theContext->Init())
		return NULL;

	assert(freq);
	if(0 >= freq)
		return NULL;
	assert(samples);
	if(0 >= samples)
		samples = 5;

	RecorderData* pRecorder = new RecorderData();
	if(!pRecorder)
		return NULL;
	unsigned int uFormat     = ((channels > 1)? (pcm16 ? AL_FORMAT_STEREO16 : AL_FORMAT_STEREO8) : (pcm16 ? AL_FORMAT_MONO16 : AL_FORMAT_MONO8));
	pRecorder->samplesNum    = freq*samples;
	pRecorder->inputDevice   = alcCaptureOpenDevice(NULL,freq,uFormat,pRecorder->samplesNum);
	pRecorder->samplesGroupSize = pRecorder->samplesNum*channels*(pcm16 ? 2 : 1);
	
	if(!pRecorder->inputDevice)
	{
		delete pRecorder;
		return NULL;
	}
	return (HRecorder)pRecorder;
}
PLAYERQ_DLL_API void xiamiRecorder_destroy(HRecorder recorder)
{
	RecorderData* pRecorder = static_cast<RecorderData*>(recorder);
	if(pRecorder)
	{
	//	xiamiRecorder_stop(recorder);
		alcCaptureCloseDevice(pRecorder->inputDevice);
		delete pRecorder;
		pRecorder = NULL;
	}
}

PLAYERQ_DLL_API bool xiamiRecorder_start(HRecorder recorder)
{
	RecorderData* pRecorder = static_cast<RecorderData*>(recorder);
	if(!pRecorder)
		return false;

	alcCaptureStart(pRecorder->inputDevice);
	return !isHasError();
}

PLAYERQ_DLL_API bool xiamiRecorder_stop(HRecorder recorder)
{
	RecorderData* pRecorder = static_cast<RecorderData*>(recorder);
	if(!pRecorder)
		return false;

	alcCaptureStop(pRecorder->inputDevice);
	return !isHasError();
}

PLAYERQ_DLL_API unsigned long xiamiRecorder_samplesSize(HRecorder recorder)
{
	RecorderData* pRecorder = static_cast<RecorderData*>(recorder);
	if(!pRecorder)
		return 0;
	return pRecorder->samplesGroupSize;
}

PLAYERQ_DLL_API  unsigned long xiamiRecorder_recording(HRecorder recorder,char* buf,unsigned long cnt)
{
	RecorderData* pRecorder = static_cast<RecorderData*>(recorder);
	if(!pRecorder || !buf || !cnt)
		return 0;

	const unsigned long sampleGrounpCnt = cnt/pRecorder->samplesGroupSize;
	unsigned long bytes = 0;
	for (unsigned long i = 0;i<sampleGrounpCnt;++i)
	{
		ALCint samples = 0;
		do 
		{
			Sleep(20);
			alcGetIntegerv(pRecorder->inputDevice,ALC_CAPTURE_SAMPLES,1,&samples);
			if(isHasError())
				return bytes;
		} while (samples < ALCint(pRecorder->samplesNum));

//		unsigned long pos = pRecorder->inputDevice->ulReadCapturedDataPos; 
		alcCaptureSamples(pRecorder->inputDevice,buf+bytes,pRecorder->samplesNum);
	//	assert(pRecorder->samplesGroupSize == pRecorder->inputDevice->ulReadCapturedDataPos - pos);
		if(isHasError())
			return bytes;

		bytes += pRecorder->samplesGroupSize;
	}
	
	return bytes;
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
void testPlay(const void* pbuf,unsigned long cnt,unsigned long rate,int channels,bool pcm16)
{
	printf( "Playing...\n" );
	HPlayerQueue hpalyer = xiamiPlayerQueue_create(0);
	assert(hpalyer);
	xiamiPlayerQueue_pushBuffer(hpalyer,pbuf,cnt,rate,channels,pcm16);
	xiamiPlayerQueue_play(hpalyer);

	for (;;)
	{
		xiamiPlayerQueue_idleBufferCnt(hpalyer);
		PlayerQueueState st = xiamiPlayerQueue_state(hpalyer);
		if(PQS_PLAYING != st)
		{
			if(xiamiPlayerQueue_inBufferCnt(hpalyer))
				xiamiPlayerQueue_play(hpalyer);
			else
				break;
		}
		Sleep(20);
	}
	printf("playend...\n");
	xiamiPlayerQueue_destroy(hpalyer);
	system("pause");
}

void testRecord()
{
#define  RATE   44100
#define  CHANNELS 2
#define  PCM      true
	HRecorder hrecorder = xiamiRecorder_create(RATE,CHANNELS,PCM,30);
	assert(hrecorder);
	char* pbuf = (char*)malloc(xiamiRecorder_samplesSize(hrecorder));
	assert(pbuf);
	xiamiRecorder_start(hrecorder);
	unsigned long cnt = xiamiRecorder_recording(hrecorder,pbuf,xiamiRecorder_samplesSize(hrecorder));
	xiamiRecorder_stop(hrecorder);
	xiamiRecorder_destroy(hrecorder);

	system("pause");

	testPlay(pbuf,cnt,RATE,CHANNELS,PCM);
	free(pbuf);
}


#define CAPTURE_FORMAT AL_FORMAT_STEREO16
#define FMTSIZE 8
#define CAPTURE_FREQUENCY 44100
#define NUM_SAMPLES (CAPTURE_FREQUENCY * 20)
static ALbyte captureData[NUM_SAMPLES * FMTSIZE];

static void recordSomething( ALCdevice *inputDevice )
{
	ALint samples;
	printf( "recording...\n" );
	alcCaptureStart( inputDevice );
	do {
		Sleep( 10000 );
		alcGetIntegerv( inputDevice, ALC_CAPTURE_SAMPLES, 1, &samples );
	} while( samples < NUM_SAMPLES );
	alcCaptureSamples( inputDevice, captureData, NUM_SAMPLES );
	alcCaptureStop( inputDevice );
}


void testRecordEx()
{
	ALuint source;
	ALuint buffer;
	ALint state;
	ALCdevice *inputDevice;

	theContext->Init();

	/*


	if( !alcIsExtensionPresent( NULL, "ALC_EXT_capture" ) ) {
		fprintf( stderr, "No ALC_EXT_capture support.\n" );
		return;
	}

	printf( "ALC_CAPTURE_DEFAULT_DEVICE_SPECIFIER: %s\n",
		alcGetString( NULL, ALC_CAPTURE_DEFAULT_DEVICE_SPECIFIER ) );

	if( alcIsExtensionPresent( NULL, "ALC_ENUMERATION_EXT" ) == ALC_FALSE ) {
		printf( "No ALC_ENUMERATION_EXT support.\n" );
	} else {
		const char *devList = ( const char * ) alcGetString( NULL,
			ALC_CAPTURE_DEVICE_SPECIFIER );
		printf( "ALC_ENUMERATION_EXT:\n" );
		while( *devList ) {
			printf( "  - %s\n", devList );
			devList += strlen( devList ) + 1;
		}
	}
	*/
	inputDevice =
		alcCaptureOpenDevice( NULL, CAPTURE_FREQUENCY, CAPTURE_FORMAT,
		NUM_SAMPLES );
	if( inputDevice == NULL ) {
		fprintf( stderr, "Couldn't open capture device.\n" );
		return;
	}
	recordSomething( inputDevice );
	alcCaptureCloseDevice( inputDevice );

	system("pause");


	printf("playing...\n");
	alGenSources( 1, &source );
	alGenBuffers( 1, &buffer );

	alBufferData( buffer, CAPTURE_FORMAT, captureData,
		sizeof( captureData ), CAPTURE_FREQUENCY );
//	alSourcei( source, AL_BUFFER, buffer );
	alSourceQueueBuffers(source,1,&buffer);
	alSourcePlay( source );

	do {
		Sleep( 20 );
		alGetSourcei( source, AL_SOURCE_STATE, &state );
	} while( state == AL_PLAYING );

	printf( "Cleaning up...\n" );

	alDeleteSources( 1, &source );
	alDeleteBuffers( 1, &buffer );
	return;

	testPlay(captureData,sizeof(captureData),CAPTURE_FREQUENCY,2,true);
}