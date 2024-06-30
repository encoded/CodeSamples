#include <stdlib.h>
#include <stdio.h>

#include "portaudio.h"

/*
    Example portaudio

    Mostly for testing initialisation of portaudio.
*/

int main(int argc, char* argv[])
{
    printf("Example portaudio...\n");

    PaError err = Pa_Initialize();
    if(err != paNoError)
    {
        fprintf(stderr, "PortAudio error: %s\n", Pa_GetErrorText(err));
        return EXIT_FAILURE;
    }
    printf("Portaudio initialized.\n");

    // Listing available devices and printing info
    int numDevices = Pa_GetDeviceCount();
    if( numDevices < 0 )
    {
        fprintf(stderr, "PortAudio error: %s\n", Pa_GetErrorText(numDevices));
        return EXIT_FAILURE;
    }

    printf("Getting %i device(s) info...\n", numDevices);

    const PaDeviceInfo *deviceInfo;
    for (int i = 0; i < numDevices; i++) {
        deviceInfo = Pa_GetDeviceInfo(i);

        printf("Device %d:\n", i + 1);
        printf("  Name: %s\n", deviceInfo->name);
        printf("  Host API Index: %d\n", deviceInfo->hostApi);
        printf("  Max Input Channels: %d\n", deviceInfo->maxInputChannels);
        printf("  Max Output Channels: %d\n", deviceInfo->maxOutputChannels);
        printf("  Default Low Input Latency: %.3f seconds\n", deviceInfo->defaultLowInputLatency);
        printf("  Default Low Output Latency: %.3f seconds\n", deviceInfo->defaultLowOutputLatency);
        printf("  Default High Input Latency: %.3f seconds\n", deviceInfo->defaultHighInputLatency);
        printf("  Default High Output Latency: %.3f seconds\n", deviceInfo->defaultHighOutputLatency);
        printf("  Default Sample Rate: %.2f Hz\n", deviceInfo->defaultSampleRate);
        printf("\n");
    }

    err = Pa_Terminate();
    if(err != paNoError)
    {
        fprintf(stderr, "PortAudio error: %s\n", Pa_GetErrorText(err));
        return EXIT_FAILURE;
    }
    printf("Portaudio terminated.\n");
    
    return EXIT_SUCCESS;
}