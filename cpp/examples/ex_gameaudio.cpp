#define LOG_MUTEX_LEVEL 1

#include <vector>

#include "AudioFile.h"
#include "PaSoundEngine.h"
#include "SineGenerator.h"
#include "Sound.h"
#include "TaskQueue.h"
#include "Timer.h"

/*
    Example game audio.
    A more comprehensive example showing the use of:
    - Sound Engine: communicating to an audio device using a ring buffer and therefore allowing playback of audio data.
    - Task Queue: used to post task requests (in this case play/stop sound) from the main game thread to the audio thread.
    - Sound: class representing a sound.
    Sounds are identified using ids and they are pre-loaded in the sound engine.
*/

/************************ PARAMS ****************************/

const double g_sampleRate = 44100.;
const int g_numChannels = 2;
const int g_framesPerBuffer = 1024;

/************************************************************/

/* Some const identifier for sounds */
const unsigned long g_soundIdSine = 1;
const unsigned long g_soundIdShot = 2;
const unsigned long g_soundIdStart = 3;
const unsigned long g_soundIdMusic = 4;

const char* getNameForSoundId(unsigned long id)
{
    switch(id)
    {
        case g_soundIdSine:
            return "Sine";
        case g_soundIdShot:
            return "Shot";	
        case g_soundIdStart:
            return "Start";
        case g_soundIdMusic:
            return "Music";	
        case 0:
        default:
            return "None";
    }
}

class TestSoundEngine : public PaSoundEngine
{
public:
    TestSoundEngine(double sampleRate, int numChannels, unsigned long framesPerBuffer):
        PaSoundEngine(sampleRate, numChannels, framesPerBuffer),
        m_queue(10) //maximum queue capacity
    {
        //============== Loading sounds into memory ===================//

        m_sounds.reserve(4);

        // We generate some sine samples using a sine generator
        {
            float sineFreq = 440.f;
            float soundDurationSeconds = 1.f;
            int soundLengthSamples = static_cast<int>(soundDurationSeconds * sampleRate);
            size_t bytes = soundLengthSamples * sizeof(float); //sound data is mono
            float* buffer = (float*)malloc(bytes);
            SineGenerator sineGenerator(sineFreq, sampleRate);
            sineGenerator.execute(buffer, soundLengthSamples, 1);

            Sound sine(g_soundIdSine);
            sine.load(buffer, soundLengthSamples);
            
            m_sounds.emplace_back(std::move(sine));
            free(buffer);
        }

        // We load sample data from sounds that we import from resources
        {
            struct SoundData
            {
                std::string m_path;
                int m_id = SOUND_INVALID_ID;
                bool m_loop = false;
            };

            const int numSounds = 3;
            SoundData soundData[numSounds];
            soundData[0].m_path = std::string(RESOURCES_PATH) + "/audio/44.1/shot.wav";
            soundData[0].m_id = g_soundIdShot;
            soundData[1].m_path = std::string(RESOURCES_PATH) + "/audio/44.1/racestart.wav";
            soundData[1].m_id = g_soundIdStart;
            soundData[2].m_path = std::string(RESOURCES_PATH) + "/audio/44.1/titlemusic.wav";
            soundData[2].m_id = g_soundIdMusic;
            soundData[2].m_loop = true;

            for(int i=0; i<numSounds; ++i)
            {
                AudioFile<float> file;
                assert(file.load(soundData[i].m_path));
                if(file.getSampleRate() != static_cast<uint32_t>(g_sampleRate))
                {
                    LM_LOG("Loaded file sample rate %i is different than sound engine %.0d.", file.getSampleRate(), g_sampleRate);
                    assert(false);
                }
                if(file.getNumChannels() > 1)
                {
                    LM_LOG("Loaded file is not mono. Only the left channel data will be loaded into the sound.");
                }
                Sound fileSound(soundData[i].m_id);
                fileSound.setLoop(soundData[i].m_loop);
                fileSound.load(file.samples[0].data(), file.samples[0].size()); //only mono supported
                m_sounds.emplace_back(std::move(fileSound));
            }
        }
    }

