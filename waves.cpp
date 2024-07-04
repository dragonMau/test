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


bool writeAudioFile(const char* filename, const AudioData& audioData) {
    // Open the audio file
    SF_INFO sfinfo;

    sfinfo.frames = (sf_count_t)audioData.samples.size();
    sfinfo.samplerate = audioData.sampleRate;
    sfinfo.channels = 1; // Assuming mono audio. Adjust as necessary.
    sfinfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16; // Adjust format as needed.

    SNDFILE *outfile = sf_open(filename, SFM_WRITE, &sfinfo);
    if (!outfile) {
        std::cerr << "Error opening file for writing: " << sf_strerror(outfile) << std::endl;
        return false;
    }

    // Write the audio data
    sf_count_t frames_written = sf_write_double(outfile, audioData.samples.data(), audioData.samples.size());

    // Close the audio file
    sf_close(outfile);
    std::cout << "Successfully wrote " << frames_written << " frames to the file." << std::endl;

    return true;
}


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
    audioData.sampleRate = (int)sfinfo.samplerate;

    // Close the audio file
    sf_close(infile);

    return audioData;
}

std::vector<double> slicing(std::vector<double>& arr, int X, int Y) {
    // Starting and Ending iterators
    auto start = arr.begin() + X;
    auto end = arr.begin() + Y + 1;

    // To store the sliced vector
    std::vector<double> result(Y - X + 1);
 
    // Copy vector using copy function()
    copy(start, end, result.begin());
 
    // Return the final sliced vector
    return result;
}


void generateColorWave(AudioData& aDataC, const std::vector<double>& powerWave, const double freqC) {
    const size_t samplesNumber = powerWave.size();
    std::vector<double> sinWaveC = generateSineWave(freqC, samplesNumber, aDataC.sampleRate);
    for (size_t i = 0; i < samplesNumber; ++i) {
        aDataC.samples[i] *= powerWave[i] * sinWaveC[i];
    }
}