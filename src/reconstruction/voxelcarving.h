#ifndef VOXELCARVING_H
#define VOXELCARVING_H

#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "dataset.h"
#include "../imaging/segmentation.h"

struct boundingbox {
    float xmin;
    float xmax;
    float ymin;
    float ymax;
    float zmin;
    float zmax;
};

class VoxelCarving {
    
public:
    VoxelCarving(DataSet ds);
    ~VoxelCarving();
    
private:
    boundingbox getBoundingBox(camera cam1, camera cam2);
    cv::Rect getBoundingRect(cv::Mat imageMask);
    DataSet _ds;
    
};

#endif