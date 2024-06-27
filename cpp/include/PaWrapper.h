#pragma once

#include <cassert>
#include <stdio.h>

#include "portaudio.h"

/*
    Wrapper for portaudio so that we don't have to repeat code across the examples (mostly for errors)
    Portaudio initialisation is linked to the lifetime of the object.
*/
class PaWrapper
{
public:    
    PaWrapper():
        m_stream(nullptr)
    {
         /* Initializing portaudio */
        PaError err = Pa_Initialize();
        if(err != paNoError)
        {
            logError(err);
        }
    }

    ~PaWrapper()
    {
        /* Ensuring stream is stopped and closed if active and open */
        stopCloseStream();

        /* Terminating portaudio */
        PaError err = Pa_Terminate();
        if(err != paNoError)
        {
            logError(err);
        }
    }

    // Deleting other special member functions as they may cause shallow copies or dangling pointers
    PaWrapper(const PaWrapper&) = delete;
    PaWrapper& operator=(const PaWrapper&) = delete;
    PaWrapper(PaWrapper&& other) = delete;
    PaWrapper& operator=(PaWrapper&& other) = delete;

    /* Open and start a new stream */
    void openStartStream(int inputChannelCount,
                            int outputChannelCount,
                            double sampleRate,
                            unsigned long framesPerBuffer,
                            PaStreamCallback *streamCallback,
                            void *userData )
    {    
        openStream(inputChannelCount, outputChannelCount, sampleRate, framesPerBuffer, streamCallback, userData);
        startStream();
    } 

    /* Stop (is active) and close a stream */
    void stopCloseStream()
    {
        stopStream();
        closeStream();
    }

    void openStream(int inputChannelCount,
                            int outputChannelCount,
                            double sampleRate,
                            unsigned long framesPerBuffer,
                            PaStreamCallback *streamCallback,
                            void *userData)
    {
        if(!m_stream)
        {
            /* Opening an audio I/O stream. */
            PaError err = Pa_OpenDefaultStream(&m_stream,
                                        inputChannelCount,          /* no input channels */
                                        outputChannelCount,          /* stereo output */
                                        paFloat32,  /* 32 bit floating point output */
                                        sampleRate,      /* sample rate */ 
                                        framesPerBuffer, /* frames per buffer, use paFramesPerBufferUnspecified 
                                                            to make PortAudio pick the best, possibly changing, buffer size.*/
                                        streamCallback, /* this is your callback function */
                                        userData);       

            if(err != paNoError)
            {
                logError(err);
            }  
        }
    }  

    void startStream()
    {
        /* Starting the stream */
        if(m_stream && !Pa_IsStreamActive(m_stream))
        {
            PaError err = Pa_StartStream(m_stream);
            if(err != paNoError)
            {
                logError(err);
            } 
        }
    }  

    void stopStream()
    {
        if(m_stream && Pa_IsStreamActive(m_stream))
        {
            /* Stopping the stream - use Pa_AbortStream if you want not to wait to finish processing the buffer in the callback.*/  
            PaError err = Pa_StopStream(m_stream);
            if(err != paNoError)
            {
                logError(err);
            }  
        }
    }  

    void closeStream()
    {
        /* Closing the audio I/O stream. */
        if(m_stream)
        {
            PaError err = Pa_CloseStream(m_stream);
            if(err != paNoError)
            {
                logError(err);
            }  

            m_stream = nullptr;
        }   
    }  

    void sleepFor(int timeSeconds)
    {
        Pa_Sleep(timeSeconds*1000); 
    }    

private:
    void logError(const PaError& err)
    {
        fprintf(stderr, "PortAudio error: %s\n", Pa_GetErrorText(err));
        assert(false);
    }

    PaStream* m_stream;
};