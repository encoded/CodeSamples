# CPP

This folder contains a collection of C++ examples related to audio processing.

## Folder Structure

The structure of this folder is as follows (some of the folders are added after installing the dependencies or building):
- **build/**: Contains built executables for the examples.
- **examples/**: Contains examples demonstrating various C++ features and concepts.
- **external/**: Contains external include files.
- **libs/**: Contains external libraries used in the projects such as portaudio.
- **include/**: Contains header files which are the main source of code for the examples.
- **resources/**: Contains resources (audio, img, etc.) used in the examples. 
- **src/**: Contains source files.

For simplicity of compilation, and considering the provided code is simply for demonstration, code has been added entirely in the header files.

## Dependencies

This project requires the following libraries:

- [PortAudio](http://www.portaudio.com/): Portable open-source audio input/output library.
- [AudioFile](https://github.com/adamstark/AudioFile): A simple C++ library to read and write audio files by Adam Stark.

## Build

### Install Dependecies

In order to install the required dependecies you can run the following command:
```bash
make install
```

You may need to install the following prerequisites based on your platform before running the installation.

#### MacOS

PortAudio dependecies (check [here](https://portaudio.com/docs/v19-doxydocs/compile_mac_coreaudio.html)):
```bash
-framework CoreAudio -framework AudioToolbox -framework AudioUnit -framework CoreServices -framework Carbon
```

#### Windows

*To Do: Add prerequisites for Windows.*

#### Linux

PortAudio dependencies [here](https://portaudio.com/docs/v19-doxydocs/compile_linux.html).
```bash
-lrt -lm -lasound -ljack -pthread
```

## Build Instructions

This project uses `make` to build its executables. 
Below are the steps and options for building the project.

### Build and Running an Example

Examples can be found in the "examples" folder.
Before each example, there are explanations and a set of global parameters that can be adjusted for demonstration purposes.

Given an example, such as "ex_gameaudio", you can run the following make command to build and run the executable:
```bash
make ex_gameaudio && ./build/ex_gameaudio
```

Recommended examples:
- **ex_gameaudio**: a demonstration of how sounds might get played in a game.
- **ex_granularsynth_random**: a demonstration of a granular synth.

### Configuration

By default, examples are built in debug mode to facilitate output logging. To build the project in release mode, specify "CONFIG=release" during the build process:
```bash
make ex_gameaudio CONFIG=release
```
It's recommended to clean old build artifacts when switching configurations.

### Cleaning Build Artifacts
```bash
make clean
```

## Feedback

Any feedback is greatly appreciated!

## Note

- **Linux and Windows Environment:** This project has primarily been developed and tested on MacOS. While efforts have been made to include necessary dependencies and commands for Linux and Windows in the Makefile, these environments haven't been fully tested. If you encounter any issues or discrepancies, please let me know!
