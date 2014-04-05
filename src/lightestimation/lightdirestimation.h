#ifndef LIGHTDIRESTIMATION_H
#define LIGHTDIRESTIMATION_H

#include <vector>
#include <iostream>
#include <iomanip>
#include <math.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>

#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <vtkPointData.h>
#include <vtkDataArray.h>

#include "ransac.h"
#include "../reconstruction/dataset.h"

class LightDirEstimation {
    
public:
    cv::Vec3f execute(camera cam, vtkSmartPointer<vtkPolyData> visualHull);
    
private:
    /** Creates images displaying shape of illumination consensus and writes 
     * those images to disk for visual debugging */
    void displayLightDirections(const char *imgName, int width, int height, cv::Mat image, std::vector<cv::Vec3f> lightDirs, cv::Vec3f maxConsens);
    /** Estimates light direction using a robust ransac scheme */
    cv::Vec3f ransacLightDir(std::vector<contourpoint> contourPoints);
    cv::Mat cameraDirection(int imgWidth, int imgHeight, camera cam);
    cv::Point2i project(cv::Mat P, double *p);
    /** Vector holding all calculated light directions */
    std::vector<cv::Vec3f> lightDirections;
};

#endif