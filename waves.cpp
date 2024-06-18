#include <vector>
#include <cmath>
#include <iostream>
#include <sndfile.h>

std::vector<double> generateSineWave(double frequency, int length, double sample_rate) {
    std::vector<double> wave(length);
    double angular_frequency = 2.0 * M_PI * frequency;
    for (int i = 0; i < length; ++i) {
        wave[i] = sin(angular_frequency * i / sample_rate);
    }
    return wave;
}

// Custom << operator overload for vector extension
std::vector<double>& operator<<(std::vector<double>& lhs, const std::vector<double>& rhs) {
    lhs.insert(lhs.end(), rhs.begin(), rhs.end());
    return lhs;
}

struct AudioData {
    std::vector<double> samples;
    int sampleRate;
};

AudioData readAudioFile(const char* filename) {
    SNDFILE *infile;
    SF_INFO sfinfo;
    AudioData audioData;

    // Open the audio file
    infile = sf_open(filename, SFM_READ, &sfinfo);
    if (!infile) {
        std::cerr << "Error opening file: " << sf_strerror(infile) << std::endl;
        return audioData; // Empty struct
    }

    // Read the audio data
    audioData.samples.resize(sfinfo.frames);
    sf_read_double(infile, audioData.samples.data(), sfinfo.frames);

    // Get sample rate
    audioData.sampleRate = sfinfo.samplerate;

    // Close the audio file
    sf_close(infile);

    return audioData;
}
