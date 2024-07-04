#include <opencv2/opencv.hpp>
#include <vector>
#include <algorithm>


void createImageFromRGB(const std::vector<double>& wave, const std::vector<std::vector<double>>& dataSets, int height, int width) {
    const cv::Vec3b gray = {127U, 127U, 127U};
    cv::Mat image(height, width * dataSets[0].size(), CV_8UC3);
    
    double max_element = std::numeric_limits<double>::min();
    for (const auto& dataSet : dataSets) {
        for (const auto& value : dataSet) {
            max_element = std::max(max_element, value);
        }
    }
    uint previous_value, k, a, b;
    uint adjusted_value = height/2;

    for (size_t i = 0; i < dataSets[0].size(); ++i) {
        cv::Vec3b color;

        for (size_t j = 0; j < dataSets.size(); ++j) {
            double normalizedValue = dataSets[j][i] / max_element;
            int colorValue = static_cast<int>(normalizedValue * 256);
            color[j] = colorValue; // Assign the color value to the corresponding channel (R, G, B)
        }
        
        previous_value = adjusted_value;
        adjusted_value = (1-((wave[i]/2)+.5))*(height-1);
        if  (height < adjusted_value) {
            std::cerr << adjusted_value << " is out of bounds\n";
            exit(1);
        }
        a = adjusted_value < previous_value ? adjusted_value : previous_value;
        b = adjusted_value > previous_value ? adjusted_value : previous_value;
        k = 0;
        do { image.at<cv::Vec3b>(k++, i) = color;
        } while (k < a);
        do { image.at<cv::Vec3b>(k++, i) = gray;
        } while (k < b);
        do { image.at<cv::Vec3b>(k++, i) = color;
        } while (k < height);
    }

    cv::imwrite("out/output_image.png", image);
}

