#include "export.h"

void Export::asPly(string filename, int voxelGridDimension, float *voxels, voxelGridParams params) {
    
    /* create vtk visualization pipeline from voxelgrid (float array) */
    vtkSmartPointer<vtkStructuredPoints> points = vtkSmartPointer<vtkStructuredPoints>::New();
    points->SetDimensions(voxelGridDimension, voxelGridDimension, voxelGridDimension);
    points->SetSpacing(params.voxelDepth, params.voxelHeight, params.voxelWidth);
    points->SetOrigin(params.startZ, params.startY, params.startX);
    points->SetScalarTypeToFloat();
    
    int voxelGridSize = voxelGridDimension*voxelGridDimension*voxelGridDimension;
    vtkSmartPointer<vtkFloatArray> vtkFArray = vtkSmartPointer<vtkFloatArray>::New();
    vtkFArray->SetNumberOfValues(voxelGridSize);
    vtkFArray->SetArray(voxels, voxelGridSize, 1);
    points->GetPointData()->SetScalars(vtkFArray);
    points->Update();
    
    /* create iso surface with marching cubes algorithm */
    vtkSmartPointer<vtkMarchingCubes> mcubes = vtkSmartPointer<vtkMarchingCubes>::New();
    mcubes->SetInputConnection(points->GetProducerPort());
    mcubes->SetNumberOfContours(1);
    mcubes->SetValue(0, 0.5);
    mcubes->Update();
    
    /* recreate mesh topoloy and merge vertices */
    vtkSmartPointer<vtkCleanPolyData> cleanPolyData = vtkSmartPointer<vtkCleanPolyData>::New();
    cleanPolyData->SetInputConnection(mcubes->GetOutputPort());
    cleanPolyData->Update();
    
    /* exports 3d model in ply format */
    vtkSmartPointer<vtkPLYWriter> plyExporter = vtkSmartPointer<vtkPLYWriter>::New();
    plyExporter->SetFileName(filename.c_str());
    plyExporter->SetInputConnection(cleanPolyData->GetOutputPort());
    plyExporter->Update();
    plyExporter->Write();
}