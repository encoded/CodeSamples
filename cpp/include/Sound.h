#pragma once

#include <memory>
#include <stdio.h>

#include "LogMutex.h"
#include "Transport.h"

#define SOUND_INVALID_ID 0

/*
    A simple class to represent a sound.
    A sound entity comprises a unique id and some audio data (which for simplicity is MONO).
    This sound class inherits from ITransport which is a wrapper for a state and playhead to play the sound.
*/
class Sound : public ITransport
{  
public:      
    Sound(unsigned long id = SOUND_INVALID_ID):
        m_id(id),
        m_data(nullptr),
        m_lengthSamples(0)
    {  

    }

    virtual ~Sound()
    {
        if(m_data)
        {
            free(m_data);
        }
    }

    Sound(const Sound& other):
        ITransport(other)
    {
        this->m_id = other.m_id;

        if(other.isValid())
        {
            this->load(other.m_data, other.m_lengthSamples);
        }
        else
        {
            this->m_data = nullptr;
            this->m_lengthSamples = 0;
        }
    }

    Sound& operator=(const Sound& other)
    {
        if(this != &other)
        {
            ITransport::operator=(other); // Copy base class state

            this->m_id = other.m_id;
            this->m_lengthSamples = 0;

            /* Because we are assigning new data, we need to always delete existing data */
            if(this->m_data)
            {
                free(this->m_data);
                this->m_data = nullptr;
            }   

            if(other.isValid())
            {
                this->load(other.m_data, other.m_lengthSamples);
            }
        }

        return *this;
    }

    Sound(Sound&& other):
        ITransport(std::move(other)),
        m_id(other.m_id),
        m_data(other.m_data),
        m_lengthSamples(other.m_lengthSamples)
    {
        other.m_id = SOUND_INVALID_ID;
        other.m_data = nullptr;
        other.m_lengthSamples = 0;
    }

    Sound& operator=(Sound&& other)
    {
        if (this != &other)
        {
            ITransport::operator=(std::move(other)); // Move base class state

            /* Because we are assigning new data, we need to always delete existing data */
            if(this->m_data)
            {
                free(this->m_data);
                this->m_data = nullptr;
            }  

            this->m_id = other.m_id;
            this->m_data = other.m_data; // no need to deallocating and reallocating data as we're moving it
            this->m_lengthSamples = other.m_lengthSamples;

            other.m_id = SOUND_INVALID_ID;
            other.m_data = nullptr;
            other.m_lengthSamples = 0;
        }
        return *this;
    }

    /* Prevent sound from playing if invalid */
    virtual void play()
    {
        if(isValid())
        {
            ITransport::play();
        }
        else
        {
            LM_ERROR("Sound: cannot play sound with invalid data!");
        }
    }

    /* Update the playing status of this sound */
    void execute(float* outputBuffer, unsigned long framesPerBuffer, int numChannels)
    {
        if(isPlaying())
        {
            for(unsigned int i=0; i<framesPerBuffer; i++)
            {
                int playhead = getAndAdvance(m_lengthSamples);
                if(playhead < 0)
                {
                    break;
                }

                float data = m_data[playhead];

                // sound is mono and we're sending the signal to all channels
                for(int c=0; c<numChannels; ++c)
                {
                    *outputBuffer++ += data;
                }
            }
        }
    }

    /* Allocating sound data */
    bool load(float* data, int lengthSamples)
    {
        if(!data || lengthSamples <= 0)
        {
            LM_ERROR("Sound: cannot load invalid data!");
            return false;
        }

        if(m_data)
        {
            free(m_data);
        }

        size_t bytes = lengthSamples * sizeof(float);
        m_data = (float*)malloc(bytes);
        memcpy(m_data, data, bytes);

        m_lengthSamples = lengthSamples;

        return true;
    }

    bool isValid() const
    {
        return m_data != nullptr && m_id != SOUND_INVALID_ID;
    }

    unsigned long getId() const
    {
        return m_id;
    }

private:
    unsigned long m_id; // unique identifier for this sound
    float* m_data; // pointer to the sound data
    int m_lengthSamples; // length of the sound in samples
};