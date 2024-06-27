#include <string>

#include "AudioFile.h"
#include "GranularSynth.h"
#include "PaSoundEngine.h"
#include "RandomUtils.h"

/*
    Example granular synth.
    Demonstration of the correct processing of the GranularSynth.
    For this, we are reconstructing the original signal, but you can change the pitch.
*/

/************************ PARAMS ****************************/

const int g_numChannels = 1; // doesn't matter as the granular synth is mono
const int g_processTimeSeconds = 10; // processing time
const std::string g_audioFilePath = std::string(RESOURCES_PATH) + "/audio/44.1/story.wav";
const float g_grainPitch = 1.2f; // you can change the pitch of the source

/************************************************************/

class TestGranularSynth : public IGranularSynth
{
    virtual void getParams(int& grainStartPosition, int& grainDurationSamples, float& grainOverlap, float& grainPitch) override
    {
        // test reconstruction of the signal
        grainStartPosition = m_head;
        grainDurationSamples = 8192; // arbitrary duration
        grainOverlap = 0.5f;
        grainPitch = g_grainPitch;

        m_head += (grainDurationSamples * grainOverlap);
        if(m_head >= m_source->size())
        {
            m_head = 0;
        }
    }

    int m_head = 0;;
};

int main(int argc, char* argv[])
{
    printf("Example granular synth random...\n");

    //============== Loading file into memory ===================//
    AudioFile<float> m_audioFile;
    assert(m_audioFile.load(g_audioFilePath));
    if(m_audioFile.getNumChannels() > 1)
    {
        LM_LOG("Loaded file is not mono. Only the left channel data will be used by the granular synth.");
    }

    TestGranularSynth granularSynth;
    granularSynth.init(m_audioFile.samples[0]);

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
        TestGranularSynth* granularSynth = (TestGranularSynth*) userData;
        granularSynth->execute(out, framesPerBuffer, g_numChannels);

        return 0;
    };

    /********************************/

    PaWrapper pa;
    pa.openStartStream(0, g_numChannels, m_audioFile.getSampleRate(), paFramesPerBufferUnspecified, callback, &granularSynth);
    pa.sleepFor(g_processTimeSeconds);
}
