#pragma once

#include <stdio.h>

/*
    ITransport
    A minimal interface to add some transport states used for instance by an audio player.
*/
class ITransport
{

public:    
    ITransport():
        m_state(TransportState::Stopped),
        m_playhead(0),
        m_loop(false)
    {

    }

    virtual ~ITransport() = 0;

    virtual void play()
    {
        m_state = TransportState::Playing;
    }

    virtual void pause()
    {
        m_state = TransportState::Paused;
    }

    virtual void stop()
    {
        m_state = TransportState::Stopped;
        m_playhead = 0;
    }

    bool isPlaying()
    {
        return m_state == TransportState::Playing;
    }

    void setLoop(bool loop)
    {
        m_loop = loop;
    }

    /* Return current playhead position and advance */
    int getAndAdvance(int length)
    {
        if(m_playhead >= length) 
        {
            // reached end of file
            if(m_loop)
            {
                m_playhead = 0;   
            }
            else
            {
                stop();
                return -1;
            }
        }

        return m_playhead++;
    }

protected:
    enum TransportState {
        Playing,
        Stopped,
        Paused
    } m_state;

private:
    int m_playhead;
    bool m_loop;
};

// Inline definition of the pure virtual destructor
inline ITransport::~ITransport() {}