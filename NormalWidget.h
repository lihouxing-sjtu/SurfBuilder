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

private:
  Ui::NormalWidget *ui;
  vtkSmartPointer<vtkRenderer> m_render;
  double m_direction[3], m_basePoint[3], m_OrigionDirection[3], m_Distance;
  bool isPickPoint;
  vtkSmartPointer<vtkActor> m_ArrowActor;
  void GetArrow(double startPt[], double direction[], double length,
                vtkPolyData *output);
  void BuildArrow();
signals:
  void pickBasePoint();
  void strechDirection();

protected slots:
  void OnLeftButton();
  void OnRightButton();
  void OnUpButton();
  void OnDownButton();
  void OnPickBasePoint();
  void OnResetDirection();
  void OnApply();
};

#endif // NORMALWIDGET_H
