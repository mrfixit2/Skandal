#include "ransac.h"

Ransac::Ransac() {
    
    _bestError = INT_MAX;
}

void Ransac::setObservationSet(vector<contourpoint> other) {
    
    observationSet = other;
}

void Ransac::setModel(lightdirectionmodel &model) {
 
    bestModel.x = model.x;
    bestModel.y = model.y;
    bestModel.z = model.z;
}

void Ransac::setThreshold(const float thresh) {
    
    _threshold = thresh;
}

void Ransac::setIterations(const int iterations) {
    
    _iterations = iterations;
}

void Ransac::setRequiredInliers(const int requiredInliers) {
    
    _requiredInliers = requiredInliers;
}

bool Ransac::getBestModel(lightdirectionmodel &model) {
    
    int modelFound = false;
    int iterations = 0;

    while (iterations < _iterations) {
        vector<contourpoint> maybeInliers = getMaybeInliers();
        vector<contourpoint> consensusSet = maybeInliers;
        model = getModel(maybeInliers[0], maybeInliers[1], maybeInliers[2]);
        
        for (int i = 0; i < observationSet.size(); i++) {
            if (!isMember(observationSet[i], maybeInliers)) {
                if (fitsModel(observationSet[i], model)) {
                    consensusSet.push_back(observationSet[i]);
                }
            }
        }
        
        if (consensusSet.size() >= _requiredInliers) {
            model = getModel(consensusSet);
            float thisError = getModelError(consensusSet, model);
            if (thisError < _bestError) {
                bestConsensusSet = consensusSet;
                bestModel = model;
                _bestError = thisError;
                modelFound = true;
            }
        }
        iterations++;
    }
    model = bestModel;
    
    return modelFound;
}

lightdirectionmodel Ransac::getModel(const contourpoint &cp1,
                                     const contourpoint &cp2,
                                     const contourpoint &cp3) {
    
    lightdirectionmodel model;
    
    cv::Mat Nabc = (cv::Mat_<float>(3,3) << cp1.nx, cp1.ny, cp1.nz,
                                            cp2.nx, cp2.ny, cp2.nz,
                                            cp3.nx, cp3.ny, cp3.nz);
    cv::Mat Iabc = (cv::Mat_<float>(3,1) << cp1.intensity, cp2.intensity, cp3.intensity);
    cv::Mat l = Nabc.inv() * Iabc;
    
    model.x = l.at<float>(0, 0);
    model.y = l.at<float>(0, 1);
    model.z = l.at<float>(0, 2);
    
    return model;
}

lightdirectionmodel Ransac::getModel(const vector<contourpoint> &observation) const {
    
    lightdirectionmodel model;
    
    cv::Mat I(observation.size(), 1, CV_32FC1);
    cv::Mat N(observation.size(), 3, CV_32FC1);
    cv::Mat N1;
    
    for (int i=0; i<observation.size(); i++) {
        I.at<float>(i, 0) = observation[i].intensity;
        N.at<float>(i, 0) = observation[i].nx;
        N.at<float>(i, 1) = observation[i].ny;
        N.at<float>(i, 2) = observation[i].nz;
    }
    
    cv::invert(N, N1, cv::DECOMP_SVD);
    cv::Mat L = N1*I;
    model.x = L.at<float>(0, 0);
    model.y = L.at<float>(0, 1);
    model.z = L.at<float>(0, 2);
    
    return model;
}

bool Ransac::isMember(const contourpoint cp, const vector<contourpoint> maybeInliers) {
    
    bool isMember = false;
    for (int i = 0; i < maybeInliers.size(); i++) {
        
        if (cp.nx == maybeInliers[i].nx &&
            cp.ny == maybeInliers[i].ny &&
            cp.nz == maybeInliers[i].nz &&
            cp.intensity == maybeInliers[i].intensity) {
            isMember = true;
        }
    }
    
    return isMember;
}

bool Ransac::fitsModel(const contourpoint cp, const lightdirectionmodel &model) const {
    
    bool fits = false;
    float distance = getDistance(cp, model);
    if (distance < _threshold) {
        fits = true;
    }
    
    return fits;
}

vector<contourpoint> Ransac::getMaybeInliers() const {
    
    vector<contourpoint> maybeInliers;
    
    int a = rand() % observationSet.size();
    int b = rand() % observationSet.size();
    int c = rand() % observationSet.size();
    
    maybeInliers.push_back(observationSet[a]);
    maybeInliers.push_back(observationSet[b]);
    maybeInliers.push_back(observationSet[c]);
    
    return maybeInliers;
}

int Ransac::getModelError(const vector<contourpoint> &pointList,
                          const lightdirectionmodel &model) const {
    
    int maxError = 99999;
    vector<contourpoint> consensusSet;
    for (int i = 0; i < pointList.size(); i++) {

        if (fitsModel(pointList[i], model)) {
            consensusSet.push_back(pointList[i]);
        }
    }
    
    return (maxError - consensusSet.size());
}

float Ransac::getDistance(const contourpoint cp, const lightdirectionmodel &model) const {
    
    cv::Mat l = (cv::Mat_<float>(3,1) << model.x, model.y, model.z);
    cv::Mat n = (cv::Mat_<float>(3,1) << cp.nx, cp.ny, cp.nz);
    
    return abs(l.dot(n) - cp.intensity);
}