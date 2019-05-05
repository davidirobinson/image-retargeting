
#pragma once

#include <opencv2/opencv.hpp>
#include <chrono>

class ScopedTimer
{
public:
    ScopedTimer() :
        t_start(std::chrono::high_resolution_clock::now())
    {}

    ~ScopedTimer()
    {
        auto t_end = std::chrono::high_resolution_clock::now();
        std::cout << std::chrono::duration<double, std::milli>(t_end - t_start).count() << std::endl;
    }

private:
    const std::chrono::system_clock::time_point t_start;
};

struct TimingInfo
{
    // TODO
};

class ImageRetargeter
{
public:
    explicit ImageRetargeter(const cv::Mat &_image) :
        image(_image)
    {}

    void retarget(const int target_rows, const int target_cols)
    {
        const auto d_rows = target_rows - image.rows;
        const auto d_cols = target_cols - image.cols;

        // Resize columns
        if (d_cols < 0)
        {
            for (int i = 0; i < abs(d_cols); ++i)
            {
                seamCarveColumn(image);
            }
        }
        else if (d_cols > 0)
        {
            throw std::runtime_error(
                "Increasing the width of the image is not yet supported");
        }

        // Resize Rows
        if (d_rows < 0)
        {
            cv::rotate(image, image, cv::ROTATE_90_CLOCKWISE);
            for (int i = 0; i < abs(d_rows); ++i)
            {
                seamCarveColumn(image);
            }
            cv::rotate(image, image, cv::ROTATE_90_COUNTERCLOCKWISE);
        }
        else if (d_rows > 0)
        {
            throw std::runtime_error(
                "Increasing the height of the image is not yet supported");
        }
    }

    cv::Mat getImage()
    {
        return image;
    }

    TimingInfo getTimingInfo()
    {
        return TimingInfo{};
    }

private:

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

                backtrack_ptr[col] = idx - prev_col_range + col ;
                energy_cumulative_ptr[col] += *result;
                prev_row_iter++;
            }
        }
    }

    void carveSeam(
        cv::Mat &input,
        cv::Mat &output,
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
            // Draw seam (debug)
            input.at<cv::Vec3b>(row, col)[0] = 0;
            input.at<cv::Vec3b>(row, col)[1] = 0;
            input.at<cv::Vec3b>(row, col)[2] = 255;

            // Assign pixels in the current row, ommitting our min cost seam
            input.row(row).colRange(0, col).copyTo(output.row(row).colRange(0, col));
            input.row(row).colRange(col + 1, input.cols).copyTo(output.row(row).colRange(col, output.cols));

            // Update min cost seam
            auto backtrack_ptr = backtrack.ptr<ushort>(row);
            col = int(backtrack_ptr[col]);
        }
    }

    void seamCarveColumn(cv::Mat &input)
    {
        {
            ScopedTimer st;
            energy = computeEnergy(input);
            std::cout << "Energy: ";
        }

        cv::Mat energy_cumulative, backtrack;
        {
            ScopedTimer st;
            minSeam(energy, energy_cumulative, backtrack);
            std::cout << "Min Seam: ";
        }

        cv::Mat output;
        {
            ScopedTimer st;
            carveSeam(input, output, energy_cumulative, backtrack);
            std::cout << "Carve Seam: ";
        }
        input = output;
    }

    cv::Mat image;
    cv::Mat energy;
};
