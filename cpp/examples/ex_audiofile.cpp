#include <stdlib.h>
#include <iostream>

#include "AudioFile.h"

/*
    Example audio file.
    Demonstration of how to read an audio file using AudioFile.
*/

/************************ PARAMS ****************************/

const std::string g_filePath = std::string(TESTSOUND_PATH);

/************************************************************/

int main(int argc, char* argv[])
{
    printf("Example audiofile...\n");

    AudioFile<float> file;
    bool loaded = file.load(g_filePath);
    assert(loaded);

    /* Printing file information */
    printf("Bit Depth: %i\n", file.getBitDepth());
    printf("Sample Rate: %i\n", file.getSampleRate());
    printf("Num Channels: %i\n", file.getNumChannels());
    printf("Length in Seconds: %.2f\n", file.getLengthInSeconds());
    
    return EXIT_SUCCESS;
}