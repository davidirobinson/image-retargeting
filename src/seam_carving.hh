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

    void retarget(const int target_rows, const int target_cols)
    {
        const auto d_rows = target_rows - m_output_image.rows;
        const auto d_cols = target_cols - m_output_image.cols;

        // Retarget columns
        if (d_cols < 0)
        {
            for (int i = 0; i < abs(d_cols); ++i)
            {
                removeMinEnergyColumn(m_output_image);
            }
        }
        else if (d_cols > 0)
        {
            throw std::runtime_error(
                "Increasing the width of the image is not yet supported");
        }

        // Retarget Rows
        if (d_rows < 0)
        {
            cv::rotate(m_output_image, m_output_image, cv::ROTATE_90_CLOCKWISE);
            for (int i = 0; i < abs(d_rows); ++i)
            {
                removeMinEnergyColumn(m_output_image);
            }
            cv::rotate(m_output_image, m_output_image, cv::ROTATE_90_COUNTERCLOCKWISE);
        }
        else if (d_rows > 0)
        {
            throw std::runtime_error(
                "Increasing the height of the image is not yet supported");
        }
    }

    cv::Mat getImage()
    {
        return m_output_image;
    }

    cv::Mat getEnergyMap()
    {
        return m_energy_image;
    }

    cv::Mat getSeamImage()
    {
        return m_seam_image;
    }

    void printReport()
    {
        std::cout << "printReport() not yet implemented" << std::endl;
    }

private:

    void computeEnergy(const cv::Mat &input, cv::Mat &energy)
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

        cv::addWeighted(dx_abs, 0.5, dy_abs, 0.5, 0, energy);
        energy_computed = true;
    }

    void computeMinSeam(
        const cv::Mat &energy,
        cv::Mat &energy_cumulative,
        cv::Mat &backtrack)
    {
        energy_cumulative = energy.clone();
        energy_cumulative.convertTo(energy_cumulative, CV_16U);
        backtrack = cv::Mat::zeros(energy.size(), CV_16U);

        auto prev_row_iter = energy_cumulative.begin<ushort>();

        for (size_t row = 1; row < energy.rows; ++row)
        {
            auto energy_cumulative_ptr = energy_cumulative.ptr<ushort>(row);
            auto backtrack_ptr = backtrack.ptr<ushort>(row);

            for (size_t col = 0; col < energy.cols; ++col)
            {
                // Deal with first and last column
                int prev_col_range = (col == 0 ? 0 : 1);
                int post_col_range = (col == energy.cols-1 ? 1 : 2);

                // Get idx of min energy from previous row, on 8-connected cols
                const auto result = std::min_element(
                    prev_row_iter - prev_col_range, prev_row_iter + post_col_range);
                const auto idx = std::distance(prev_row_iter - prev_col_range, result);

                backtrack_ptr[col] = idx - prev_col_range + col;
                energy_cumulative_ptr[col] += *result;
                prev_row_iter++;
            }
        }
    }

    void carveSeam(
        cv::Mat &input,
        cv::Mat &output,
        cv::Mat &seam,
        const cv::Mat &energy_cumulative,
        const cv::Mat &backtrack)
    {
        output = cv::Mat::zeros(cv::Size(input.cols-1, input.rows), input.type());

        // Compute cumulative lowest energy pixel in bottom row
        const auto last_row = energy_cumulative.begin<ushort>()
            + (energy_cumulative.rows - 1) * energy_cumulative.cols;
        const auto result = std::min_element(
            last_row, energy_cumulative.end<ushort>());
        auto col = std::distance(last_row, result);

        // Track back through matrix to remove min cost seam
        for (size_t row = energy_cumulative.rows - 1; signed(row) >= 0; --row)
        {
            // Draw seam over original image
            seam.at<cv::Vec3b>(row, col)[0] = 0;
            seam.at<cv::Vec3b>(row, col)[1] = 0;
            seam.at<cv::Vec3b>(row, col)[2] = 255;

            // Assign pixels in the current row, ommitting our min cost seam
            input.row(row).colRange(0, col).copyTo(output.row(row).colRange(0, col));
            input.row(row).colRange(col + 1, input.cols).copyTo(output.row(row).colRange(col, output.cols));

            // Update min cost seam
            auto backtrack_ptr = backtrack.ptr<ushort>(row);
            col = int(backtrack_ptr[col]);
        }
    }

    void removeMinEnergyColumn(cv::Mat &input)
    {
        computeEnergy(input, m_energy_image);

        cv::Mat energy_cumulative, backtrack;
        computeMinSeam(m_energy_image, energy_cumulative, backtrack);

        cv::Mat output;
        m_seam_image = input.clone();
        carveSeam(input, output, m_seam_image, energy_cumulative, backtrack);
        input = output;
    }

    cv::Mat m_original_image;
    cv::Mat m_output_image;
    cv::Mat m_seam_image;
    cv::Mat m_energy_image;

    bool energy_computed;
};
