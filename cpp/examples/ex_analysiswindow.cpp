#include <iostream>
#include <vector>
#include <cmath>
#include <cassert>

#include "AudioSignalUtils.h"

/*
    Example analysis window

    A very quick example of windowing a signal using different windows type.
    The windowing is both symmetric and periodic.
*/

int main(int argc, char* argv[])
{
    std::cout << "Example analysis window...\n";
    
    using namespace AudioSignalUtils;

    // Example audio block
    std::vector<std::vector<float>> signals;
    signals.push_back({1.f, 1.f, 1.f, 1.f, 1.f, 1.f}); // even
    signals.push_back({1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f}); // odd

    // Window types to test
    std::vector<Windows::Type> windowTypes = {
        Windows::Type::RECT,
        Windows::Type::TRIANG,
        Windows::Type::HANN,
        Windows::Type::HAMMING,
        Windows::Type::BLACKMAN
    };


    std::vector<float> windowBlock;

    for(int i=0; i<signals.size(); ++i)
    {
        const std::vector<float>& signal = signals[i];
        int size = signal.size();

        std::cout << "Audio signal size: " << signal.size() << "\n";

        for (size_t w = 0; w < windowTypes.size(); ++w)
        {
            std::cout << "Window Type: " << static_cast<int>(windowTypes[w]) << "\n";

            std::cout << "Windowed signal symmetric: ";
            Windows::window<float>(size, windowTypes[w], windowBlock, false);
            for(int s=0; s<size; ++s)
            {
                std::cout << windowBlock[s] << " ";
            }
            std::cout << "\n";

            std::cout << "Windowed signal periodic: ";
            Windows::window<float>(size, windowTypes[w], windowBlock, true);
            for(int s=0; s<size; ++s)
            {
                std::cout << windowBlock[s] << " ";
            }
            std::cout << "\n";
        }
    }

    return EXIT_SUCCESS;
}
