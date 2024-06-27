#include <stdio.h>

#include "PaWrapper.h"
#include "SineGenerator.h"
#include "Sound.h"

/*
    Example portaudio sound.
    Demonstration of writing some sine audio data into a Sound and playback using portaudio.
*/

/************************ PARAMS ****************************/

const float g_freqHz = 440.f;
const float g_soundDurationSeconds = 2.f;
const double g_sampleRate = 48000.;
const int g_numChannels = 2;
const float g_volume = 0.5f;
const int g_processTimeSeconds = 5; // processing time

/************************************************************/

int main(int argc, char* argv[])
{
    printf("Example portaudio sound...\n");

    /****** Data initialisation ******/

    //============== Loading sound into memory ===================//
    // We generate some sine samples using a sine generator
    float sineFreq = g_freqHz;
    float soundDurationSeconds = g_soundDurationSeconds;
    int soundLengthSamples = static_cast<int>(soundDurationSeconds * g_sampleRate);
    size_t bytes = soundLengthSamples * sizeof(float); //sound data is mono
    float* buffer = (float*)malloc(bytes);
    SineGenerator sineGenerator(sineFreq, g_sampleRate);
    sineGenerator.setGain(g_volume);
    sineGenerator.execute(buffer, soundLengthSamples, 1);

    const int soundId = 1;
    Sound sound(soundId);
    sound.load(buffer, soundLengthSamples);
    sound.play(); //!Remember to play the sound
    free(buffer);

    //===========================================================//

    auto callback = [](const void *inputBuffer, void *outputBuffer, 
                            unsigned long framesPerBuffer,
                            const PaStreamCallbackTimeInfo* timeInfo,
                            PaStreamCallbackFlags statusFlags,
                            void *userData)
    {
        // init buffer with zeros
        memset(outputBuffer, 0, framesPerBuffer*g_numChannels*sizeof(float));

        float *out = (float*)outputBuffer;
        Sound* sound = (Sound*) userData;
        sound->execute(out, framesPerBuffer, g_numChannels);

        return 0;
    };

    /********************************/

    PaWrapper pa;
    pa.openStartStream(0, g_numChannels, g_sampleRate, paFramesPerBufferUnspecified, callback, &sound);
    pa.sleepFor(g_processTimeSeconds);

}