#include "voxelcarving.h"

VoxelCarving::VoxelCarving(DataSet ds) : _ds(ds) {
    
    /* segment images */
    Segmentation::binarize(&_ds, cv::Scalar(0,0,40), cv::Scalar(255,255,255));
    
    /* assuming round table scans we estimate that quarter amounts of 
       images are orthogonal to each other. As such, we calculate the 
       boundingbox of the object from the first two orthogonal images */
    camera cam1 = _ds.cameras[0];
    camera cam2 = _ds.cameras[_ds.cameras.size()/4];
    boundingbox bb = getBoundingBox(cam1, cam2);
    
}

VoxelCarving::~VoxelCarving() {
    
}

cv::Rect VoxelCarving::getBoundingRect(cv::Mat mask) {
    
    int largestArea = 0;
    int largestContourIdx = 0;
    
    std::vector< std::vector<cv::Point> > contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::Rect boundingRect;
    
    cv::findContours(mask.clone(), contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
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

/**
 * Calculation of the boundingbox is done via two orthogonal
 * camera images. The (2D) silhouettes of the object both 
 * define a 3D boundingbox in world coordinates which fully
 * encloses the object. 
 * It is furthermore assumed that the object rotates around 
 * its longitudinal axis at the centre of the world coordinate
 * system.
 * With the two given camera projection matrices one can project
 * the four edges of each 2D bounding rect into world coords.
 *
 *
 *                        | z-axis
 *                        |
 *                        |
 *                  +-----|-x8-------+
 *                 /|     |         /|
 *                x1|     |        x4|
 *               /  |     |       /  |                 +
 *              +---+---x5|------+   |                /|
 *              |   |     |      |   |               / |
 *              |   |     |    /y-axis              /  |
 *              |   |     |   /  |   |             /   |
 *              |   |     |  /   |   |            / p8 |
 *              |   +-----|-x7---+---+           /  /| |
 *              |  /      |/ x-axis  /          +  / | |
 *              | x2      * ---- | x3           | p5 p7|
 *              |/               |/             | | /  +
 *              +-------x6-------+              | |/  /
 *                      /                       | p6 /
 *                     / t1                     |   /
 *        +-----------/-----+                   |  /       (cam2)
 *        |          /      |                   | /
 *        |    p1---/--p4   |                   |/
 *        |    |   /   |    |                   +
 *        |    p2-/----p3   |
 *        |      /          |
 *        +-----/-----------+
 *             /
 *          (cam1)
 */
boundingbox VoxelCarving::getBoundingBox(camera cam1, camera cam2) {
    
    boundingbox bb;
    cv::Rect rect1 = getBoundingRect(cam1.mask);
    cv::Rect rect2 = getBoundingRect(cam2.mask);
    
    cv::Mat t1 = (cam1.K.inv()*cam1.P).col(3);
    cv::Mat t2 = (cam2.K.inv()*cam2.P).col(3);
    cv::Mat p1 = (cv::Mat_<float>(3,1) << rect1.x, rect1.y, 1);
    cv::Mat p2 = (cv::Mat_<float>(3,1) << rect1.x, rect1.y+rect1.height, 1);
    cv::Mat p3 = (cv::Mat_<float>(3,1) << rect1.x+rect1.width, rect1.y+rect1.height, 1);
    cv::Mat p4 = (cv::Mat_<float>(3,1) << rect1.x+rect1.width, rect1.y, 1);
    cv::Mat p5 = (cv::Mat_<float>(3,1) << rect2.x, rect2.y, 1);
    cv::Mat p6 = (cv::Mat_<float>(3,1) << rect2.x, rect2.y+rect2.height, 1);
    cv::Mat p7 = (cv::Mat_<float>(3,1) << rect2.x+rect2.width, rect2.y+rect2.height, 1);
    cv::Mat p8 = (cv::Mat_<float>(3,1) << rect2.x+rect2.width, rect2.y, 1);
    
    cv::Mat x1 = cv::norm(t1)*cam1.K.inv()*p1;
    cv::Mat x2 = cv::norm(t1)*cam1.K.inv()*p2;
    cv::Mat x3 = cv::norm(t1)*cam1.K.inv()*p3;
    cv::Mat x4 = cv::norm(t1)*cam1.K.inv()*p4;
    cv::Mat x5 = cv::norm(t2)*cam1.K.inv()*p5;
    cv::Mat x6 = cv::norm(t2)*cam1.K.inv()*p6;
    cv::Mat x7 = cv::norm(t2)*cam1.K.inv()*p7;
    cv::Mat x8 = cv::norm(t2)*cam1.K.inv()*p8;
    
    bb.xmin = x2.at<float>(0, 0);
    bb.xmax = x3.at<float>(0, 0);
    bb.ymin = x6.at<float>(0, 0);
    bb.ymax = x7.at<float>(0, 0);
    bb.zmin = x4.at<float>(0, 1);
    bb.zmax = x3.at<float>(0, 1);
    
    return bb;
}
