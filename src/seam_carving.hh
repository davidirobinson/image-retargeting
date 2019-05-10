//
// seam_carving.hh
//
// Author: David Robinson
// Date: 05/06/19
//

#pragma once

#include <chrono>
#include <numeric>
#include <opencv2/opencv.hpp>

using TimePoint = std::chrono::time_point<std::chrono::high_resolution_clock>;
using DeltaTimePoint = std::chrono::duration<double, std::milli>;

struct TimingInfo
{
    std::vector<DeltaTimePoint::rep> energy_map;
    std::vector<DeltaTimePoint::rep> compute_seam;
    std::vector<DeltaTimePoint::rep> remove_seam;
};

class SeamCarving
{
public:

    ///
    /// Constructor for seam carving object
    ///
    /// @param _image Input image to perform retargeting operation on
    ///
    explicit SeamCarving(const cv::Mat &_image);

    ///
    /// Function to perform the seam carving operation on the image rows
    /// and columns to achieve the desired retargeting size
    ///
    /// @param target_rows The number of rows in the retargeted image
    /// @param target_cols The number of cols in the retargeted image
    ///
    void retarget(const int target_rows, const int target_cols);

    ///
    /// Function to print the retargting results and timing analysis
    ///
    void printReport();

    ///
    /// Getter for the retargeted image
    ///
    /// @return retargeted image
    ///
    cv::Mat getImage()
    { return m_retargeted_image; }

    ///
    /// Getter for the seam carving energy map
    ///
    /// @return seam carving energy map
    ///
    cv::Mat getEnergyMap()
    { return m_energy_image; }

    ///
    /// Getter for the image with the minimum cost seam overlay
    ///
    /// @return seam image
    ///
    cv::Mat getSeamImage()
    { return m_seam_image; }

private:

    ///
    /// Function which computes the energy function of the image by finding
    /// image gradients in the x and y direction with a sobel filter and then
    /// averaging them
    ///
    /// @param input The image to compute the energy map from
    /// @param energy Reference ot the energy map which will be computed
    ///
    void computeEnergy(const cv::Mat &input, cv::Mat &energy);

    ///
    /// Dynamic programming algorithm to find the minimum energy seam in the image
    /// and then remove it, while updating the member images
    ///
    /// See https://en.wikipedia.org/wiki/Seam_carving#Dynamic_programming
    ///
    /// @param input The input image to remove the seam from
    ///
    void removeMinEnergySeam(cv::Mat &input);

    ///
    /// Member variables
    ///
    cv::Mat m_original_image;
    cv::Mat m_retargeted_image;
    cv::Mat m_seam_image;
    cv::Mat m_energy_image;
    TimingInfo timing_info;
};
