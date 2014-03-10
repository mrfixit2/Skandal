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

struct voxelGridParams {
    float startX;
    float startY;
    float startZ;
    float voxelWidth;
    float voxelHeight;
    float voxelDepth;
};

struct voxel {
    float xpos;
    float ypos;
    float zpos;
    float res;
    float value;
};

class VoxelCarving {
    
public:
    VoxelCarving(DataSet ds);
    ~VoxelCarving();
    
private:
    boundingbox getBoundingBox(camera cam1, camera cam2);
    cv::Rect getBoundingRect(cv::Mat imageMask);
    voxelGridParams getStartParameter(boundingbox bb);
    void carve(camera cam);
    cv::Point2i project(camera cam, voxel v);
    DataSet _ds;
    float *voxels;
    voxelGridParams params;
    const int voxelDimension = 32;
    const int voxelGridSlize = voxelDimension*voxelDimension;
    const int voxelGridSize = voxelDimension*voxelDimension*voxelDimension;
};

#endif