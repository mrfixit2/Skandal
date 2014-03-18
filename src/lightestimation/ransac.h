#ifndef RANSAC_H
#define RANSAC_H

/** Contourpoint */
typedef struct {
    float nx; /**< x component of normal vector */
    float ny; /**< y component of normal vector */
    float nz; /**< z component of normal vector */
    short intensity; /**< intensity of contour point */
} contourpoint;

/** Light direction model */
typedef struct {
    float x; /**< x component of light direction */
    float y; /**< y component of light direction */
    float z; /**< z component of light direction */
} lightdirectionmodel;

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <vector>
#include <stdlib.h>

using namespace std;

class Ransac {
    
public:
    Ransac();
    void setObservationSet(vector<contourpoint> observationSet);
    void setModel(lightdirectionmodel &model);
    void setThreshold(const float thresh);
    void setIterations(const int iterations);
    void setRequiredInliers(const int requiredInliers);
    bool getBestModel(lightdirectionmodel &model);
    lightdirectionmodel getModel(const contourpoint &cp1,
                                 const contourpoint &cp2,
                                 const contourpoint &cp3);
    lightdirectionmodel getModel(const vector<contourpoint> &observation) const;
    
    lightdirectionmodel bestModel;
    
private:
    bool isMember(const contourpoint cp, const vector<contourpoint> maybeInliers);
    bool fitsModel(const contourpoint cp, const lightdirectionmodel &model) const;
    vector<contourpoint> getMaybeInliers() const;
    int getModelError(const vector<contourpoint> &pointList,
                      const lightdirectionmodel &model) const;
    float getDistance(const contourpoint cp, const lightdirectionmodel &model) const;
    
    vector<contourpoint> observationSet;
    vector<contourpoint> bestConsensusSet;
    int _requiredInliers;
    int _iterations;
    float _bestError;
    float _threshold;
};

#endif