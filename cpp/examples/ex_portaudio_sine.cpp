#include <cstdlib>
#include <stdio.h>

#include "portaudio.h"

#include "SineGenerator.h"

/*
    Example portaudio sine.
    Demonstration of sending some sine audio data to a portaudio device.
*/

/************************ PARAMS ****************************/

const float g_freqHz = 440.f;
const double g_sampleRate = 48000.;
const int g_numChannels = 2;
const float g_volume = 0.5f;
const int g_processTimeSeconds = 5; // processing time

/************************************************************/

int main(int argc, char* argv[])
{
    printf("Example portaudio sine...\n");

    /****** Data initialisation ******/

    PaStream *stream;
    SineGenerator sineGenerator(g_freqHz, g_sampleRate);
    sineGenerator.setGain(g_volume); // to avoid referencing global variable from our generator struct 
    auto callback = [](const void *inputBuffer, void *outputBuffer, 
                            unsigned long framesPerBuffer,
                            const PaStreamCallbackTimeInfo* timeInfo,
                            PaStreamCallbackFlags statusFlags,
                            void *userData)
    {
        SineGenerator* sineGenerator = (SineGenerator*) userData;
        float *out = (float*)outputBuffer;
        sineGenerator->execute(out, framesPerBuffer, g_numChannels);

        return 0;
    };

    /********************************/

    /* Initializing portaudio */
    PaError err = Pa_Initialize();
    if(err != paNoError) goto error;

    /* Opening an audio I/O stream. */
    err = Pa_OpenDefaultStream(&stream,
                                0,          /* no input channels */
                                g_numChannels,          /* stereo output */
                                paFloat32,  /* 32 bit floating point output */
                                g_sampleRate,      /* sample rate */ 
                                paFramesPerBufferUnspecified,        /* frames per buffer, use paFramesPerBufferUnspecified 
                                                    to make PortAudio pick the best, possibly changing, buffer size.*/
                                callback, /* this is your callback function */
                                &sineGenerator);       

    if(err != paNoError) goto error;

    /* Starting the stream */
    err = Pa_StartStream(stream);
    if(err != paNoError) goto error;

    /* Allow some time for processing */
    printf("Play for %i seconds.\n", g_processTimeSeconds);
    Pa_Sleep(g_processTimeSeconds*1000);  

    /* Stopping the stream - use Pa_AbortStream if you want not to wait to finish processing the buffer in the callback.*/  
    err = Pa_StopStream(stream);
    if(err != paNoError) goto error;          

    /* Closing the audio I/O stream. */
    err = Pa_CloseStream( stream );
    if(err != paNoError) goto error;                   

    /* Terminating portaudio */
    err = Pa_Terminate();
    if(err != paNoError) goto error;
    
    return EXIT_SUCCESS;

error:
    fprintf(stderr, "PortAudio error: %s\n", Pa_GetErrorText(err));
    return EXIT_FAILURE;
}