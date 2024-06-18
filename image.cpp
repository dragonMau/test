#include <opencv2/opencv.hpp>
#include <vector>
#include <algorithm>

void createImageFromRGB(std::vector<double>& wave, const std::vector<std::vector<double>>& dataSets, int height, int width) {
    cv::Mat image(height, width * dataSets[0].size(), CV_8UC3);
    
    double max_element = std::numeric_limits<double>::min();
    for (const auto& dataSet : dataSets) {
        for (const auto& value : dataSet) {
            max_element = std::max(max_element, value);
        }
    }

    for (size_t i = 0; i < dataSets[0].size(); ++i) {
        cv::Vec3b color;

        for (size_t j = 0; j < dataSets.size(); ++j) {
            double normalizedValue = dataSets[j][i] / max_element;
            int colorValue = static_cast<int>(normalizedValue * 256);
            color[j] = colorValue; // Assign the color value to the corresponding channel (R, G, B)
        }

        for (int k = 0; k < height; ++k) {
            // image.at<cv::Vec3b>(i, j * width + k) = color;
            image.at<cv::Vec3b>(k, i) = color;
        }

        // /* debug
        std::cout << "i: " << +i << " wave[60162] = " << wave[60162] << "\n";
        if (wave[60162] > 1) {
            std::cout << "wtf\n";
        }
        if (i == 20855) {
            std::cout << "here\n";
        }
        // debug */

        int adjusted_value = (1-(wave[i]-.5))*height;
        image.at<cv::Vec3b>(adjusted_value, i) = {127U, 127U, 127U};
    }

    cv::imwrite("output_image.png", image);
}

