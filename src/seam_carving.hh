//
// seam_carving.cc
//
// Author: David Robinson
// Date: 05/06/19
//

#pragma once

#include <chrono>
#include <opencv2/opencv.hpp>

class SeamCarving
{
public:
    explicit SeamCarving(const cv::Mat &_image);

    void retarget(const int target_rows, const int target_cols);

    void printReport();

    cv::Mat getImage()
    { return m_output_image; }

    cv::Mat getEnergyMap()
    { return m_energy_image; }

    cv::Mat getSeamImage()
    { return m_seam_image; }

private:

    void computeEnergy(const cv::Mat &input, cv::Mat &energy);

    void computeMinSeam(
        const cv::Mat &energy,
        cv::Mat &energy_cumulative,
        cv::Mat &backtrack);

    void carveSeam(
        cv::Mat &input,
        cv::Mat &output,
        cv::Mat &seam,
        const cv::Mat &energy_cumulative,
        const cv::Mat &backtrack);

    void removeMinEnergyColumn(cv::Mat &input);

    cv::Mat m_original_image;
    cv::Mat m_output_image;
    cv::Mat m_seam_image;
    cv::Mat m_energy_image;
};
