#include <stdio.h>

#include "portaudio.h"

#include "RandomUtils.h"

/*
    Example portaudio white noise.
    Writing some white noise into a portaudio device callback buffer.
    For demonstration porpuses we write data only to the left channel.
*/

/************************ PARAMS ****************************/

const int g_numChannels = 2;
const float g_volume = 0.1f; // minimising the risk of damaging the ears
const int g_processTimeSeconds = 10; // processing time

/************************************************************/

// In addition to good practices when it comes to audio callback function (avoiding memory allocation/deallocation, I/O, context switching (such as exec() or yield()), mutex operations, etc.)
// it is not safe to call any PortAudio API functions (except as explicitly permitted in the documentation).
// https://www.portaudio.com/docs/v19-doxydocs/writing_a_callback.html 
int paCallback( const void *inputBuffer, void *outputBuffer,
                           unsigned long framesPerBuffer,
                           const PaStreamCallbackTimeInfo* timeInfo,
                           PaStreamCallbackFlags statusFlags,
                           void *userData )
{
    float *out = (float*)outputBuffer;
    for(unsigned int i=0; i<framesPerBuffer; i++ )
    {
        /* Audio data is interleaved*/
        for(int c=0; c<g_numChannels; ++c)
        {
            if (c == 0) //Left channel
            {
                float value = RandomUtils::g_random.getRandRealInRange<float>(-1.f, 1.f);
                *out++ = value * g_volume;
            }
            else //Right channel
            {
                *out++ = 0.f;  // silence
            }
        }
    }

    return 0;
}

int main(int argc, char* argv[])
{
    printf("Example portaudio white noise...\n");

    /* Initializing portaudio */
    PaError err = Pa_Initialize();
    if(err != paNoError)
    {
        fprintf(stderr, "PortAudio error on initialization: %s\n", Pa_GetErrorText(err));
        return EXIT_FAILURE;
    }

    /* Opening an audio I/O stream. */
    PaStream *stream;
    err = Pa_OpenDefaultStream( &stream,
                                0,          /* no input channels */
                                g_numChannels,          /* stereo output */
                                paFloat32,  /* 32 bit floating point output */
                                44100,      /* sample rate */ 
                                256,        /* frames per buffer, use paFramesPerBufferUnspecified 
                                                    to make PortAudio pick the best, possibly changing, buffer size.*/
                                paCallback, /* this is your callback function */
                                nullptr);        /*This is a pointer that will be passed to
                                                   your callback*/
    if(err != paNoError)
    {
        fprintf(stderr, "PortAudio error when opening default stream: %s\n", Pa_GetErrorText(err));
        return EXIT_FAILURE;
    }

    /* Starting the stream */
    err = Pa_StartStream(stream);
    if(err != paNoError)
    {
        fprintf(stderr, "PortAudio error when starting the stream: %s\n", Pa_GetErrorText(err));
        return EXIT_FAILURE;
    } 

    /* Allow some time for processing */
    printf("Play for %i seconds.\n", g_processTimeSeconds);
    Pa_Sleep(g_processTimeSeconds*1000);  

    /* Stopping the stream - use Pa_AbortStream if you want not to wait to finish processing the buffer in the callback.*/  
    err = Pa_StopStream(stream);
    if(err != paNoError)
    {
        fprintf(stderr, "PortAudio error when stopping the stream: %s\n", Pa_GetErrorText(err));
        return EXIT_FAILURE;
    }             

    /* Closing the audio I/O stream. */
    err = Pa_CloseStream( stream );
    if(err != paNoError)
    {
        fprintf(stderr, "PortAudio error when closing the stream: %s\n", Pa_GetErrorText(err));
        return EXIT_FAILURE;
    }                        

    /* Terminating portaudio */
    err = Pa_Terminate();
    if(err != paNoError)
    {
        fprintf(stderr, "PortAudio error on termination: %s\n", Pa_GetErrorText(err));
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}