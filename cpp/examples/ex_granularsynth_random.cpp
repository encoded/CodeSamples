#include <string>

#include "AudioFile.h"
#include "GranularSynth.h"
#include "PaSoundEngine.h"
#include "RandomUtils.h"

/*
    Example granular synth random.
    An actual GranularSynth which uses some randomised parameters.
*/

/************************ PARAMS ****************************/

const int g_numChannels = 1; // doesn't matter as the granular synth is always mono!
const int g_processTimeSeconds = 10; // processing time
const std::string g_audioFilePath = std::string(RESOURCES_PATH) + "/audio/44.1/story.wav";

// granular synth params
int g_grainStartPositionRange[2] = {0, 0}; // the source index from where we're taking the grain from (capped at duration of the source)
const int g_grainDurationSamples[2] = {1000, 10000}; // the grain lenght in num samples (this could be potentially expressed as a % of the source length)
const float g_grainOverlap[2] = {0.4f, 0.6f}; // the overlap between grains expressed as a % of the grain length (range between 0.01f and 1.f)
const float g_grainPitch[2] = {0.8f, 1.2f}; // the grain pitch shift (range between 0.1f and 10.f)

/************************************************************/

class RandGranularSynth : public IGranularSynth
{
    virtual void getParams(int& grainStartPosition, int& grainDurationSamples, float& grainOverlap, float& grainPitch) override
    {
        grainStartPosition = RandomUtils::g_random.getRandIntInRange<int>(g_grainStartPositionRange[0], g_grainStartPositionRange[1]);
        grainDurationSamples = RandomUtils::g_random.getRandIntInRange<int>(g_grainDurationSamples[0], g_grainDurationSamples[1]);
        grainOverlap = RandomUtils::g_random.getRandRealInRange<float>(g_grainOverlap[0], g_grainOverlap[1]);
        grainPitch = RandomUtils::g_random.getRandRealInRange<float>(g_grainPitch[0], g_grainPitch[1]);
    }
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

    g_grainStartPositionRange[1] = m_audioFile.samples[0].size();

    RandGranularSynth granularSynth;
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
        RandGranularSynth* granularSynth = (RandGranularSynth*) userData;
        granularSynth->execute(out, framesPerBuffer, g_numChannels);

        return 0;
    };

    /********************************/

    PaWrapper pa;
    pa.openStartStream(0, g_numChannels, m_audioFile.getSampleRate(), paFramesPerBufferUnspecified, callback, &granularSynth);
    pa.sleepFor(g_processTimeSeconds);
}