#ifndef DATASET_H
#define DATASET_H

#include <iostream>
#include <string>
#include <vector>
#include <vector>
#include <iomanip> 

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>

#include <boost/algorithm/string/case_conv.hpp>
#include <boost/filesystem.hpp>

using namespace std;
using namespace boost::filesystem;
using namespace boost::algorithm;

struct camera {
    cv::Mat K;
    cv::Mat P;
    cv::Mat R;
    cv::Mat t;
    cv::Mat image;
    cv::Mat mask;
};

class DataSet {
    
public:
    DataSet(string directory);
    ~DataSet();
    bool read(string directory);
    vector<camera> cameras;
    
private:
    bool isValid(cv::Mat K, cv::Mat dist);
    cv::Mat K, dist;
};

#endif