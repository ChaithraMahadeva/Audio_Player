/*
 * CAudioStreamOut.cpp
 *
 *  Created on: Nov 17, 2019
 *      Author: Chaithra Mahadeva
 */
#include <iostream>
#include "CAudioStreamOut.h"
#include "portaudio.h"
#include "CASDDException.h"

CAudioStreamOut::CAudioStreamOut()
{
	// TODO Auto-generated constructor stub
	m_err = 0;
	m_stream = NULL;
	m_streamState = STREAM_NOTREADY;
	m_channelCnt = 1;
	m_blockCnt = 8;
	m_fs = 16000;
	m_N = m_fs;
	m_framesPerBlock = 0;
	m_sbufsize = m_channelCnt*m_N;
}

CAudioStreamOut::~CAudioStreamOut()
{
	// TODO Auto-generated destructor stub
	close();
}

void CAudioStreamOut::open(int channelCnt,long fs, int blockCnt, int framesPerBlock, long sblockSize)
{
	m_fs =fs;
	m_blockCnt = blockCnt;
	m_N = m_fs;
	m_framesPerBlock = framesPerBlock;
	m_channelCnt = channelCnt;
	m_sbufsize = sblockSize;

	m_err = Pa_Initialize();				// init PortAudio API

	if( m_err == paNoError )
	{
		// the computers default output device should work
		m_outputParameters.device = Pa_GetDefaultOutputDevice();

		if (m_outputParameters.device != paNoDevice)
		{
			// set appropriate output parameters
			m_outputParameters.channelCount = m_channelCnt;	// channels
			m_outputParameters.sampleFormat = paFloat32;  // 32 bit floating point output
			//Default latency values for robust non-interactive applications (e.g. playing sound files)
			//(is ignored ??)
			m_outputParameters.suggestedLatency = Pa_GetDeviceInfo( m_outputParameters.device )->defaultHighOutputLatency;
			m_outputParameters.hostApiSpecificStreamInfo = NULL;	// not necessary

			//m_streamState = STREAM_NOTREADY; // setting state to not ready
		}

	}

	if(m_streamState == STREAM_NOTREADY)
	{
		m_err = Pa_OpenStream(&m_stream, NULL, /* no input */&m_outputParameters, m_fs, m_framesPerBlock,
							//paClipOff,	/* we won't output out of range samples so don't bother clipping them */
							paNoFlag, // lab3
							NULL, 		/* no callback, use blocking API */
							NULL ); 	/* no callback, so no callback userData */

		if( m_err == paNoError )
		{
			m_streamState = STREAM_READY;
		}
	}

}

void CAudioStreamOut::start()
{
	if(m_streamState == STREAM_READY)
	{
		m_err= Pa_StartStream(m_stream);
		if( m_err == paNoError )
		{
			m_streamState = STREAM_PLAYING;
		}
	}

}

void CAudioStreamOut::play(float* sbufBlock, long frameSize)
{
	if(m_streamState == STREAM_PLAYING)
	{
		m_err = Pa_WriteStream( m_stream, sbufBlock, frameSize);
		if( m_err != paNoError )
		{
			close();
		}
		m_streamState = STREAM_PLAYING;
	}
}

void CAudioStreamOut::pause()
{
	stop();
}

void CAudioStreamOut::resume()
{
	start();
}

void CAudioStreamOut::stop()
{
	if(m_streamState == STREAM_PLAYING)
	{
		Pa_StopStream(m_stream);
		m_streamState = STREAM_READY;
	}
}

void CAudioStreamOut::close()
{
	if(m_streamState == STREAM_READY)
	{
		Pa_CloseStream(m_stream);
		m_streamState = STREAM_NOTREADY;
		Pa_Terminate();
	}

}


