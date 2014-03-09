#ifndef VOXELCARVING_H
#define VOXELCARVING_H

#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "dataset.h"
#include "../imaging/segmentation.h"

class VoxelCarving {
    
public:
    VoxelCarving(DataSet ds);
    ~VoxelCarving();
    
private:
    cv::Rect getBoundingRect(cv::Mat imageMask);
    DataSet _ds;
    
};

#endif