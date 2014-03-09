#include "segmentation.h"

void Segmentation::binarize(DataSet *ds, cv::Scalar startvals, cv::Scalar endvals) {
    
    /* threshold all images in dataset with given range values */
    for (int i = 0; i < ds->cameras.size(); i++) {
        cv::Mat binary;
        cv::cvtColor(ds->cameras[i].image, ds->cameras[i].mask, CV_BGR2HSV);
        cv::inRange(ds->cameras[i].mask, startvals, endvals, ds->cameras[i].mask);
    }
    
}

void Segmentation::grabcut(DataSet *ds) {

    tbb::task_scheduler_init init;
    tbb::parallel_for_each(ds->cameras.begin(), ds->cameras.end(), grabCutParallel);
}

void Segmentation::grabCutParallel(camera cam) {
    
    /* assuming foreground in the middle of the image */
    int eightsW = cam.image.cols/8.0;
    int eightsH = cam.image.rows/8.0;
    cv::Rect area(eightsW*2, 0, eightsW*4, eightsH*7);
    
    /* threshold all images in dataset with graph cut algorithm */
    cv::Mat result, bgModel, fgModel;
    cv::grabCut(cam.image, result, area, bgModel, fgModel, 1, cv::GC_INIT_WITH_RECT);
    
    cv::compare(result, cv::GC_PR_FGD, result, cv::CMP_EQ);
    cam.mask = result.clone();
    cv::bitwise_not(cam.mask, cam.mask);
}
