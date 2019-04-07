//
// Author: David Robinson
// Date: 04/07/19
//

#include <string>
#include <vector>
#include <unistd.h>
#include <iostream>
#include <algorithm>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

void print_usage()
{
    std::cout << "USAGE: ./main -i <path-to-image> -w <%-width> -h <%-height>" << std::endl;
}

cv::Mat computeEnergy(const cv::Mat &input)
{
    cv::Mat filtered;
    cv::cvtColor(input, filtered, cv::COLOR_BGR2GRAY);
    cv::GaussianBlur(filtered, filtered, cv::Size(5, 5), 0, 0);
    
    cv::Mat dx, dy;
    cv::Sobel(filtered, dx, CV_16S, 1, 0, 3);
    cv::Sobel(filtered, dy, CV_16S, 0, 1, 3);

    cv::Mat dx_abs, dy_abs;
    cv::convertScaleAbs(dx, dx_abs);
    cv::convertScaleAbs(dy, dy_abs);
    
    cv::Mat energy;
    cv::addWeighted(dx_abs, 0.5, dy_abs, 0.5, 0, energy);
    return energy;
}

void minSeam( 
    const cv::Mat &energy, 
    cv::Mat &energy_cumulative,
    cv::Mat &backtrack)
{
    energy_cumulative = energy.clone();
    backtrack = cv::Mat::zeros(energy.size(), CV_8U);
    
    auto prev_row_iter = energy_cumulative.begin<uchar>();
    
    for (size_t row = 1; row < energy.rows; ++row)
    {
        auto energy_cumulative_ptr = energy_cumulative.ptr<uchar>(row);
        auto backtrack_ptr = energy_cumulative.ptr<uchar>(row);

        for (size_t col = 0; col < energy.cols; ++col)
        {
            // Deal with first and last column
            size_t prev_col_range = (col == 0 ? 0 : 1);
            size_t post_col_range = (col == energy.cols-1 ? 0 : 1);

            // Get idx of min energy from previous row, on 8-connected cols  
            const auto result = std::min_element(
                prev_row_iter - prev_col_range - 1, prev_row_iter + post_col_range);
            const auto idx = std::distance(prev_row_iter - prev_col_range - 1, result);

            backtrack_ptr[col] = idx + col - prev_col_range;
            energy_cumulative_ptr[col] += *result;
            prev_row_iter++;
        }
    }
}

void carveSeam(
    cv::Mat &input, 
    const cv::Mat &energy_cumulative,
    const cv::Mat &backtrack)
{
    cv::Mat output;
    // TODO
}

void seamCarveCol(cv::Mat &input)
{
    const auto energy = computeEnergy(input);
    cv::Mat energy_cumulative, backtrack;
    minSeam(energy, energy_cumulative, backtrack);
    carveSeam(input, energy_cumulative, backtrack);
}

cv::Mat retargetImg(const cv::Mat &input, const int rows, const int cols)
{
    auto ret = input.clone();
    const auto d_rows = rows - ret.rows;
    const auto d_cols = cols - ret.cols;

    if (d_cols < 0)
    {
        for (int i = 0; i < abs(d_cols); ++i)
            seamCarveCol(ret);
    }
    else if (d_cols > 0)
    {
        // TODO
    }
    if (d_rows < 0)
    {
        // TODO
    }
    else if (d_rows > 0)
    {
        // TODO
    }
    return ret;
}

int main(int argc, char *argv[])
{
    int option = -1;
    float width = 1.0f;
    float height = 1.0f;
    std::string img_path;

    while ((option = getopt (argc, argv, "i:w:h:")) != -1)
    {
        switch (option)
        {
            case 'i':
                img_path = std::string(optarg);
                break;
            case 'w':
                width = atof(optarg);
                break;
            case 'h':
                height = atof(optarg);
                break;  
            default:
                break;
        }
    }

    if (img_path.empty())
    {
        print_usage(); 
        return 1;
    }

    // Load original image
    cv::Mat img = cv::imread(img_path, cv::IMREAD_COLOR);

    // Generate retargeted image
    int target_rows = img.rows * height;
    int target_cols = img.cols * width;
    cv::Mat retargeted_img = retargetImg(img, target_rows, target_cols);
    
    // Display results
    cv::imshow("img", img);
    cv::imshow("retargeted", retargeted_img);
    cv::waitKey(0);

    return 0;
}