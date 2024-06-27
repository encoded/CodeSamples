#pragma once

#include <random>

/*
    A collection of utility functions to generate random values.
*/
namespace RandomUtils
{
    /* 
        A class for quickly iniitialisation of random.
        For simplicity, a global variable g_random is instantiated.
        This will work for most case scenario.
    */
    struct Random
    {
        Random()
        {
            //initialising the random generator with a random device
            std::random_device m_rd;
            m_rg = std::mt19937(m_rd()); 
        }

        /* 
            Returns a random real in the range min and max (both included).
        */
        template<typename T>
        T getRandRealInRange(T min, T max)
        {
            std::uniform_real_distribution<T> dist(min, max);
            return dist(m_rg);
        }

        /* 
            Returns a random int in the range min and max (both included).
        */
        template<typename T>
        T getRandIntInRange(T min, T max)
        {
            std::uniform_int_distribution<T> dist(min, max);
            return dist(m_rg);
        }

    private:
        /* Random generator */
        std::mt19937 m_rg;
    } g_random;  
}