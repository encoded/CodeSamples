#pragma once

#include <memory>

#include "Math.h"

/*
    A sine generator using a wavetable to generate a sinusoid.
*/
struct SineGenerator
{ 
    SineGenerator(float freqHz, double sampleRate):
        m_data(nullptr),
        m_dataSize(0),
        m_phaseIndex(0),
        m_gain(1.f)
    {
        setFrequency(freqHz, sampleRate);
    }

    virtual ~SineGenerator()
    {
        if(m_data)
        {
            free(m_data);
        }
    }

    // Deleting other special member functions as they may cause shallow copies or dangling pointers
    SineGenerator(const SineGenerator&) = delete;
    SineGenerator& operator=(const SineGenerator&) = delete;
    SineGenerator(SineGenerator&& other) = delete;
    SineGenerator& operator=(SineGenerator&& other) = delete;

    void setGain(float gain)
    {
        m_gain = gain;
    }

    void execute(float* outputBuffer, unsigned long framesPerBuffer, int numChannels)
    {
        for(unsigned int i=0; i<framesPerBuffer; i++)
        {
            // writing same data to all channels
            float data = m_data[m_phaseIndex] * m_gain;
            m_phaseIndex = (m_phaseIndex + 1) % m_dataSize;
        
            for(int c=0; c<numChannels; ++c)
            {
                *outputBuffer++ = data;
            }
        }    
    }

private:
    /* As this is allocating and deallocating memory we make it private to prevent its use in unwanted places: e.g. callback function */
    void setFrequency(float freqHz, double sampleRate)
    {
        /* Free previously allocated data */
        if(m_data)
        {
            free(m_data);
        }

        /* Allocate memory for wavetable based on derived size */
        m_dataSize = calculateTableSize(sampleRate, freqHz);
        m_data = (float*) malloc(m_dataSize * sizeof(float));

        /* Calculating the phase value for the sine wave */
        for(int i=0; i<m_dataSize; ++i)
        {
            m_data[i] = (float) sin(((double)i/(double)m_dataSize) *  Math::M_PI * 2.);
        }
    }

    /* To allow the calculation of the size of our sine table based on the sample rate and sine frequency in Hz */
    constexpr int calculateTableSize(double sampleRate, float sineFreqHz) 
    {
        return static_cast<int>(std::round(sampleRate / sineFreqHz));
    }

    float* m_data; // pointer to our watable data
    int m_dataSize; // size of our wavetable
    int m_phaseIndex; // current phase index to our wavetable
    float m_gain; //to adjust the volume
};