#pragma once

#include <atomic>
#include <cstring>

/*
    RingBuffer

    This buffer makes the assumption m_Read and m_Write are changed respectively by the same thread 
    (can be a single thread or two separate running thread)
    In case of two threads: the writing thread is the only one to modify the value of m_write
                            while the reading thread is the only one to modify the value of m_read. 
    It implies a single producer and single consumer.
    When we read those values therefore, it is safe to assume that they will not be modified by the other threads.
    While, on the other hand, m_NumFrames is a thread-safe atomic type.

    In this context "frame" indicate a group of samples.
*/
template<typename T> class RingBuffer
{
public:
    RingBuffer(int samples, int numFrames) :
        m_numSamples(samples),
        m_maxNumFrames(numFrames),
        m_buffer(nullptr),
        m_read(0),
        m_write(0),
        m_numFrames(numFrames) //you'll then read the init values
    {  
        m_buffer = (float*) malloc(m_numSamples * m_maxNumFrames * sizeof(float));
        memset(m_buffer, 0, m_numSamples * m_maxNumFrames * sizeof(float));
    }

    virtual ~RingBuffer()
    {
        free(m_buffer);
    }

    // Deleting other special member functions as they may cause shallow copies or dangling pointers
    RingBuffer(const RingBuffer&) = delete;
    RingBuffer& operator=(const RingBuffer&) = delete;
    RingBuffer(RingBuffer&& other) = delete;
    RingBuffer& operator=(RingBuffer&& other) = delete;

    bool canWrite()
    {
        return m_numFrames.load() != m_maxNumFrames;
    }

    bool canRead()
    {
        return m_numFrames.load() != 0;
    }

    T* getWriteBuffer()
    {
        return m_buffer + (m_write * m_numSamples);
    }

    T* getReadBuffer()
    {
        return m_buffer + (m_read * m_numSamples);
    }

    void finishWrite()
    {
        m_write = (m_write + 1) % m_maxNumFrames;
        m_numFrames.fetch_add(1);
    }

    void finishRead()
    {
        m_read = (m_read + 1) % m_maxNumFrames;
        m_numFrames.fetch_sub(1);
    }

    //safe to expose as they are const
    const int m_numSamples;
    const int m_maxNumFrames;

private:
    T* m_buffer;
    int m_read; //index to read frame
    int m_write; //index to write frame
    std::atomic<int> m_numFrames;
};
