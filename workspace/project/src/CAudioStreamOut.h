/*
 * CAudioStreamOut.h
 *
 *  Created on: Nov 17, 2019
 *      Author: NIKHIL
 */

#ifndef CAUDIOSTREAMOUT_H_
#define CAUDIOSTREAMOUT_H_
#include "portaudio.h"
#include "CASDDException.h"

enum AUDIO_STREAM_STATE{STREAM_NOTREADY, STREAM_READY, STREAM_PLAYING};

class CAudioStreamOut
{
	PaStreamParameters m_outputParameters; // desired configuration of the stream
	PaStream* m_stream;					 // we'll get the address of the stream from Pa_OpenStream
	PaError m_err;						 // PortAudio specific error values
	AUDIO_STREAM_STATE m_streamState;	// state of the object
	int m_channelCnt, m_blockCnt;
	long m_fs, m_N, m_framesPerBlock, m_sbufsize;
//	CASDDException m_asddexception();


public:
	CAudioStreamOut();
	virtual ~CAudioStreamOut();

//	void init(int channelCnt); // this will initialize the PortAudio class.

	void open(int channelCnt,long fs, int blockCnt, int framesPerBlock, long sblockSize); // this will open the stream and change the state from not ready to ready

	void start(); // this will start the playback of the stream when the state is ready and changes it to playing

//	void play(float* sbufBlock); // this will play the stream when the state is playing

	void play(float* sbufBlock, long frameSize); // to play the last block

	void pause(); // this will pause/stop the stream playback when the state is in playing and changes to ready

	void resume(); // this will start or resume playback when the state is in ready and changes to playing

	void stop(); // this will stop the playback when the state is in playing and changes to ready

	void close(); // this will close the audio stream when the state is ready and changes the state to not ready
};

#endif /* CAUDIOSTREAMOUT_H_ */
