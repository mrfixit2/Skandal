FILE (GLOB_RECURSE project_SRCS *.cpp *.cxx *.cc *.C *.c *.h *.hpp)
SET (project_MOC_HEADERS app.h)
SET (project_LIBS ${Boost_LIBRARIES} ${TBB_LIBRARY} ${QT_LIBRARIES} ${OpenCV_LIBS} ${PHIDGETS_LIBRARIES} ${aruco_LIBS} ${DC1394_LIBRARIES} ${VTK_LIBRARIES} QVTK vtkHybrid)
SET (project_BIN ${PROJECT_NAME})

QT4_WRAP_CPP(project_MOC_SRCS_GENERATED ${project_MOC_HEADERS})
ADD_EXECUTABLE(${project_BIN} ${project_SRCS} ${project_MOC_SRCS_GENERATED})
TARGET_LINK_LIBRARIES(${project_BIN} ${project_LIBS})
INSTALL(TARGETS ${project_BIN} DESTINATION bin)
