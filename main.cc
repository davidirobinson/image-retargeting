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

    // Create iterator for argmin computations
    cv::MatConstIterator_<uchar> prev_row_iter = energy_cumulative.begin<uchar>();
    
    // Begin with second row
    for (size_t row = 1; row < energy.rows; ++row)
    {
        uchar* energy_cumulative_ptr = energy_cumulative.ptr<uchar>(row);
        uchar* backtrack_ptr = energy_cumulative.ptr<uchar>(row);

        std::cout << "\n\n\n\n\nBegin\n\n\n\n\n" << std::endl;
        for (size_t col = 0; col < energy.cols; ++col)
        {
            // Deal with first and last column
            size_t prev_col_range = (col == 0 ? 0 : 1);
            size_t post_col_range = (col == energy.cols-1 ? 0 : 1);

            // Get idx of min energy from previous row, on 8-connected cols  
            const auto result = std::min_element(
                prev_row_iter - prev_col_range - 1, prev_row_iter + post_col_range);
            const auto idx = std::distance(prev_row_iter - prev_col_range - 1, result);

            std::cout << "idx: " << idx << " result " << int(*result);
            std::cout 
                << "\nbefore: " << int(*(prev_row_iter - prev_col_range)) 
                << " curr: " << int(*prev_row_iter) 
                << " ahead: " << int(*(prev_row_iter + post_col_range)) << std::endl;
 
            for (auto it = prev_row_iter - prev_col_range - 1; it != prev_row_iter + post_col_range; ++it)
            {
                std::cout << int(*it) << " ";
            }
            std::cout << " test " << std::endl;

            backtrack_ptr[col] = idx + col - prev_col_range;
            energy_cumulative_ptr[col] += *result;

            prev_row_iter++;
        }
    }
}

void seamCarveCol(cv::Mat &input)
{
    const auto energy = computeEnergy(input);
    cv::imshow("grad", energy);
    cv::waitKey(0);

    cv::Mat energy_cumulative, backtrack;
    minSeam(energy, energy_cumulative, backtrack);
}

cv::Mat retargetImg(const cv::Mat &input, const int rows, const int cols)
{
    auto ret = input.clone();
    const auto d_rows = rows - ret.rows;
    const auto d_cols = cols - ret.cols;

    std::cout << d_rows << " " << d_cols << std::endl;

    // Apply change to cols
    if (d_cols < 0)
    {
        for (int i = 0; i < abs(d_cols); ++i)
            seamCarveCol(ret);
    }
    else if (d_cols > 0)
    {
        // TODO
    }

    // Apply change to cols
    // TODO

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

    cv::Mat img = cv::imread(img_path, cv::IMREAD_COLOR);

    int target_rows = img.rows * height;
    int target_cols = img.cols * width;

    cv::Mat retargeted_img = retargetImg(img, target_rows, target_cols);
    
    // // cv::namedWindow("img", cv::WINDOW_NORMAL);
    // cv::imshow("img", img);
    // cv::imshow("retargeted", retargeted_img);
    // cv::waitKey(0);

    // while (cv::waitKey(30) != 'q')
    // {    
    //     std::cout << "loop" << std::endl;
    //     img -= cv::Scalar(1,1,1);
    //     cv::imshow("img", img);
    // }
    // cv::destroyWindow("img");
    return 0;
}