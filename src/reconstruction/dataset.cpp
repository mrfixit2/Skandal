#include "dataset.h"

DataSet::DataSet() {
    
}

DataSet::DataSet(string directory) {
    
    read(directory);
}

DataSet::~DataSet() {
    
}

bool DataSet::isValid(cv::Mat K, cv::Mat dist) {
    
    if (K.rows != 3 && K.cols != 3) {
        std::cerr << "Error: expected K with 3 rows and 3 cols." << std::endl;
        std::cerr << "camera calibration matrix K has wrong number of rows/cols." << std::endl;
        return false;
    } else if (dist.rows != 1 && dist.cols != 4) {
        std::cerr << "Error: expected dist with 1 row and 4 cols." << std::endl;
        std::cerr << "camera distortion matrix dist has wrong number of rows/cols." << std::endl;
        return false;
    }
    
    return true;
}

bool DataSet::read(std::string directory) {

    /* read in camera images */
    path dir(directory);
    
    /* acceptable image formats */
    string exts[] = {".png", ".jpg"};
    vector<string> extensions(exts, exts + sizeof(exts) / sizeof(string));
    
    cameras.clear();
    for (directory_iterator it(dir); it != directory_iterator(); ++it) {
        if (is_regular_file(it->status()) && find(begin(extensions), end(extensions), it->path().extension().string()) != end(extensions)) {
            string filename = it->path().string();
            camera cam;
            cam.image = cv::imread(filename);
            cameras.push_back(cam);
        }
    }
    
    /* no images found */
    if (cameras.size() == 0) {
        cerr << "Error: no images found" << endl;
        return false;
    }
    
    /* read camera calibration/distortion matrix */
    cv::FileStorage Kfs((dir / "K.xml").string(), cv::FileStorage::READ);
    Kfs["K_matrix"] >> K;
    cv::FileStorage Dfs((dir / "dist.xml").string(), cv::FileStorage::READ);
    Dfs["dist_coeff"] >> dist;
    if (!isValid(K, dist)) {
        return false;
    }

    /* read camera projection matrices */
    cv::FileStorage Pfs((dir / "viff.xml").string(), cv::FileStorage::READ);
    for (int i = 0; i < cameras.size(); i++) {
        std::stringstream s;
        s << "viff" << std::setfill('0') << std::setw(3) << i << "_matrix";
        Pfs[s.str()] >> cameras[i].P;
        cv::decomposeProjectionMatrix(cameras[i].P, cameras[i].K, cameras[i].R, cameras[i].t);
        cameras[i].K = K;
    }
    
    return true;
}
