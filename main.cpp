#include "waves.cpp"
#include "gnuplot.cpp"
#include "oscillator.cpp"
#include "image.cpp"
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

int main() {
    std::ifstream config_file("config.json");
    json config;
    config_file >> config;
    
    const bool plot = config["plot"];
    const char *filename = config["filename"].get_ref<const std::string&>().c_str();
    const float start = config["start"];
    const float end = config["end"];
    const int height = config["height"];
    const int width = config["width"];
    const double freqR = config["freqR"];
    const double freqG = config["freqG"];
    const double freqB = config["freqB"];
    const double s_torq = config["s_torq"];
    const uint8_t iters = config["iters"];
    
    std::cout << "reading file\n";
    AudioData audio = readAudioFile(filename);
    int sampleRate = audio.sampleRate;

    const std::vector<double> forceWave = slicing(audio.samples, start*sampleRate, end*sampleRate);
    const size_t samplesNumber = forceWave.size();

    ReturnDataRequest request = {1, 0, 0};

    std::cout << "simulating red\n"; // make in 3 threads
    Oscillator oscR(freqR, 0.0, s_torq, sampleRate, iters);
    ReturnData resultR = oscR.runOscillator(forceWave, request);

    std::cout << "simulating green\n"; // make in 3 threads
    Oscillator oscG(freqG, 0.0, s_torq, sampleRate, iters);
    ReturnData resultG = oscG.runOscillator(forceWave, request);

    std::cout << "simulating blue\n"; // make in 3 threads
    Oscillator oscB(freqB, 0.0, s_torq, sampleRate, iters);
    ReturnData resultB = oscB.runOscillator(forceWave, request);


    Gnuplot plotter;
    if (plot) {
        std::cout << "plotting data\n";
        plotter.plot(
            {forceWave, resultR.power, resultG.power, resultB.power},
            {"lc rgb 'gray' title 'Force'", "lc rgb 'red' title 'Red'", "lc rgb 'green' title 'Green'", "lc rgb 'blue' title 'Blue'"}
        );
    }

    std::cout << "generating image\n";
    std::vector<std::vector<double>> dataSets = {resultR.power, resultG.power, resultB.power};
    createImageFromRGB(forceWave, dataSets, height, width);

    std::cout << "genrating sound files red\n";
    AudioData aDataR = {std::vector<double>(samplesNumber, 0.0), sampleRate};
    generateColorWave(aDataR, resultR.power, freqR, sampleRate);
    writeAudioFile("audio_r.wav", aDataR);

    std::cout << "genrating sound files green\n";
    AudioData aDataG = {std::vector<double>(samplesNumber, 0.0), sampleRate};
    generateColorWave(aDataG, resultG.power, freqG, sampleRate);
    writeAudioFile("audio_g.wav", aDataG);

    std::cout << "genrating sound files blue\n";
    AudioData aDataB = {std::vector<double>(samplesNumber, 0.0), sampleRate};
    generateColorWave(aDataB, resultB.power, freqB, sampleRate);
    writeAudioFile("audio_b.wav", aDataB);

    std::cout << "generating sound files white\n";
    // std::vector<double> aWaveW(samplesNumber, 0.0);
    AudioData aDataW = {std::vector<double>(samplesNumber, 0.0), sampleRate};
    for (size_t i = 0; i < samplesNumber; ++i) {
        aDataW.samples[i] = aDataR.samples[i] + aDataG.samples[i] + aDataB.samples[i];
    }
    writeAudioFile("audio_w.wav", aDataW);



    std::cout << "done!\n";
    if (plot) {plotter.show();}
    return 0;
}
