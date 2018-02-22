#ifndef SURFACEFORMWIDGET_H
#define SURFACEFORMWIDGET_H

#include "stdafx.h"
#include <QWidget>
namespace Ui {
class SurfaceFormWidget;
}

class SurfaceFormWidget : public QWidget {
  Q_OBJECT

public:
  explicit SurfaceFormWidget(QWidget *parent = 0, vtkRenderer *ren = 0);
  ~SurfaceFormWidget();

public:
  void SetCutData(vtkPolyData *data);
  void SetDirection(double *updir, double *downdir);
  void SetEndPos(double *pos);
  void SetStartPos(double *pos);
  Handle(TopoDS_HShape) GetDownHingeSurface();
  void GetDownCenter(double dcenter[]);
  Handle(TopoDS_HShape) GetUpHingeSurface();
  void GetUpCenter(double ucenter[]);

  Handle(TopoDS_HShape) GetUpOffSetUp();
  Handle(TopoDS_HShape) GetUpOffSetDown();
  Handle(TopoDS_HShape) GetDownOffSetUp();
  Handle(TopoDS_HShape) GetDownOffSetDown();
  void SetDownVisibility(bool vi);
  void SetUpVisibility(bool vi);
  void SetTubesVisibility(bool vi);
  void SetBeltVisibility(bool vi);
  double m_DownWireDir[3];
  double m_UpWireDir[3];
  double m_DownWirePoints[4][3];
  double m_UpWirePoints[4][3];

  double m_DownWireRegion[4];
  double m_UpWireRegion[4];

  double m_TubeRegion[4];
  double m_TubeInformation[3];

  Handle(Geom_Surface) m_GeomSurfaceDown;
  Handle(Geom_Surface) m_GeomSurfaceUp;

  Handle(TopoDS_HShape) m_DownBelt1;
  Handle(TopoDS_HShape) m_DownBelt2;
  Handle(TopoDS_HShape) m_DownBelt3;
  Handle(TopoDS_HShape) m_DownBelt4;

private:
  Ui::SurfaceFormWidget *ui;

  // data for cut
  vtkSmartPointer<vtkPolyData> m_ForCutData;
  vtkSmartPointer<vtkRenderer> m_Render;
  vtkSmartPointer<vtkActor> m_ContourActor;
  vtkSmartPointer<vtkActor> m_SurfaceActor;
  vtkSmartPointer<vtkActor> m_TubesActor;

  vtkSmartPointer<vtkActor> m_Belt1Actor;
  vtkSmartPointer<vtkActor> m_Belt2Actor;
  vtkSmartPointer<vtkActor> m_Belt3Actor;
  vtkSmartPointer<vtkActor> m_Belt4Actor;

  double m_Direction[3];
  double m_StartPos[3];
  double m_EndPosp[3];
  double m_UpDirection[3], m_DownDirection[3];

  double m_DownCenter[3];
  double m_UpCenter[3];

  Handle(TopoDS_HShape) m_DownHingeSurface;
  Handle(TopoDS_HShape) m_UpHingeSurface;

  Handle(TopoDS_HShape) m_DownOffSetUp;
  Handle(TopoDS_HShape) m_DownOffSetDown;

  Handle(TopoDS_HShape) m_UpOffSetUp;
  Handle(TopoDS_HShape) m_UpOffSetDown;

  vtkSmartPointer<vtkActor> m_DownOffSetUpActor;
  vtkSmartPointer<vtkActor> m_DownOffSetDownActor;

  vtkSmartPointer<vtkActor> m_UpOffSetUpActor;
  vtkSmartPointer<vtkActor> m_UpOffSetDownActor;

  vtkSmartPointer<vtkActor> m_fromOrigonActor;

protected:
  void ConvertTopoDS2PolyData(TopoDS_Shape input, vtkPolyData *output);
  void GetContourPoints(vtkPolyData *inputData, int sampleNum,
                        vtkPlane *cutplane, vtkPoints *output,
                        vtkPolyData *outData, vtkPolyData *outdata1, bool down);
  void SetGP(gp_Pnt &gp, double p[]);
  void GetGP(gp_Pnt gp, double p[]);

  TopoDS_Shape CalculateBeltShape(double uvRegion[], double height,
                                  QDoubleSpinBox *uspin, QDoubleSpinBox *vspin);
  void SetBeltConnet(QDoubleSpinBox *);
signals:
  void pickTwoPoint();
  void canclePick();
protected slots:
  void BuildSurface();
  void OnChangeContourNum(int);
  void OnChangeEndPos(int);
  void OnChangeSampleNum(int);
  void OnChangeStartPos(int);
  void OnPickTwoPoints();
  void OnPickCancle();
  void OnSetUpDirection();
  void OnSetDownDirection();
  void OnTubesVisibility();

  void OnBuildHingeRegion();
  void OnBuildTubeRegion();

  void OnBuildBeltRegion();
};

#endif // SURFACEFORMWIDGET_H
