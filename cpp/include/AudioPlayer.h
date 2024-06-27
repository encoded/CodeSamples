#pragma once

#include "AudioFile.h"
#include "Transport.h"

/*
    Audio Player
    A minimal audio player used to load and play some sample data from a file loaded wih AudioFile
*/
class AudioPlayer : public ITransport
{
public:    
    AudioPlayer(const char* filePath)
    {
        assert(file.load(filePath));
    }

    void execute(float* outputBuffer, unsigned long framesPerBuffer, int numChannels)
    {
        if(isPlaying())
        {
            for(unsigned int i=0; i<framesPerBuffer; i++)
            {
                int playhead = getAndAdvance(file.getNumSamplesPerChannel());
                if(playhead < 0)
                {
                    break;
                }

                for(int c=0; c<numChannels; ++c)
                {
                    if(c < file.getNumChannels())
                    {
                        *outputBuffer++ = file.samples[c][playhead];
                    }
                    else
                    {
                        *outputBuffer++ = 0.f;
                    }
                }
            }
        }
    }

    /* Keep it public to allow access to member functions */
    AudioFile<float> file; 
};