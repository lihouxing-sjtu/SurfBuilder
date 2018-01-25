#-------------------------------------------------
#
# Project created by QtCreator 2017-11-15T14:49:11
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SurfBuilder
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        MainWindow.cpp \
    CustomViewWidget.cpp \
    ModelItem.cpp \
    NormalWidget.cpp \
    SurfaceFormWidget.cpp

HEADERS += \
        MainWindow.h \
        stdafx.h \
    stdafx.h \
    CustomViewWidget.h \
    ModelItem.h \
    NormalWidget.h \
    SurfaceFormWidget.h

RESOURCES +=resource.qrc

FORMS += \
        MainWindow.ui \
    CustomViewWidget.ui \
    NormalWidget.ui \
    SurfaceFormWidget.ui

INCLUDEPATH+=C:\Environment\VTK\Install\Vs2015-qt590-vtk801\include\vtk-8.0       \
                            C:\Environment\opencascad\instal\OCCT\inc


LIBS+=-LC:/Environment/opencascad/instal/OCCT/win64/vc14/lib \
            -lTKService\
            -lTKShHealing\
            -lTKStd\
            -lTKStdL\
            -lTKSTEP\
            -lTKSTEP209\
            -lTKSTEPAttr\
            -lTKSTEPBase\
            -lTKSTL\
            -lTKTObj\
            -lTKTObjDRAW\
            -lTKTopAlgo\
            -lTKTopTest\
            -lTKV3d\
            -lTKVCAF\
            -lTKViewerTest\
            -lTKVRML\
            -lTKXCAF\
            -lTKXDEDRAW\
            -lTKXDEIGES\
            -lTKXDESTEP\
            -lTKXMesh\
            -lTKXml\
            -lTKXmlL\
            -lTKXmlTObj\
            -lTKXmlXCAF\
            -lTKXSBase\
            -lTKXSDRAW\
            -lTKBin\
            -lTKBinL\
            -lTKBinTObj\
            -lTKBinXCAF\
            -lTKBO\
            -lTKBool\
            -lTKBRep\
            -lTKCAF\
            -lTKCDF\
            -lTKDCAF\
            -lTKDraw\
            -lTKernel\
            -lTKFeat\
            -lTKFillet\
            -lTKG2d\
            -lTKG3d\
            -lTKGeomAlgo\
            -lTKGeomBase\
            -lTKHLR\
            -lTKIGES\
            -lTKIVTK\
            -lTKIVTKDraw\
            -lTKLCAF\
            -lTKMath\
            -lTKMesh\
            -lTKMeshVS\
            -lTKOffset\
            -lTKOpenGl\
            -lTKPrim\
            -lTKQADraw\
