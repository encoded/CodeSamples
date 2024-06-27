#include <stdlib.h>
#include <iostream>

#include "AudioPlayer.h"
#include "PaWrapper.h"
#include "portaudio.h"

/*
    Example audio player.
    Demonstration of an audio player that reads a file loaded with AudioFile library.
    The data is written onto portaudio device callback buffer.
*/

/************************ PARAMS ****************************/

const int g_numChannels = 1; // output number of channels for the audio stream
const int g_processTimeSeconds = 5; // processing time
const int g_loop = true; // whether to loop of not

/************************************************************/

int main(int argc, char* argv[])
{
    printf("Example audio player...\n");

    const std::string filePath = std::string(TESTSOUND_PATH);
    AudioPlayer audioPlayer(filePath.c_str());
    audioPlayer.setLoop(g_loop);
    audioPlayer.play();

    uint32_t sampleRate = audioPlayer.file.getSampleRate();
    int numChannels = audioPlayer.file.getNumChannels();

    auto callback = [](const void *inputBuffer, void *outputBuffer, 
                            unsigned long framesPerBuffer,
                            const PaStreamCallbackTimeInfo* timeInfo,
                            PaStreamCallbackFlags statusFlags,
                            void *userData)
    {
        AudioPlayer* audioPlayer = (AudioPlayer*) userData;
        float *out = (float*)outputBuffer;

        // init buffer with zeros
        memset(outputBuffer, 0, framesPerBuffer*g_numChannels*sizeof(float));
        
        audioPlayer->execute(out, framesPerBuffer, g_numChannels);

        return 0;
    };

    PaWrapper pa;
    pa.openStartStream(0, g_numChannels, sampleRate, paFramesPerBufferUnspecified, callback, &audioPlayer);
    pa.sleepFor(g_processTimeSeconds);
    
    return EXIT_SUCCESS;
}