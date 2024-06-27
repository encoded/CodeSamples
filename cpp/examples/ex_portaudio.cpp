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

    err = Pa_Terminate();
    if(err != paNoError)
    {
        fprintf(stderr, "PortAudio error: %s\n", Pa_GetErrorText(err));
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}