-LC:/Environment/VTK/Install/Vs2015-qt590-vtk801/lib \
            -lvtkCommonColor-8.0\
            -lvtkCommonComputationalGeometry-8.0\
            -lvtkCommonCore-8.0\
            -lvtkCommonDataModel-8.0\
            -lvtkCommonExecutionModel-8.0\
            -lvtkCommonMath-8.0\
            -lvtkCommonMisc-8.0\
            -lvtkCommonSystem-8.0\
            -lvtkCommonTransforms-8.0\
            -lvtkDICOMParser-8.0\
            -lvtkDomainsChemistry-8.0\
            -lvtkDomainsChemistryOpenGL2-8.0\
            -lvtkexoIIc-8.0\
            -lvtkexpat-8.0\
            -lvtkFiltersAMR-8.0\
            -lvtkFiltersCore-8.0\
            -lvtkFiltersExtraction-8.0\
            -lvtkFiltersFlowPaths-8.0\
            -lvtkFiltersGeneral-8.0\
            -lvtkFiltersGeneric-8.0\
            -lvtkFiltersGeometry-8.0\
            -lvtkFiltersHybrid-8.0\
            -lvtkFiltersHyperTree-8.0\
            -lvtkFiltersImaging-8.0\
            -lvtkFiltersModeling-8.0\
            -lvtkFiltersParallel-8.0\
            -lvtkFiltersParallelImaging-8.0\
            -lvtkFiltersPoints-8.0\
            -lvtkFiltersProgrammable-8.0\
            -lvtkFiltersSelection-8.0\
            -lvtkFiltersSMP-8.0\
            -lvtkFiltersSources-8.0\
            -lvtkFiltersStatistics-8.0\
            -lvtkFiltersTexture-8.0\
            -lvtkFiltersTopology-8.0\
            -lvtkFiltersVerdict-8.0\
            -lvtkfreetype-8.0\
            -lvtkGeovisCore-8.0\
            -lvtkgl2ps-8.0\
            -lvtkglew-8.0\
            -lvtkGUISupportQt-8.0\
            -lvtkhdf5_hl-8.0\
            -lvtkhdf5-8.0\
            -lvtkImagingColor-8.0\
            -lvtkImagingCore-8.0\
            -lvtkImagingFourier-8.0\
            -lvtkImagingGeneral-8.0\
            -lvtkImagingHybrid-8.0\
            -lvtkImagingMath-8.0\
            -lvtkImagingMorphological-8.0\
            -lvtkImagingSources-8.0\
            -lvtkImagingStatistics-8.0\
            -lvtkImagingStencil-8.0\
            -lvtkInfovisCore-8.0\
            -lvtkInfovisLayout-8.0\
            -lvtkInteractionImage-8.0\
            -lvtkInteractionStyle-8.0\
            -lvtkInteractionWidgets-8.0\
            -lvtkIOAMR-8.0\
            -lvtkIOCore-8.0\
            -lvtkIOEnSight-8.0\
            -lvtkIOExodus-8.0\
            -lvtkIOExport-8.0\
            -lvtkIOExportOpenGL2-8.0\
            -lvtkIOGeometry-8.0\
            -lvtkIOImage-8.0\
            -lvtkIOImport-8.0\
            -lvtkIOInfovis-8.0\
            -lvtkIOLegacy-8.0\
            -lvtkIOLSDyna-8.0\
            -lvtkIOMINC-8.0\
            -lvtkIOMovie-8.0\
            -lvtkIONetCDF-8.0\
            -lvtkIOParallel-8.0\
            -lvtkIOParallelXML-8.0\
            -lvtkIOPLY-8.0\
            -lvtkIOSQL-8.0\
            -lvtkIOTecplotTable-8.0\
            -lvtkIOVideo-8.0\
            -lvtkIOXML-8.0\
            -lvtkIOXMLParser-8.0\
            -lvtkjpeg-8.0\
            -lvtkjsoncpp-8.0\
            -lvtklibharu-8.0\
            -lvtklibxml2-8.0\
            -lvtklz4-8.0\
            -lvtkmetaio-8.0\
            -lvtknetcdf_c++\
            -lvtkNetCDF-8.0\
            -lvtkoggtheora-8.0\
            -lvtkParallelCore-8.0\
            -lvtkpng-8.0\
            -lvtkproj4-8.0\
            -lvtkRenderingAnnotation-8.0\
            -lvtkRenderingContext2D-8.0\
            -lvtkRenderingContextOpenGL2-8.0\
            -lvtkRenderingCore-8.0\
            -lvtkRenderingFreeType-8.0\
            -lvtkRenderingGL2PSOpenGL2-8.0\
            -lvtkRenderingImage-8.0\
            -lvtkRenderingLabel-8.0\
            -lvtkRenderingLOD-8.0\
            -lvtkRenderingOpenGL2-8.0\
            -lvtkRenderingOpenVR-8.0\
            -lvtkRenderingVolume-8.0\
            -lvtkRenderingVolumeOpenGL2-8.0\
            -lvtksqlite-8.0\
            -lvtksys-8.0\
            -lvtktiff-8.0\
            -lvtkverdict-8.0\
            -lvtkViewsContext2D-8.0\
            -lvtkViewsCore-8.0\
            -lvtkViewsInfovis-8.0\
            -lvtkzlib-8.0\
            -lvtkalglib-8.0\
            -lvtkChartsCore-8.0

RESOURCES += \
    resource.qrc
