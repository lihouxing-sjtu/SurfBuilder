#ifndef NORMALWIDGET_H
#define NORMALWIDGET_H

#include "stdafx.h"
#include <QDialog>
namespace Ui {
class NormalWidget;
}

class NormalWidget : public QDialog {
  Q_OBJECT

public:
  explicit NormalWidget(QWidget *parent = 0, vtkRenderer *ren = 0);
  ~NormalWidget();
  void SetPoints(double direction[], double location[]);
  double GetDirectionAndDistance(double dir[]);
  void SetContourPoints(vtkPoints *pts);
  void SetGeomSurface(GeomPlate_MakeApprox plate, double Umin, double Umax,
                      double Vmin, double Vmax);
  void SetGeomSurface(Handle_Geom_Surface gs, double Umin, double Umax,
                      double Vmin, double Vmax);
  Handle_TopoDS_HShape GetData(vtkPolyData *out);

private:
  Ui::NormalWidget *ui;
  vtkSmartPointer<vtkRenderer> m_render;
  double m_direction[3], m_basePoint[3], m_OrigionDirection[3], m_Distance;
  bool isPickPoint;
  vtkSmartPointer<vtkActor> m_ArrowActor;
  vtkSmartPointer<vtkPoints> m_ContourPoints;
  vtkSmartPointer<vtkActor> m_StrechActor;
  Handle_Geom_Surface m_GeomSurface;
  QList<vtkActor *> m_TubeActorList;
  Handle_TopoDS_HShape m_PlateDS;
  Handle_TopoDS_HShape m_TubeDS;
  Handle_TopoDS_HShape m_FinalDS;
  vtkSmartPointer<vtkPolyData> m_StrechData;
  double m_PlateBounds[4];
  void GetArrow(double startPt[], double direction[], double length,
                vtkPolyData *output);
  void BuildArrow();

  void ConvertTopoDS2PolyData(TopoDS_Shape input, vtkPolyData *output);

signals:
  void pickBasePoint();
  void strechDone();

protected slots:
  void OnLeftButton();
  void OnRightButton();
  void OnUpButton();
  void OnDownButton();
  void OnPickBasePoint();
  void OnResetDirection();
  void OnApply();
  void BuildPlate();
};

#endif // NORMALWIDGET_H
