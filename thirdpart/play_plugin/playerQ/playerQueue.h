#ifndef _PLAYERQUEUE_H
#define _PLAYERQUEUE_H
#pragma once
#ifdef _MSC_VER
#ifdef PLAYERQ_DLL_EXPORT
#define PLAYERQ_DLL_API __declspec(dllexport)
#else
#define PLAYERQ_DLL_API __declspec(dllimport)
#pragma comment(lib,"playerQ.lib")
#endif
#else
#define  PLAYERQ_DLL_API
#endif 

#if defined(__cplusplus)
extern "C" {
#endif

typedef enum 
{
	PQS_INIITAL = 0,
	PQS_PLAYING,
	PQS_PAUSED,
	PQS_STOPPED
}PlayerQueueState;

typedef void* HPlayerQueue;


PLAYERQ_DLL_API HPlayerQueue xiamiPlayerQueue_create(unsigned int nMaxBufferCnt);
PLAYERQ_DLL_API void xiamiPlayerQueue_destroy(HPlayerQueue playerQueue);

PLAYERQ_DLL_API float xiamiPlayerQueue_volume(HPlayerQueue playerQueue);
PLAYERQ_DLL_API void  xiamiPlayerQueue_setVolume(HPlayerQueue playerQueue,float f);

PLAYERQ_DLL_API PlayerQueueState xiamiPlayerQueue_state(HPlayerQueue playerQueue);
PLAYERQ_DLL_API void  xiamiPlayerQueue_play(HPlayerQueue playerQueue);
PLAYERQ_DLL_API void  xiamiPlayerQueue_pause(HPlayerQueue playerQueue);
PLAYERQ_DLL_API void  xiamiPlayerQueue_stop(HPlayerQueue playerQueue);

PLAYERQ_DLL_API double xiamiPlayerQueue_duration(HPlayerQueue playerQueue);

PLAYERQ_DLL_API unsigned long xiamiPlayerQueue_inBufferCnt(HPlayerQueue playerQueue);
PLAYERQ_DLL_API unsigned long xiamiPlayerQueue_idleBufferCnt(HPlayerQueue playerQueue);

PLAYERQ_DLL_API bool xiamiPlayerQueue_pushBuffer(
	HPlayerQueue playerQueue,
	const void* pdata,
	unsigned long cnt,
	unsigned long freq,
	int channels,
	int sampleBit);

/************************************************************************/
/*                                                                      */
/************************************************************************/

typedef void* HRecorder;

PLAYERQ_DLL_API HRecorder xiamiRecorder_create(unsigned long freq,int channels,bool pcm16,unsigned int samples);
PLAYERQ_DLL_API void xiamiRecorder_destroy(HRecorder recorder);

PLAYERQ_DLL_API bool xiamiRecorder_start(HRecorder recorder);
PLAYERQ_DLL_API bool xiamiRecorder_stop(HRecorder recorder);

PLAYERQ_DLL_API unsigned long xiamiRecorder_samplesSize(HRecorder recorder);
PLAYERQ_DLL_API unsigned long xiamiRecorder_recording(HRecorder recorder,char* buf,unsigned long cnt);

PLAYERQ_DLL_API void testRecord();
PLAYERQ_DLL_API void testRecordEx();

#if defined(__cplusplus)
}
#endif


#endif//_PLAYERQUEUE_H