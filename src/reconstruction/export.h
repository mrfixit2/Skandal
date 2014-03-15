#ifndef EXPORT_H
#define EXPORT_H

#include <string>
#include <vtkSmartPointer.h>
#include <vtkStructuredPoints.h>
#include <vtkPointData.h>
#include <vtkPLYWriter.h>
#include <vtkFloatArray.h>
#include <vtkMarchingCubes.h>
#include <vtkCleanPolyData.h>
#include <vtkPolyDataMapper.h>
#include "voxelcarving.h"

using namespace std;

class Export {
    
public:
    static void asPly(string filename, int voxelGridDimension, float *voxels, voxelGridParams params);
};

#endif