#pragma once

#include <algorithm>
#include <memory>
#include <cmath>
#include <vector>
#include <iostream>

#include "AudioSignalUtils.h"
#include "LogMutex.h"

/*
    GranularSynth. 
    Abstract class for a granular synth.
*/
class IGranularSynth
{
public:
    IGranularSynth():
        m_indexNextGrain(0),
        m_samplesNextGrain(0)
    {

    }

    virtual void init(const std::vector<float>& source)
    {
        m_source = &source;

        m_indexNextGrain = 0;
        m_samplesNextGrain = 0;
    }

    /* Called before triggering a new grain. It provides an interface to change params in derived classes. */
    virtual void getParams(int& grainStartPosition, int& grainDurationSamples, float& grainOverlap, float& grainPitch) = 0;

    virtual void execute(float* outputBuffer, unsigned long framesPerBuffer, int numChannels)
    {
        if(!m_source)
        {
            return;
        }

        // writing to buffer
        for(int i = 0; i < framesPerBuffer; ++i)
        {
            // Triggering next grain
            if(m_samplesNextGrain == 0)
            {
                // getting synthesis params
                int grainStartPosition = 0;
                int grainDurationSamples = 0;
                float grainOverlap = 0.5f;
                float grainPitch = 1.f;
                getParams(grainStartPosition, grainDurationSamples, grainOverlap, grainPitch);
                fixParams(grainStartPosition, grainDurationSamples, grainOverlap, grainPitch);

                // triggering next grain
                triggerNextGrain(grainStartPosition, grainDurationSamples, grainPitch);
            
                m_samplesNextGrain = static_cast<int>(grainDurationSamples * grainOverlap);        
            }
            --m_samplesNextGrain;

            // processing active grains
            float data = 0.f;
            for(int g = 0; g < m_maxNumGrains; ++g)
            {
                

                Grain& grain = m_grains[g];
                if(grain.isActive())
                {
                    data += grain.process(*m_source);
                }
            }

            //copy same data to all channels
            for(int c=0; c<numChannels; ++c)
            {
                *outputBuffer++ += data;
            }
        }
    }

    void tearDown()
    {
        // reset grains
        for(int i=0; i < m_maxNumGrains; ++i)
        {
            m_grains[i].tearDown();
        }

        m_source = nullptr;
    }

protected:
    const std::vector<float>* m_source; // a pointer to the source data

private:
    // to ensure params used to get next grain are always valid
    void fixParams(int& grainStartPosition, int& grainDurationSamples, float& grainOverlap, float& grainPitch)
    {
        // the grain cannot be longer than the available samples in the source
        int maxSampleIndex = (*m_source).size() - 1;
        grainStartPosition = std::clamp<int>(grainStartPosition, 0, maxSampleIndex);
        grainDurationSamples = std::clamp<int>(grainDurationSamples, 0, maxSampleIndex - grainStartPosition);
        grainOverlap = std::clamp<float>(grainOverlap, 0.01f, 1.0f); // clamp the grainOverlap to the range [0.1, 1.0]
        grainPitch = std::clamp<float>(grainPitch, 0.1f, 10.0f);
    }

    struct Grain
    {
        public:
            Grain():
                m_sourceStartPosition(0),
                m_lengthSamples(0),
                m_grainPitch(1.f),
                m_active(false),
                m_headPosition(0)
            {

            }

            void init(int sourceStartPosition, int lengthSamples, float grainPitch)
            {
                m_sourceStartPosition = sourceStartPosition;
                m_lengthSamples = lengthSamples;
                m_grainPitch = grainPitch;

                m_active = true;
                m_headPosition = 0;
            }

            float process(const std::vector<float>& source)
            {
                using namespace AudioSignalUtils;

                if (!m_active)
                {
                    return 0.f;
                }

                if (m_headPosition >= m_lengthSamples)
                {
                    m_active = false;
                    return 0.f;
                }

                // Calculate the current sample index
                float actualHead = m_headPosition * m_grainPitch;
                int sourceIndex = m_sourceStartPosition + static_cast<int>(actualHead);

                // Perform linear interpolation - in so doing the length of grain remain the same
                int nextIndex = sourceIndex + 1;
                float frac = actualHead - static_cast<int>(actualHead);
                float sample1 = source[sourceIndex];
                float sample2 = (nextIndex < source.size()) ? source[nextIndex] : 0.f; // Handle out-of-bounds

                float interpolatedSample = sample1 + frac * (sample2 - sample1);

                // Apply window function
                float windowValue = Windows::hann<float>(m_headPosition, m_lengthSamples);
                float data = interpolatedSample * windowValue;

                // Update head position
                ++m_headPosition; // Increment by 1

                return data;
            }

            void tearDown()
            {
                m_active = false;
                m_sourceStartPosition = 0;
                m_lengthSamples = 0;
                m_headPosition = 0;
            }

            bool isActive() const
            {
                return m_active;
            }

            int getLengthSamples() const
            {
                return m_lengthSamples;
            }

        private:
            int m_sourceStartPosition; //index sample to the source where we're starting taking the grain from
            int m_lengthSamples; //length in samples of the grain
            float m_grainPitch; // pitch multiplier for the grain

            bool m_active; // whether there are any more samples to process
            int m_headPosition; //the number of grain samples we have already processed
    };

    // trigger a new grain and returns samples to next grain
    Grain& triggerNextGrain(int sourceStartPosition, int lengthSamples, float grainPitch)
    {
        // Just a warning of whether we are chopping off grains that still needed to be processed
        Grain& nextGrain = m_grains[m_indexNextGrain];
        if(nextGrain.isActive())
        {
            LM_ERROR("Initialising a grain that is still being processed. Increase the maxNumGrains.");
        }

        nextGrain.init(sourceStartPosition, lengthSamples, grainPitch);
        m_indexNextGrain =(m_indexNextGrain + 1) % m_maxNumGrains;

        return nextGrain;
    }

    /* Maximum number of concurrent grains */
    static const int m_maxNumGrains = 10;

    Grain m_grains[m_maxNumGrains];
    int m_indexNextGrain;
    int m_samplesNextGrain;
};