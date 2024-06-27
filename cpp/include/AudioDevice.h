#pragma once

#include <iostream>
#include <thread>

/*
    AudioDevice.

    Interface used to replace an actual audio device.
    Does not provide any actual functionality. It's simply an abstraction of an audio device.
*/
class AudioDevice
{
public:
    // Typedef for the callback function type
    typedef void (*CallbackFunc)(float*, int, int, void*);

    /*
        We provide to the audio device sample rate, number of channles, number of samples per channel (i.e. number of frames), 
        a callback function used to fill the audio device buffer whenever data is needed.

        !The total number of samples would be given by: numChannels * numFrames
        As an audio frame consists of a sample from each channel at the same point in time.
    */
    AudioDevice(double sampleRate, int numChannels, int numFrames, CallbackFunc callback, void* cookie):
        m_sampleRate(sampleRate),
        m_numChannels(numChannels),
        m_numFrames(numFrames),
        m_callback(callback),
        m_cookie(cookie)
    {
        // Initialize the buffer to hold the total number of samples
        m_buffer = (float*) malloc(numChannels * numFrames * sizeof(float));
        memset(m_buffer, 0.f, numChannels * numFrames * sizeof(float));
    }

    // Destructor to free the allocated buffer
    virtual ~AudioDevice() {
        if(m_buffer)
        {
            free(m_buffer);
        }
    }

    // Deleting other special member functions as they may cause shallow copies or dangling pointers
    AudioDevice(const AudioDevice&) = delete;
    AudioDevice& operator=(const AudioDevice&) = delete;
    AudioDevice(AudioDevice&& other) = delete;
    AudioDevice& operator=(AudioDevice&& other) = delete;

    //========================= TESTING PURPOSES ONLY ==========================//
    // helper function to simulate calls to the callback function
    // in an audio device this is typically called by the audio processing thread whenever the audio buffer needs to be filled with more data 
    void process(float durationSeconds)
    {
        float elapsedTime = 0.f;
        while(elapsedTime < durationSeconds)
        {
            auto loopStartTime = std::chrono::high_resolution_clock::now(); // Get loop start time

            // callback
            if (m_callback) 
            {
                m_callback(m_buffer, m_numFrames, m_numChannels, m_cookie);
            }

            // arbitrary time 
            std::this_thread::sleep_for(std::chrono::milliseconds(200));

            auto loopEndTime = std::chrono::high_resolution_clock::now(); // Get loop end time

            elapsedTime += std::chrono::duration<float>(loopEndTime - loopStartTime).count();
        }
    }
    //=======================================================================//

private:
    double m_sampleRate;
    int m_numChannels;
    int m_numFrames;
    CallbackFunc m_callback;
    void* m_cookie; // user data
    float* m_buffer; // buffer to hold audio samples
};
