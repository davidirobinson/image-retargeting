//
// seam_carving.cc
//
// Author: David Robinson
// Date: 05/06/19
//

#include <seam_carving.hh>

SeamCarving::SeamCarving(const cv::Mat &_image) :
    m_original_image(_image.clone()),
    m_retargeted_image(_image.clone()),
    m_seam_image(_image.clone())
{
    if (!_image.data)
        throw std::runtime_error("Input image is empty");

    if (_image.channels() != 1 && _image.channels() != 3)
        throw std::runtime_error("Seam Carving only supports RGB & Grayscale images");

    // Compute energy upon contruction so that getter is valid
    computeEnergy(_image, m_energy_image);
}

void SeamCarving::retarget(const int target_rows, const int target_cols)
{
    const auto d_rows = target_rows - m_retargeted_image.rows;
    const auto d_cols = target_cols - m_retargeted_image.cols;

    // Retarget columns
    if (d_cols < 0)
    {
        for (int i = 0; i < abs(d_cols); ++i)
        {
            removeMinEnergySeam(m_retargeted_image);
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
        cv::rotate(m_retargeted_image, m_retargeted_image, cv::ROTATE_90_CLOCKWISE);
        for (int i = 0; i < abs(d_rows); ++i)
        {
            removeMinEnergySeam(m_retargeted_image);
        }
        cv::rotate(m_retargeted_image, m_retargeted_image, cv::ROTATE_90_COUNTERCLOCKWISE);
    }
    else if (d_rows > 0)
    {
        throw std::runtime_error(
            "Increasing the height of the image is not yet supported");
    }
}

void SeamCarving::printReport()
{
    const auto mean_energy_map_time =
        std::accumulate(timing_info.energy_map.begin(), timing_info.energy_map.end(), 0.0) /
        static_cast<double>(timing_info.energy_map.size());

    const auto mean_compute_seam_time =
        std::accumulate(timing_info.compute_seam.begin(), timing_info.compute_seam.end(), 0.0) /
        static_cast<double>(timing_info.compute_seam.size());

    const auto mean_remove_seam_time =
        std::accumulate(timing_info.remove_seam.begin(), timing_info.remove_seam.end(), 0.0) /
        static_cast<double>(timing_info.remove_seam.size());

    std::cout
        << "Seam Carving Report\n"
        << "-------------------\n\n"
        << "Original image size:    " << m_original_image.size() << "\n"
        << "Retargeted image size:  " << m_retargeted_image.size() << "\n\n"
        << "Mean Energy Map Generation Time:    " << mean_energy_map_time << "ms\n"
        << "Mean Min Seam Calulation Duration:  " << mean_compute_seam_time << "ms\n"
        << "Mean Min Seam Removal Duration:     " << mean_remove_seam_time << "ms"
        << std::endl;
}

void SeamCarving::computeEnergy(const cv::Mat &input, cv::Mat &energy)
{
    cv::Mat filtered;
    if (input.channels() == 1)
        filtered = input.clone();
    else
        cv::cvtColor(input, filtered, cv::COLOR_BGR2GRAY);
    cv::GaussianBlur(filtered, filtered, cv::Size(5, 5), 0, 0);

    cv::Mat dx, dy;
    cv::Sobel(filtered, dx, CV_16S, 1, 0, 3);
    cv::Sobel(filtered, dy, CV_16S, 0, 1, 3);

    cv::Mat dx_abs, dy_abs;
    cv::convertScaleAbs(dx, dx_abs);
    cv::convertScaleAbs(dy, dy_abs);

    cv::addWeighted(dx_abs, 0.5, dy_abs, 0.5, 0, energy);
}

void SeamCarving::removeMinEnergySeam(cv::Mat &input)
{
    TimePoint t_start;
    TimePoint t_end;

    //
    // Compute the energy in the image
    //
    t_start = std::chrono::high_resolution_clock::now();

    computeEnergy(input, m_energy_image);

    t_end = std::chrono::high_resolution_clock::now();
    timing_info.energy_map.push_back(DeltaTimePoint(t_end - t_start).count());

    //
    // Compute the minimum energy seam in the energy image
    //
    t_start = std::chrono::high_resolution_clock::now();

    cv::Mat energy_cumulative = m_energy_image.clone();
    energy_cumulative.convertTo(energy_cumulative, CV_16U);
    cv::Mat backtrack = cv::Mat::zeros(m_energy_image.size(), CV_16U);

    auto prev_row_iter = energy_cumulative.begin<ushort>();

    for (size_t row = 1; row < m_energy_image.rows; ++row)
    {
        auto energy_cumulative_ptr = energy_cumulative.ptr<ushort>(row);
        auto backtrack_ptr = backtrack.ptr<ushort>(row);

        for (size_t col = 0; col < m_energy_image.cols; ++col)
        {
            // Deal with first and last column
            int prev_col_range = (col == 0 ? 0 : 1);
            int post_col_range = (col == m_energy_image.cols-1 ? 1 : 2);

            // Get idx of min energy from previous row, on 8-connected cols
            const auto result = std::min_element(
                prev_row_iter - prev_col_range, prev_row_iter + post_col_range);
            const auto idx = std::distance(prev_row_iter - prev_col_range, result);

            backtrack_ptr[col] = idx - prev_col_range + col;
            energy_cumulative_ptr[col] += *result;
            prev_row_iter++;
        }
    }

    t_end = std::chrono::high_resolution_clock::now();
    timing_info.compute_seam.push_back(DeltaTimePoint(t_end - t_start).count());

    //
    // Remove the minimum cost seam
    //
    t_start = std::chrono::high_resolution_clock::now();

    // Create output image with one less column
    cv::Mat output = cv::Mat::zeros(cv::Size(input.cols-1, input.rows), input.type());

    // Update the seam image to our input before seam removal
    if (input.channels() == 1)
        cv::cvtColor(input, m_seam_image, cv::COLOR_GRAY2BGR);
    else
        m_seam_image = input.clone();

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
        m_seam_image.at<cv::Vec3b>(row, col)[0] = 0;
        m_seam_image.at<cv::Vec3b>(row, col)[1] = 0;
        m_seam_image.at<cv::Vec3b>(row, col)[2] = 255;

        // Assign pixels in the current row, ommitting our min cost seam
        input.row(row).colRange(0, col).copyTo(output.row(row).colRange(0, col));
        input.row(row).colRange(col + 1, input.cols).copyTo(output.row(row).colRange(col, output.cols));

        // Update min cost seam
        auto backtrack_ptr = backtrack.ptr<ushort>(row);
        col = int(backtrack_ptr[col]);
    }

    t_end = std::chrono::high_resolution_clock::now();
    timing_info.remove_seam.push_back(DeltaTimePoint(t_end - t_start).count());

    // Copy reference to output
    input = output;
}