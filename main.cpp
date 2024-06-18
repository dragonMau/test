#include "waves.cpp"
#include "gnuplot.cpp"
#include "oscillator.cpp"
#include "image.cpp"
#include <iostream>

int main() {
    std::cout << "reading file\n";
    auto audio = readAudioFile("audio.mp3");
    double sampleRate = audio.sampleRate; // Sample rate in samples per second

    std::vector<double> forceWave;
    // force_wave << generateSineWave(450.0, 44*500, sampleRate);
    audio.samples.resize(3*sampleRate);
    forceWave << audio.samples;


    ReturnDataRequest request = {1, 0, 0};

    std::cout << "simulating red\n";
    Oscillator oscR(415.3, 0.0, 60.0, sampleRate, 1U);
    ReturnData resultR = oscR.runOscillator(forceWave, request);

    std::cout << "simulating green\n";
    Oscillator oscG(523.2, 0.0, 60.0, sampleRate, 1U);
    ReturnData resultG = oscG.runOscillator(forceWave, request);

    std::cout << "simulating blue\n";
    Oscillator oscB(622.2, 0.0, 60.0, sampleRate, 1U);
    ReturnData resultB = oscB.runOscillator(forceWave, request);

    std::cout << "plotting data\n";
    Gnuplot plotter;
    plotter.plot(
        {forceWave, resultR.power, resultG.power, resultB.power},
        // {"title 'Force'",   "title 'Red'", "title 'Green'", "title 'Blue'"}
        {"lc rgb 'gray' title 'Force'", "lc rgb 'red' title 'Red'", "lc rgb 'green' title 'Green'", "lc rgb 'blue' title 'Blue'"}
    );

    std::cout << "generating image\n";
    std::vector<std::vector<double>> dataSets = {resultR.power, resultG.power, resultB.power};

    int height = 32; // Specify the height of the image
    int width = 1;    // Specify the width of each sample
    createImageFromRGB(forceWave, dataSets, height, width);

    std::cout << "done!\n";
    plotter.show();
    return 0;
}
