#include "voxelcarving.h"

VoxelCarving::VoxelCarving(DataSet ds) : _ds(ds) {
    
    /* segment images */
    Segmentation::binarize(&_ds, cv::Scalar(0,0,40), cv::Scalar(255,255,255));
    
    cv::Rect rect1 = getBoundingRect(_ds.cameras[0].mask.clone());
    cv::Mat img = _ds.cameras[0].image;
    cv::Mat mask = _ds.cameras[0].mask;
    cv::rectangle(img, rect1, cv::Scalar(0,0,255));
    
    cv::resize(img, img, cv::Size(640,480));
    cv::imshow("image", img);
    cv::resize(mask, mask, cv::Size(640,480));
    cv::imshow("mask", mask);
    cv::waitKey();
}

VoxelCarving::~VoxelCarving() {
    
}

cv::Rect VoxelCarving::getBoundingRect(cv::Mat mask) {
    
    int largestArea = 0;
    int largestContourIdx = 0;
    
    std::vector< std::vector<cv::Point> > contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::Rect boundingRect;
    
    cv::findContours(mask, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
    for (int i = 0; i < contours.size(); i++) {
        double a = cv::contourArea(contours[i], false);
        if (a > largestArea) {
            largestArea = a;
            largestContourIdx = i;
            boundingRect = cv::boundingRect(contours[i]);
        }
    }
    
    return boundingRect;
}