    void playSound(unsigned long soundId)
    {
        struct TaskParams
        {
            unsigned long soundId;
        } taskParams;
        taskParams.soundId = soundId;

        auto task = [](void* context, void* params, float deltaTime)
        {
            TestSoundEngine* soundEngine = (TestSoundEngine*)context;
            TaskParams* taskParams = (TaskParams*)params;

            Sound* s = soundEngine->getSound(taskParams->soundId);
            if(!s)
            {
                return;
            }

            s->play();
            LM_LOG("Playing sound %s", getNameForSoundId(s->getId()));
        };

        m_queue.push(task, this, &taskParams, sizeof(taskParams));
    }

    void stopSound(unsigned long soundId)
    {
        struct TaskParams
        {
            unsigned long soundId;
        } taskParams;
        taskParams.soundId = soundId;

        auto task = [](void* context, void* params, float deltaTime)
        {
            TestSoundEngine* soundEngine = (TestSoundEngine*)context;
            TaskParams* taskParams = (TaskParams*)params;

            Sound* s = soundEngine->getSound(taskParams->soundId);
            if(!s)
            {
                return;
            }

            s->stop();
            LM_LOG("Stopping sound %s", getNameForSoundId(s->getId()));
        };

        m_queue.push(task, this, &taskParams, sizeof(taskParams));
    }

private:
    // we process the queue here - beginning of update function
    virtual void audioThreadProcess(float deltaTime) override
    {
        // processing the tasks queue
        processTaskQueue(deltaTime);
    }

    // we do any audio processing here - each time a write buffer is available
    virtual void audioThreadExecute(float* outputBuffer, unsigned long framesPerBuffer, int numChannels) override
    {
        // clean buffer
        memset(outputBuffer, 0, framesPerBuffer*numChannels*sizeof(float));

        // processing sounds
        for(int i=0; i<m_sounds.size(); ++i)
        {
            m_sounds[i].execute(outputBuffer, framesPerBuffer, numChannels);
        }
    }

    /* Retrieve a sound based on its id */
    Sound* getSound(unsigned long soundId)
    {
        // ensure we call this function from the audio thread
        assert(isInAudioThread());

        for(int i=0; i<m_sounds.size(); ++i)
        {
            Sound& s = m_sounds[i];
            if(s.getId() == soundId)
            {
                return &s;
            }
        }

        LM_ERROR("Could not find sound with id %i.", soundId);

        return nullptr;
    }

    void processTaskQueue(float deltaTime)
    {
        // ensure we call this function from the audio thread
        assert(isInAudioThread());

        // No processing if there are no tasks to process
        if(!m_queue.getNumTasks())
        {
            return;
        }

        LM_VERBOSE("Begin frame.");

        /* For simplicity we define a fix size of number of tasks per frame. */
        int numProcessedTasks = 0;
        
        while(numProcessedTasks < m_numMaxTasksPerFrame)
        {
            TaskQueue::Task task;
            if(!m_queue.pop(task))
            {
                break;
            }

            task.execute(deltaTime);
            ++numProcessedTasks;
        }

        LM_VERBOSE("End frame.");
    }

    /* This NOT THREAD SAFE and should only be accessed by the update thread. */
    std::vector<Sound> m_sounds; // examples of sounds loaded in memory

    TaskQueue m_queue;
    const int m_numMaxTasksPerFrame = 2;
};

int main(int argc, char* argv[])
{
    printf("Hello World from main...\n");

    TestSoundEngine soundEngine(g_sampleRate, g_numChannels, g_framesPerBuffer);
    soundEngine.initialise();
    
    // 1. Let's play the start sound and music
    soundEngine.playSound(g_soundIdStart);
    soundEngine.sleepFor(1); //wait

    // 2. Let's play some music
    soundEngine.playSound(g_soundIdMusic);
    soundEngine.sleepFor(4); //wait

    // 3. Play sine
    soundEngine.playSound(g_soundIdSine);
    soundEngine.sleepFor(3); //wait

    // 4. Stop music
    soundEngine.stopSound(g_soundIdMusic);
    soundEngine.sleepFor(1); //wait

    // 5. Playing a shot sound
    soundEngine.playSound(g_soundIdShot);
    soundEngine.sleepFor(2); //wait

    soundEngine.terminate();
    
    return EXIT_SUCCESS;
}