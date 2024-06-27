# Detect operating system
UNAME_S := $(shell uname -s)

# Resources
RESOURCES_PATH = ../resources
TESTSOUND_PATH = ../resources/audio/44.1/shot.wav
DRESOURCES = -DRESOURCES_PATH=\"$(RESOURCES_PATH)\" -DTESTSOUND_PATH=\"$(TESTSOUND_PATH)\"

# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -I$(IDIR) $(DRESOURCES)

# Debug and Release flags
DEBUGFLAGS = -DDEBUG -g # we define a preprocessor macro "DEBUG" and include debugging information
RELEASEFLAGS = -DNDEBUG -O2 # we disable debug assertions, debug specific code and set optimization level 2

# Default cofiguration is debug but you can pass CONFIG=release to override this
CONFIG ?= debug

ifeq ($(CONFIG), debug)
    CXXFLAGS += $(DEBUGFLAGS)
else ifeq ($(CONFIG), release)
	CXXFLAGS += $(RELEASEFLAGS)	
endif

# Source and object directories
SRCDIR = src
IDIR = include
BUILDDIR = build

# Targets for different examples
TARGET_MAIN = $(BUILDDIR)/main #target for main
TARGET_EX_SOUNDENGINE = $(BUILDDIR)/ex_soundengine
TARGET_EX_TASKQUEUE = $(BUILDDIR)/ex_taskqueue
TARGET_EX_GRANULARSYNTH = $(BUILDDIR)/ex_granularsynth
TARGET_EX_GRANULARSYNTH_RANDOM = $(BUILDDIR)/ex_granularsynth_random
TARGET_EX_PORTAUDIO = $(BUILDDIR)/ex_portaudio
TARGET_EX_AUDIOFILE = $(BUILDDIR)/ex_audiofile
TARGET_EX_PORTAUDIO_WHITENOISE = $(BUILDDIR)/ex_portaudio_whitenoise
TARGET_EX_PORTAUDIO_SINE = $(BUILDDIR)/ex_portaudio_sine
TARGET_EX_PORTAUDIO_SOUND = $(BUILDDIR)/ex_portaudio_sound
TARGET_EX_AUDIOPLAYER = $(BUILDDIR)/ex_audioplayer
TARGET_EX_ANALYSISWINDOW = $(BUILDDIR)/ex_analysiswindow
TARGET_EX_GAMEAUDIO = $(BUILDDIR)/ex_gameaudio
TARGET_ALL = $(TARGET_MAIN) $(TARGET_EX_SOUNDENGINE) $(TARGET_EX_TASKQUEUE) $(TARGET_EX_GRANULARSYNTH) $(TARGET_EX_GRANULARSYNTH_RANDOM) $(TARGET_EX_PORTAUDIO) $(TARGET_EX_PORTAUDIO_WHITENOISE) $(TARGET_EX_PORTAUDIO_SOUND) $(TARGET_EX_PORTAUDIO_SINE) $(TARGET_EX_AUDIOPLAYER) $(TARGET_EX_ANALYSISWINDOW) $(TARGET_EX_GAMEAUDIO)

######################## RULES ######################

# Phony targets
.PHONY: all clean install install-portaudio uninstall-portaudio main ex_soundengine ex_taskqueue ex_granularsynth ex_granularsynth_random ex_portaudio ex_audiofile ex_portaudio_whitenoise ex_portaudio_sine ex_portaudio_sound ex_audioplayer ex_analysiswindow

# Default target
all: $(TARGET_ALL)

# Clean
clean:
	rm -rf $(BUILDDIR) $(TARGET)

# Rules to build executables
main: $(TARGET_MAIN)
ex_soundengine: $(TARGET_EX_SOUNDENGINE)
ex_taskqueue: $(TARGET_EX_TASKQUEUE)
ex_granularsynth: $(TARGET_EX_GRANULARSYNTH)
ex_granularsynth_random: $(TARGET_EX_GRANULARSYNTH_RANDOM)
ex_portaudio: $(TARGET_EX_PORTAUDIO)
ex_audiofile: $(TARGET_EX_AUDIOFILE)
ex_portaudio_whitenoise: $(TARGET_EX_PORTAUDIO_WHITENOISE)
ex_portaudio_sine: $(TARGET_EX_PORTAUDIO_SINE)
ex_portaudio_sound: $(TARGET_EX_PORTAUDIO_SOUND)
ex_audioplayer: $(TARGET_EX_AUDIOPLAYER)
ex_analysiswindow: $(TARGET_EX_ANALYSISWINDOW)
ex_gameaudio: $(TARGET_EX_GAMEAUDIO)

