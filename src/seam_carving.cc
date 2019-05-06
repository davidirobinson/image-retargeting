//
// seam_carving.cc
//
// Author: David Robinson
// Date: 05/06/19
//

#include <seam_carving.hh>

SeamCarving::SeamCarving(const cv::Mat &_image) :
    energy_computed(false)
{
    m_original_image = _image.clone();
    m_output_image = _image.clone();
    m_seam_image = _image.clone();

    // Compute energy upon contruction so that getter is valid
    computeEnergy(_image, m_energy_image);
}