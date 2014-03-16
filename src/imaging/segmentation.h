#ifndef SEGMENTATION_H
#define SEGMENTATION_H

#include <sstream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <tbb/blocked_range.h>
#include <tbb/parallel_for_each.h>
#include <tbb/task_scheduler_init.h>

#include "../reconstruction/dataset.h"
#include "../app.h"

class Segmentation {
    
public:
    static void binarize(DataSet *ds, cv::Scalar startvals, cv::Scalar endvals);
    static void grabcut(DataSet *ds);
    
private:
    static void grabCutParallel(camera cam);
};

#endif