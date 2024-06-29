#pragma once

#include <iostream>
#include <semaphore>
#include <thread>

#include "LogMutex.h"
#include "RingBuffer.h"
#include "Timer.h"

/*
    SoundEngine.

    Interface for a sound engine.

    Reference:
    Murray, Dan. "Multithreading for Game Audio." Game Audio Programming 2: Principles and Practices, edited by Guy Somberg, CRC Press, Taylor & Francis Group, 2019, pp. 33-59.
*/
class SoundEngine
{
public:
    SoundEngine(double sampleRate, int numChannels, unsigned long framesPerBuffer, int ringBufferNumFrames = 3):
        m_sampleRate(sampleRate),
        m_numChannels(numChannels),
        m_framesPerBuffer(framesPerBuffer),
        m_buffers(framesPerBuffer * numChannels, ringBufferNumFrames),
        m_audioThreadRunningFlag(false),
        m_semaphore(0),
        m_initialised(false)
    {
        LM_VERBOSE("SoundEngine created.");
    }

    virtual ~SoundEngine()
    {
        terminate(); //making sure sound enigne is terminated 
        LM_VERBOSE("SoundEngine shut down completed.");
    }

    /* Initialise sound engine */
    virtual bool initialise()
    {
        if(!m_initialised)
        {
            m_audioThreadRunningFlag.store(true);
            m_audioThread = std::thread(&SoundEngine::process, this);

            m_initialised = true;

            LM_LOG("SoundEngine initialised.");

            return true;
        }

        return false;
    }

    /* Terminate sound engine */
    virtual bool terminate()
    {
        if(m_initialised)
        {
            m_semaphore.release();
            m_audioThreadRunningFlag.store(false); // signal the audio thread to stop
            if (m_audioThread.joinable())
            {
                m_audioThread.join(); //wait for the write thread to finish
            }
            m_initialised = false;

            LM_LOG("SoundEngine terminated.");

            return true;
        }

        return false;
    }

    bool isInitialised()
    {
        return m_initialised;
    }

protected:
    /* To ensure functions are called from the correct thread */
    bool isInAudioThread()
    {
        return std::this_thread::get_id() == m_audioThread.get_id();
    }

    /* Called theoretically by the audio device where craving more audio data*/
    static void callback(float* buffer, int numChannels, int numFrames, void* cookie)
    {
        SoundEngine* soundEngine = (SoundEngine*) cookie;
        soundEngine->writeDataToDevice(buffer);
    }

    const double m_sampleRate;
    const int m_numChannels;
    const unsigned long m_framesPerBuffer;

private:    

    void writeDataToDevice(float* buffer)
    {
        LM_VERBOSE("Call to write data to device.");

        // copies data into the audio device buffer
        if(m_buffers.canRead())
        {
            LM_VERBOSE("Reading from buffer.");
            memcpy(buffer, m_buffers.getReadBuffer(), getReadWriteBufferBytes());
            m_buffers.finishRead();

            // notify the audio thread to compute more data
            LM_VERBOSE("Notify audio thread to compute frame.");
            m_semaphore.release();
        }
    }

    /* Function called from the sound engine audio thread where we could process our audio data */
    virtual void audioThreadProcess(float deltaTime)
    {
        LM_VERBOSE("Call to process from audio thread.");
    }

    /* Function called from the sound engine audio thread where we could process our audio data */
    virtual void audioThreadExecute(float* outputBuffer, unsigned long framesPerBuffer, int numChannels)
    {
        LM_VERBOSE("Call to execute from audio thread.");
    }

private:
    void process()
    {
        LM_VERBOSE("Audio thread started.");

        Timer timer;

        while (m_audioThreadRunningFlag.load())
        {
            // general process logic
            audioThreadProcess(timer.tick());

            // wait to be notified of the need to compute a frame
            LM_VERBOSE("Wait to compute audio frames.");

            m_semaphore.acquire();

            while(m_buffers.canWrite())
            {
                // compute as many frames as needed
                LM_VERBOSE("Begin audio frame.");
                
                //request to write into buffer
                audioThreadExecute(m_buffers.getWriteBuffer(), m_framesPerBuffer, m_numChannels);

                m_buffers.finishWrite();

                // compute as many frames as needed
                LM_VERBOSE("End audio frame.");
            }
        }

        LM_VERBOSE("Audio thread exiting.");
    }

    // helper function to get the num bytes for the ring buffer frames
    int getReadWriteBufferBytes() const
    {
        return m_buffers.m_numSamples * sizeof(float);
    }

    RingBuffer<float> m_buffers;
    std::thread m_audioThread;
    std::atomic<bool> m_audioThreadRunningFlag; //atomic flag to control the lifetime of the audio thread
    std::binary_semaphore m_semaphore; // semaphore used to notify the audio thread when to compute more audio data
    bool m_initialised;
};