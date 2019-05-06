//
// demo.cc
//
// Author: David Robinson
// Date: 05/06/19
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
    std::cerr << "USAGE: ./main -i <path-to-image> -w <%-width> -h <%-height> -v <verbose-flag>" << std::endl;
}

int main(int argc, char *argv[])
{
    int option = -1;
    float width = 1.0f;
    float height = 1.0f;
    bool verbose = false;
    std::string image_path;

    while ((option = getopt (argc, argv, "i:w:h:v")) != -1)
    {
        switch (option)
        {
            case 'i':
                image_path = std::string(optarg);
                break;
            case 'w':
                width = atof(optarg);
                break;
            case 'h':
                height = atof(optarg);
                break;
            case 'v':
                verbose = true;
                break;
            default:
                break;
        }
    }

    if (image_path.empty())
    {
        print_usage();
        return -1;
    }
    else if (width == 1.0f && height == 1.0f)
    {
        std::cerr << "WARNING: No width or height change specified" << std::endl;
    }

    // Load original image and check that it's valid
    cv::Mat image = cv::imread(image_path, cv::IMREAD_COLOR);

    if(!image.data)
    {
        std::cout <<  "Could not open or find the image" << std::endl;
        print_usage();
        return -1;
    }

    // Construct image retargeter
    const auto seam_carving = std::make_unique<SeamCarving>(image);

    // Generate retargeted image
    const int target_rows = image.rows * height;
    const int target_cols = image.cols * width;
    seam_carving->retarget(target_rows, target_cols);

    // Display results
    cv::imshow("Original Image", image);
    cv::imshow("Retargeted Image", seam_carving->getImage());

    if (verbose)
    {
        seam_carving->printReport();
        cv::imshow("Minimum Cost Seams", seam_carving->getSeamImage());
        cv::imshow("Energy Map", seam_carving->getEnergyMap());
    }

    cv::waitKey(0);

    return 0;
}