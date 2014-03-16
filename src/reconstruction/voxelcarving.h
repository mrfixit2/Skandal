#ifndef VOXELCARVING_H
#define VOXELCARVING_H

/** Bounding box */
typedef struct {
    float xmin; /**< Minimum x value */
    float xmax; /**< Maximum x value */
    float ymin; /**< Minimum y value */
    float ymax; /**< Maximum y value */
    float zmin; /**< Minimum z value */
    float zmax; /**< Maximum z value */
} boundingbox;

/** Voxelgrid parameter */
typedef struct {
    float startX; /**< Start value in x direction */
    float startY; /**< Start value in y direction */
    float startZ; /**< Start value in z direction */
    float voxelWidth; /**< Width of a single voxel */
    float voxelHeight; /**< Height of a single voxel */
    float voxelDepth; /**< Depth of a single voxel */
} voxelGridParams;

/** Voxel */
typedef struct {
    float xpos; /**< X position of voxel */
    float ypos; /**< Y position of voxel */
    float zpos; /**< Z position of voxel */
    float value; /**< Iso value of voxel */
} voxel;

#include <string>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "dataset.h"
#include "../imaging/segmentation.h"
#include "exportmesh.h"
#include "../app.h"

class VoxelCarving : public ExportMesh {
    
public:
    VoxelCarving(DataSet ds, const int voxelGridDimension, string method);
    ~VoxelCarving();
    /** Returns boundingbox of two orthogonal cams */
    boundingbox getBoundingBox(camera cam1, camera cam2);
    void exportAsPly(string filename);
    
private:
    cv::Rect getBoundingRect(cv::Mat imageMask);
    voxelGridParams getStartParameter(boundingbox bb);
    void carve(camera cam);
    cv::Point2i project(camera cam, voxel v);
    DataSet _ds;
    float *voxels;
    voxelGridParams params;
    const int _voxelGridDimension;
    int _voxelGridSlize;
    int _voxelGridSize;
};

#endif