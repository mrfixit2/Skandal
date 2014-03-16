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

using namespace std;

class ExportMesh {
    
public:
    virtual void exportAsPly(string filename) = 0;
};

#endif