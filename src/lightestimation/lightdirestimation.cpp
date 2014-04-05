#include "lightdirestimation.h"

cv::Vec3f LightDirEstimation::execute(camera cam, vtkSmartPointer<vtkPolyData> visualHull) {
    
    int height = cam.image.rows;
    int width = cam.image.cols;
    std::vector<contourpoint> contourPoints;
    lightDirections.clear();
    
    /* image intensity, assuming linear camera response function */
    cv::Mat gray;
    cv::cvtColor(cam.image, gray, CV_BGR2GRAY);
    
    /* get points on visual hull and corresponding surface normals */
    for (int i = 0; i < visualHull->GetNumberOfPoints(); i++) {
        double p[3], n[3];
        visualHull->GetPoint(i, p);
        visualHull->GetPointData()->GetNormals()->GetTuple(i, n);
        
        /* check, if visible in image */
        cv::Mat normal = (cv::Mat_<float>(3,1) << n[0], n[1], n[2]);
        cv::Mat camDir = cameraDirection(width, height, cam);
        double angle = normal.dot(camDir);
        if (angle >= 0.5) {
            cv::Point2i c = project(cam.P, p);
            contourpoint cp = { n[0], n[1], n[2], gray.at<uchar>(c.y, c.x) };
            contourPoints.push_back(cp);
        }
    }
    
    std::vector<contourpoint> sampleContourpoints;
    for (int idx = 0; idx<10000; idx++) {
        int a = std::rand() % contourPoints.size();
        sampleContourpoints.push_back(contourPoints.at(a));
    }
    
    cv::Vec3f lightdir = ransacLightDir(sampleContourpoints);
    lightDirections.push_back(lightdir);
    
    /* DEBUG: display all light directions */
    std::vector<cv::Vec3f> lightDirs;
    for (int j=0; j<contourPoints.size()*40; j++) {
        int a = std::rand() % contourPoints.size();
        int b = std::rand() % contourPoints.size();
        int c = std::rand() % contourPoints.size();
        cv::Mat NABC = (cv::Mat_<float>(3,3) << contourPoints.at(a).nx, contourPoints.at(a).ny, contourPoints.at(a).nz,
                        contourPoints.at(b).nx, contourPoints.at(b).ny, contourPoints.at(b).nz,
                        contourPoints.at(c).nx, contourPoints.at(c).ny, contourPoints.at(c).nz);
        cv::Mat I = (cv::Mat_<float>(3,1) << contourPoints.at(a).intensity, contourPoints.at(b).intensity, contourPoints.at(c).intensity);
        cv::Mat L = NABC.inv()*I;
        cv::normalize(L, L);
        cv::Vec3f l(L.at<float>(0, 0), L.at<float>(0, 1), L.at<float>(0, 2));
        
        if (l[2] >= 0.0f) {
            lightDirs.push_back(l);
        }
    }

    displayLightDirections("contour.png", 640, 480, cam.image, lightDirs, lightdir);
    
    return lightdir;
}

inline int clamp(int val, int max) {
    
    if (val >= max) { return max; }
    else { return val; }
}

