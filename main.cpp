#include <iostream>
#include <cmath>
#include <RtAudio.h>

// Audio settings
constexpr unsigned int sampleRate = 44100;
constexpr unsigned int bufferFrames = 256;
constexpr double frequency = 440.0; // A4 note (440 Hz)

// Sine wave generation variables
double phase = 0.0;
constexpr double twoPi = 6.28318530718;

// Audio callback function
int callback(void *outputBuffer, void * /*inputBuffer*/, unsigned int nFrames,
             double /*streamTime*/, RtAudioStreamStatus status, void * /*userData*/) {
    if (status) std::cerr << "Stream underflow detected!" << std::endl;

    float *buffer = static_cast<float *>(outputBuffer);
    double phaseIncrement = twoPi * frequency / sampleRate;

    for (unsigned int i = 0; i < nFrames; i++) {
        float sample = static_cast<float>(std::sin(phase)); // Generate sine wave sample
        phase += phaseIncrement;
        if (phase >= twoPi) phase -= twoPi;

        // Write to both left and right channels
        buffer[2 * i] = sample;     // Left channel
        buffer[2 * i + 1] = sample; // Right channel
    }

    return 0;
}

int main() {
    RtAudio audio;
    if (audio.getDeviceCount() < 1) {
        std::cerr << "No audio devices found!" << std::endl;
        return 1;
    }

    RtAudio::StreamParameters parameters;
    parameters.deviceId = audio.getDefaultOutputDevice();
    parameters.nChannels = 2;  // Stereo output
    parameters.firstChannel = 0;

    try {
        unsigned int bufferSize = bufferFrames;
        audio.openStream(&parameters, nullptr, RTAUDIO_FLOAT32, sampleRate, &bufferSize, &callback);
        audio.startStream();

        std::cout << "Press Enter to stop..." << std::endl;
        std::cin.get();

        audio.stopStream();
        audio.closeStream();
    } catch (const std::exception &e) {
        std::cerr << "Exception caught: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Unknown exception caught!" << std::endl;
    }

    return 0;
}
