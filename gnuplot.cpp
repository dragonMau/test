#include <iostream>
#include <vector>
#include <thread>

class Gnuplot {
public:
    Gnuplot() {
        gnuplotPipe = popen("gnuplot -persistent", "w");
        if (!gnuplotPipe) {
            std::cerr << "Error: Could not open pipe to gnuplot.\n";
        }
    }

    ~Gnuplot() {
        if (gnuplotPipe) {
            pclose(gnuplotPipe);
        }
    }

    void plot(const std::vector<std::vector<double>>& dataSets, const std::vector<std::string>& titles) {
        if (!gnuplotPipe) return;

        fprintf(gnuplotPipe, "set title 'Sine Wave'\n");
        fprintf(gnuplotPipe, "set xlabel 'Sample Number'\n");
        fprintf(gnuplotPipe, "set ylabel 'Amplitude'\n");
        fprintf(gnuplotPipe, "set grid\n");

        // Construct the plot command with multiple datasets
        fprintf(gnuplotPipe, "plot");
        for (size_t i = 0; i < dataSets.size(); ++i) {
            if (i > 0) fprintf(gnuplotPipe, ",");
            fprintf(gnuplotPipe, " '-' with filledcurves y1=0 %s", titles[i].c_str());
        }
        fprintf(gnuplotPipe, "\n");

        // Send the data for each dataset
        for (const auto& dataSet : dataSets) {
            for (size_t i = 0; i < dataSet.size(); ++i) {
                fprintf(gnuplotPipe, "%zu %f\n", i, dataSet[i]);
            }
            fprintf(gnuplotPipe, "e\n");
        }

        fflush(gnuplotPipe);
    }

    void show() {
        if (gnuplotPipe) {
            // Keep the program running to maintain the gnuplot window
            std::cout << "Press Enter to close the plot..." << std::endl;
            std::cin.get();
        }
    }

private:
    FILE* gnuplotPipe = nullptr;
};
