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
    original = _image.clone();
    seam_image = _image.clone();
}