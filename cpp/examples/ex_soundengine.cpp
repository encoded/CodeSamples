
#include "AudioDevice.h"
#include "SoundEngine.h"

/*
    Example sound engine.
    Demonstration of a sound engine using a mock audio device.
    So that we can showcase this independetely from any other library such as portaudio.
*/

/************************ PARAMS ****************************/

const double g_sampleRate = 48000.;
const int g_numChannels = 2;
const unsigned long g_framesPerBuffer = 512;

/************************************************************/

class MockSoundEngine : public SoundEngine
{
public:
    MockSoundEngine(double sampleRate, int numChannels, unsigned long framesPerBuffer):
        SoundEngine(sampleRate, numChannels, framesPerBuffer),
        m_audioDevice(nullptr)
    {
        
    }

    virtual bool initialise() override
    {
        if(SoundEngine::initialise())
        {
            m_audioDevice = new AudioDevice(m_sampleRate, m_numChannels, m_framesPerBuffer, callback, this);
        }
    }

    virtual bool terminate() override
    {
        if(SoundEngine::terminate())
        {
            if(m_audioDevice)
            {
                delete m_audioDevice;
            }
        }
    }
    

    /* For testing porpuses to simulate audio device process */
    void process(float durationSeconds)
    {
        if(isInitialised())
        {
            m_audioDevice->process(durationSeconds);
        }
    }

private:
    AudioDevice* m_audioDevice;
};

int main(int argc, char* argv[])
{
    printf("Example sound engine...");

    MockSoundEngine soundEngine(g_sampleRate, g_numChannels, g_framesPerBuffer);
    
    soundEngine.initialise();
    soundEngine.process(2);
    soundEngine.terminate();

    //wait some time before reinitialising it
    soundEngine.process(2);

    soundEngine.initialise();
    soundEngine.process(2);
    soundEngine.terminate();

    return EXIT_SUCCESS;
}