############## BUILD AND LINK RULES ###############

# Default target - used for quick testing
$(TARGET_MAIN): $(SRCDIR)/main.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -o $@ $<

# Other targets
$(TARGET_EX_SOUNDENGINE): examples/ex_soundengine.cpp $(IDIR)/SoundEngine.h $(IDIR)/AudioDevice.h $(IDIR)/RingBuffer.h
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -o $@ $<	

$(TARGET_EX_TASKQUEUE): examples/ex_taskqueue.cpp $(IDIR)/TaskQueue.h
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -o $@ $<	

$(TARGET_EX_GRANULARSYNTH): examples/ex_granularsynth.cpp $(IDIR)/GranularSynth.h $(IDIR)/PaWrapper.h
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -o $@ $< $(IPORTAUDIO) $(IAUDIOFILE) $(PORTAUDIO_DEPS) $(PORTAUDIO_LIB)

$(TARGET_EX_GRANULARSYNTH_RANDOM): examples/ex_granularsynth_random.cpp $(IDIR)/GranularSynth.h $(IDIR)/PaWrapper.h
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -o $@ $< $(IPORTAUDIO) $(IAUDIOFILE) $(PORTAUDIO_DEPS) $(PORTAUDIO_LIB)

$(TARGET_EX_PORTAUDIO): examples/ex_portaudio.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -o $@ $< $(IPORTAUDIO) $(PORTAUDIO_DEPS) $(PORTAUDIO_LIB)	

$(TARGET_EX_AUDIOFILE): examples/ex_audiofile.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -o $@ $< $(IAUDIOFILE)	

$(TARGET_EX_PORTAUDIO_WHITENOISE): examples/ex_portaudio_whitenoise.cpp $(IDIR)/RandomUtils.h
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -o $@ $< $(IPORTAUDIO) $(PORTAUDIO_DEPS) $(PORTAUDIO_LIB)	

$(TARGET_EX_PORTAUDIO_SINE): examples/ex_portaudio_sine.cpp $(IDIR)/SineGenerator.h
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -o $@ $< $(IPORTAUDIO) $(PORTAUDIO_DEPS) $(PORTAUDIO_LIB)	

$(TARGET_EX_PORTAUDIO_SOUND): examples/ex_portaudio_sound.cpp $(IDIR)/SineGenerator.h $(IDIR)/Sound.h $(IDIR)/PaWrapper.h
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -o $@ $< $(IPORTAUDIO) $(PORTAUDIO_DEPS) $(PORTAUDIO_LIB)	

$(TARGET_EX_AUDIOPLAYER): examples/ex_audioplayer.cpp $(IDIR)/PaWrapper.h $(IDIR)/AudioPlayer.h
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -o $@ $< $(IPORTAUDIO) $(IAUDIOFILE) $(PORTAUDIO_DEPS) $(PORTAUDIO_LIB)	

$(TARGET_EX_ANALYSISWINDOW): examples/ex_analysiswindow.cpp $(IDIR)/AudioSignalUtils.h
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -o $@ $<

$(TARGET_EX_GAMEAUDIO): examples/ex_gameaudio.cpp $(IDIR)/Sound.h $(IDIR)/Transport.h $(IDIR)/PaSoundEngine.h $(IDIR)/TaskQueue.h $(IDIR)/LogMutex.h
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -o $@ $< $(IPORTAUDIO) $(IAUDIOFILE) $(PORTAUDIO_DEPS) $(PORTAUDIO_LIB)	


############## COMMANDS TO INSTALL/UNINSTALL DEPENDENCIES ###############

# Combined install target
install: install-portaudio install-audiofile
uninstall: uninstall-portaudio uninstall-audiofile

#### PORTAUDIO ####
# We're copying portaudio in the lib folder: "curl" is used so that we pass the file directly to "tar" without having to save it.
# We then run the installation steps required for portaudio: for macos https://portaudio.com/docs/v19-doxydocs/compile_mac_coreaudio.html
#
# !Because -Werror is set in the compilation of portaudio and therea are multiple variables which are not used, the compilation fails.
# Therefore in order to build portaudio correctly you may need to run ./configure first, remove the flag -Werror, and then run "$(MAKE) -j"
# Alternatively building using "--disable-mac-universal" solves the issue!

PORTAUDIO_URL = https://files.portaudio.com/archives/pa_stable_v190700_20210406.tgz
PORTAUDIO_DIR = libs/portaudio
IPORTAUDIO = -I./$(PORTAUDIO_DIR)/include #include path for portaudio
PORTAUDIO_LIB = ./$(PORTAUDIO_DIR)/lib/.libs/libportaudio.a #portaudio library
PORTAUDIO_DEPS = 

