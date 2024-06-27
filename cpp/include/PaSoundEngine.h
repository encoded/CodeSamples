#pragma once

#include "PaWrapper.h"
#include "SoundEngine.h"

/*
    PaSoundEngine
    A sound engine using a portaudio device callback so that we can hear sound!
*/
class PaSoundEngine : public SoundEngine
{
public:
    PaSoundEngine(double sampleRate, int numChannels, unsigned long framesPerBuffer):
        SoundEngine(sampleRate, numChannels, framesPerBuffer)
    {

    }

    virtual bool initialise() override
    {
        if(SoundEngine::initialise())
        {
            /* To map portaudio callback signature to SoundEngine */
            auto callback = [](const void *inputBuffer, void *outputBuffer, 
                                unsigned long framesPerBuffer,
                                const PaStreamCallbackTimeInfo* timeInfo,
                                PaStreamCallbackFlags statusFlags,
                                void *userData)
            {
                PaSoundEngine* soundEngine = (PaSoundEngine*) userData;
                float *out = (float*)outputBuffer;
                soundEngine->callback(out, soundEngine->m_numChannels, framesPerBuffer, userData);

                return 0;
            };

            pa.openStartStream(0, m_numChannels, m_sampleRate, m_framesPerBuffer, callback, this);
        }
    }

    virtual bool terminate() override
    {
        if(SoundEngine::terminate())
        {
            pa.stopStream();
        }
    }

    /* main thread sleep - same as std::this_thread::sleep_for */
    void sleepFor(int durationSeconds)
    {
        pa.sleepFor(durationSeconds);
    }

    virtual void audioThreadExecute(float* outputBuffer, unsigned long framesPerBuffer, int numChannels) override
    {
        //write here your audio logic
    }

private:
    PaWrapper pa;
};