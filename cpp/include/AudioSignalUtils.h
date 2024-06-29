#pragma once

#include <vector>

#include "Math.h"

/*
    A collection of audio signal utility functions.
*/
namespace AudioSignalUtils
{
    /*
        Analysis windows.
        Windows are symmetric by default.
        For symmetric calculation N in the formula should be N - 1.
        Periodic on the other hand allows the window to end on the second-to-last value so that it can perfectly loop over
        Reference: https://uk.mathworks.com/help/signal/ug/windows.html
    */
    namespace Windows
    {
        /* Rectangular window - here mostly for reference */
        template<typename T>
        T rectangular(int n, int N)
        {
            return static_cast<T>(1.);
        }

        /* Triangular window */
        template<typename T>
        T triangular(int n, int N, bool periodic = false)
        {
            if(!periodic)
            {
                N -= 1;
            }

            return static_cast<T>(1. - std::abs((n - N * 0.5) / (N * 0.5)));
        }

        /* Hann window */
        template<typename T>
        T hann(int n, int N, bool periodic = false)
        {
            if(!periodic)
            {
                N -= 1;
            }

            return static_cast<T>(0.5 * (1. - std::cos(2. * Math::M_PI * n / N)));
        }

        template<typename T>
        T hamming(int n, int N, bool periodic = false)
        {
            if(!periodic)
            {
                N -= 1;
            }

            return static_cast<T>(0.54 - 0.46 * std::cos(2. * Math::M_PI * n / N));
        }

        template<typename T>
        T blackman(int n, int N, bool periodic = false)
        {
            if(!periodic)
            {
                N -= 1;
            }

            return static_cast<T>(0.42 - 0.5 * std::cos(2. * Math::M_PI * n / N) + 0.08 * std::cos(4. * Math::M_PI * n / N));
        }

        /* 
            Block windowing 
            In some cases it's quicker to pre-calculate the window into an audio block.
        */

        enum Type
        {
            RECT,
            TRIANG,
            HANN,
            HAMMING,
            BLACKMAN
        };

        /* Utility template function to fill a vector with window data */
        template<typename T>
        void window(int size, Type windowType, std::vector<T>& output, bool periodic = false)
        {
            output.resize(size);

            switch(windowType)
            {
                case Type::TRIANG:
                    for(int i=0; i<size; ++i)
                    {
                        output[i] = triangular<T>(i, size, periodic);
                    }
                    break;
                case Type::HANN:
                    for(int i=0; i<size; ++i)
                    {
                        output[i] = hann<T>(i, size, periodic);
                    }
                    break;
                case Type::HAMMING:
                    for(int i=0; i<size; ++i)
                    {
                        output[i] = hamming<T>(i, size, periodic);
                    }
                    break;
                case Type::BLACKMAN:
                    for(int i=0; i<size; ++i)
                    {
                       output[i] = blackman<T>(i, size, periodic);
                    }
                    break;
                case Type::RECT:
                default:
                    for(int i=0; i<size; ++i)
                    {
                        output[i] = rectangular<T>(i, size);
                    }
                    break;
            }
        }

        /* Utility template function to fill a previously allocated buffer with window data */
        template<typename T>
        void window(T* output, int size, Type windowType, bool periodic)
        {
            switch(windowType)
            {
                case Type::TRIANG:
                    for(int i=0; i<size; ++i)
                    {
                        *output++ = triangular<T>(i, size, periodic);
                    }
                    break;
                case Type::HANN:
                    for(int i=0; i<size; ++i)
                    {
                        *output++ = hann<T>(i, size, periodic);
                    }
                    break;
                case Type::HAMMING:
                    for(int i=0; i<size; ++i)
                    {
                        *output++ = hamming<T>(i, size, periodic);
                    }
                    break;
                case Type::BLACKMAN:
                    for(int i=0; i<size; ++i)
                    {
                        *output++ = blackman<T>(i, size, periodic);
                    }
                    break;
                case Type::RECT:
                default:
                    for(int i=0; i<size; ++i)
                    {
                        *output++ = rectangular<T>(i, size, periodic);
                    }
                    break;
            }
        }
    }
}