void LightDirEstimation::displayLightDirections(const char *imgName, int width, int height, cv::Mat image, std::vector<cv::Vec3f> lightDirs, cv::Vec3f maxConsens) {
    
    /* create composed image with surface normals and direction */
    cv::Mat composedImage(height, 2*width, CV_8UC3);
    cv::Mat left = composedImage(cv::Rect(0, 0, width, height));
    cv::Mat right = composedImage(cv::Rect(width, 0, width, height));
    cv::Mat imgCopy;
    cv::resize(image, imgCopy, cv::Size(width,height));
    imgCopy.copyTo(left);
    
    int radius = height/2;
    cv::Mat normals(height, width, CV_8UC3, cv::Scalar(255,255,255));
    cv::circle(normals, cv::Point(width/2,height/2), radius, cv::Scalar(255,0,0), -1, 8);
    cv::cvtColor(normals, normals, CV_BGR2HSV);
    int step = 1;
    for (int i=0; i<lightDirs.size(); i++) {
        int x = (int)(2*lightDirs.at(i)[0]/(1.0f+lightDirs.at(i)[2])*(radius/2.0f))+width/2;
        int y = (int)(2*lightDirs.at(i)[1]/(1.0f+lightDirs.at(i)[2])*(radius/2.0f))+height/2;
        if (lightDirs.at(i)[2] >= 0.0f) {
            int h = normals.at<cv::Vec3b>(y, x)[0]+step;
            int s = normals.at<cv::Vec3b>(y, x)[1];
            int v = normals.at<cv::Vec3b>(y, x)[2];
            cv::circle(normals, cv::Point(x, y), 2, cv::Scalar(clamp(h, 180),s,v), -1, 8);
        }
        
    }
    cv::cvtColor(normals, normals, CV_HSV2BGR);
    
    /* draw estimated light source direction (from RANSAC) */
    int x = (int)(2*maxConsens[0]/(1.0f+maxConsens[2])*(radius/2.0f))+width/2;
    int y = (int)(2*maxConsens[1]/(1.0f+maxConsens[2])*(radius/2.0f))+height/2;
    cv::circle(normals, cv::Point(x, y), 5, cv::Scalar(0, 255, 0), -1, 8);
    
    /* annotations */
    cv::circle(normals, cv::Point(10, 17), 2, cv::Scalar(0, 0, 255), -1, 8);
    cv::circle(normals, cv::Point(10, 37), 4, cv::Scalar(0, 255, 0), -1, 8);
    cv::putText(normals, "light direction", cv::Point(20, 20), CV_FONT_HERSHEY_COMPLEX_SMALL, 0.6, cv::Scalar(0,0,0));
    cv::putText(normals, "max consensus", cv::Point(20, 40), CV_FONT_HERSHEY_COMPLEX_SMALL, 0.6, cv::Scalar(0,0,0));
    
    normals.copyTo(right);
    cv::imwrite(imgName, composedImage);
}

cv::Point2i LightDirEstimation::project(cv::Mat P, double *p) {
    
    cv::Point2i coord;
    
    /* project world point into camera image coords */
    float z =   P.at<float>(2, 0) * p[0] +
                P.at<float>(2, 1) * p[1] +
                P.at<float>(2, 2) * p[2] +
                P.at<float>(2, 3);
    
    coord.y =   (P.at<float>(1, 0) * p[0] +
                 P.at<float>(1, 1) * p[1] +
                 P.at<float>(1, 2) * p[2] +
                 P.at<float>(1, 3)) / z;
    
    coord.x =   (P.at<float>(0, 0) * p[0] +
                 P.at<float>(0, 1) * p[1] +
                 P.at<float>(0, 2) * p[2] +
                 P.at<float>(0, 3)) / z;
    
    return coord;
}

cv::Mat LightDirEstimation::cameraDirection(int imgWidth, int imgHeight, camera cam) {
    
    /* returns view direction of camera */
    cv::Mat center = (cv::Mat_<float>(3,1) << imgWidth/2.0, imgHeight/2.0, 1.0);
    
    cv::Mat X;
    cv::solve(cam.K, center, X, cv::DECOMP_LU);
    
    cv::Mat Rt;
    cv::transpose(cam.R, Rt);
    X = Rt * (X*(-1));
    
    return X/cv::norm(X);
}

cv::Vec3f LightDirEstimation::ransacLightDir(std::vector<contourpoint> contourPoints) {
    
    lightdirectionmodel model;
    Ransac ransac;
    
    ransac.setObservationSet(contourPoints);
    ransac.setModel(model);
    ransac.setIterations(2000);
    ransac.setRequiredInliers(3);
    ransac.setThreshold(1.0f);
    ransac.getBestModel(model);
    
    cv::Vec3f l = cv::Vec3f(model.x, model.y, model.z);
    cv::normalize(l, l);
    std::cout << "RANSAC: " << l[0] << ", " << l[1] << ", " << l[2] << std::endl;
    
    return l;
}