INSTALL_PORTAUDIO_CMD = mkdir -p libs && curl $(PORTAUDIO_URL) | tar -zx -C libs && cd $(PORTAUDIO_DIR) && ./configure && $(MAKE) -j
UNINSTALL_PORTAUDIO_CMD = cd $(PORTAUDIO_DIR) && $(MAKE) uninstall && rm -rf $(PORTAUDIO_DIR)

# OS-specific installation steps for PortAudio
ifeq ($(UNAME_S),Linux)
    INSTALL_PORTAUDIO_CMD = mkdir -p libs && curl $(PORTAUDIO_URL) | tar -zx -C libs && cd $(PORTAUDIO_DIR) && ./configure && $(MAKE) -j
    PORTAUDIO_LIB = ./$(PORTAUDIO_DIR)/lib/.libs/libportaudio.a
	PORTAUDIO_DEPS += -lrt -lm -lasound -ljack -pthread
endif

ifeq ($(UNAME_S),Darwin)
    INSTALL_PORTAUDIO_CMD = mkdir -p libs && curl $(PORTAUDIO_URL) | tar -zx -C libs && cd $(PORTAUDIO_DIR) && ./configure --disable-mac-universal && $(MAKE) -j
    PORTAUDIO_LIB = ./$(PORTAUDIO_DIR)/lib/.libs/libportaudio.a
	PORTAUDIO_DEPS += -framework CoreAudio -framework AudioToolbox -framework AudioUnit -framework CoreServices -framework Carbon
endif

ifeq ($(OS),Windows_NT)
    INSTALL_PORTAUDIO_CMD = if not exist libs mkdir libs && curl -L $(PORTAUDIO_URL) -o pa_stable_v190700_20210406.tgz && tar -zx -C libs -f pa_stable_v190700_20210406.tgz && cd $(PORTAUDIO_DIR) && ./configure && $(MAKE) -j
    UNINSTALL_PORTAUDIO_CMD = cd $(PORTAUDIO_DIR) && $(MAKE) uninstall && rmdir /S /Q $(PORTAUDIO_DIR)
    PORTAUDIO_LIB = # to add
	PORTAUDIO_DEPS += # add
endif

install-portaudio:
	mkdir -p libs
	curl $(PORTAUDIO_URL) | tar -zx -C libs
	cd $(PORTAUDIO_DIR) && ./configure --disable-mac-universal && $(MAKE) -j 

uninstall-portaudio:
	cd $(PORTAUDIO_DIR) && $(MAKE) uninstall
	rm -rf $(PORTAUDIO_DIR)

#### AUDIOFILE ####
# Minimal header to read audio file of format wav and aiff: https://github.com/adamstark/AudioFile/tree/master

# Default install and uninstall commands
AUDIOFILE_DIR = external/audiofile
IAUDIOFILE = -I./$(AUDIOFILE_DIR) #include path for audiofile
INSTALL_AUDIOFILE_CMD = mkdir -p external && git clone https://github.com/adamstark/AudioFile $(AUDIOFILE_DIR)
UNINSTALL_AUDIOFILE_CMD = rm -rf $(AUDIOFILE_DIR)

# OS-specific installation steps
ifeq ($(UNAME_S),Linux)
    INSTALL_AUDIOFILE_CMD = mkdir -p external && git clone https://github.com/adamstark/AudioFile $(AUDIOFILE_DIR)
endif

ifeq ($(UNAME_S),Darwin)
    INSTALL_AUDIOFILE_CMD = mkdir -p external && git clone https://github.com/adamstark/AudioFile $(AUDIOFILE_DIR)
endif

ifeq ($(OS),Windows_NT)
    INSTALL_AUDIOFILE_CMD = if not exist external mkdir external && git clone https://github.com/adamstark/AudioFile $(AUDIOFILE_DIR)
    UNINSTALL_AUDIOFILE_CMD = rmdir /S /Q $(AUDIOFILE_DIR)
endif

install-audiofile:
	$(INSTALL_AUDIOFILE_CMD)

uninstall-audiofile:
	$(UNINSTALL_AUDIOFILE_CMD)

################################################################################

# Notes
# $@: expands to the target of the rule, such as build/code_samples.
# $^: expands to prerequisites (dependencies) of the rule such as build/main.o, etc.
# $<: expands to the first prerequisite (the source file in the compile rule).

# Utility debug print used when needed:
# debug-print:
#     $(info UNAME_S is $(UNAME_S))
#     $(info OS is $(OS))
