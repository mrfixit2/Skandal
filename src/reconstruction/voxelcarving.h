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

/** Reconstructing 3D shape of an object from given dataset
 *
 * With the segmented images of an object from multiple camera views this class
 * reconstructs the maximum volume in 3D space which may have produced them.
 * It requires a dataset with calibrated cameras (meaning intrinsic parameter
 * and extrinsic parameter must be known) together with original camera images.
 * The camera images will be automatically segmented before the reconstruction
 * through a segmentation method provided through @ref Segmentation class */
class VoxelCarving : public ExportMesh {
    
public:
    /** Constructor for voxel carving.
     *
     * It is assumed that the dataset contains round table scans, so that
     * the camera is circulating around the object while taking photos. With
     * this assumption the boundingbox is calculated using two orthogonal
     * camera images with the first and the dataset size / 4 th image.
     * @param ds Dataset with calibrated cameras and segmented images 
     * @param voxelGridDimension Used voxel grid dimension for reconstruction
     * @param method Segmentation method. Available are thresh and grabcut */
    VoxelCarving(DataSet ds, const int voxelGridDimension, string method);
    /** Destructor for voxel carving */
    ~VoxelCarving();
    /** Returns boundingbox of two orthogonal cams */
    boundingbox getBoundingBox(camera cam1, camera cam2);
    /** Exports the reconstruction in ply object format
     * @param filename Filename of the exported ply object */
    void exportAsPly(string filename);
    
private:
    /** Returns 2D boundingbox around object */
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