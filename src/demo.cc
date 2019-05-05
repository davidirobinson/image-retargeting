//
// Author: David Robinson
// Date: 04/07/19
//

#include <string>
#include <vector>
#include <chrono>
#include <unistd.h>
#include <iostream>
#include <algorithm>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

#include <seam_carving.hh>

void print_usage()
{
    std::cerr << "USAGE: ./main -i <path-to-image> -w <%-width> -h <%-height>" << std::endl;
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
        return -1;
    }
    else if (width == 1.0f && height == 1.0f)
    {
        std::cerr << "WARNING: No width or height change specified" << std::endl;
    }

    // Load original image and check that it's valid
    cv::Mat img = cv::imread(img_path, cv::IMREAD_COLOR);

    if(!img.data)
    {
        std::cout <<  "Could not open or find the image" << std::endl;
        print_usage();
        return -1;
    }

    // Construct image retargeter
    const auto seam_carving = std::make_unique<SeamCarving>(img);

    // Generate retargeted image
    const int target_rows = img.rows * height;
    const int target_cols = img.cols * width;
    seam_carving->retarget(target_rows, target_cols);

    // Display results
    cv::imshow("original", img);
    cv::imshow("retargeted", seam_carving->getImage());
    cv::waitKey(0);

    return 0;
}