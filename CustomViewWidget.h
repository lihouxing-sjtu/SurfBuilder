#ifndef CUSTOMVIEWWIDGET_H
#define CUSTOMVIEWWIDGET_H
#include "stdafx.h"
#include <QWidget>
VTK_MODULE_INIT(vtkInteractionStyle)
VTK_MODULE_INIT(vtkRenderingFreeType)
VTK_MODULE_INIT(vtkRenderingOpenGL2)
namespace Ui {
class CustomViewWidget;
}

class CustomViewWidget : public QVTKWidget {
  Q_OBJECT

public:
  explicit CustomViewWidget(QWidget *parent = 0);
  ~CustomViewWidget();
  vtkRenderer *GetViewRenderer();
  void GetPickPoints(vtkPoints *output);

private:
  Ui::CustomViewWidget *ui;
  QMenu *m_RightButtonMenu;
  vtkSmartPointer<vtkRenderWindow> m_RenderWin;
  vtkSmartPointer<vtkRenderer> m_RenderRen;
  vtkSmartPointer<vtkRenderWindowInteractor> m_Interactor;
  vtkSmartPointer<vtkInteractorStyleTrackballCamera> m_TrackBall;

  vtkSmartPointer<vtkOrientationMarkerWidget> m_OrientationMarker;
  vtkSmartPointer<vtkAnnotatedCubeActor> m_AnnotatedCube;
  vtkSmartPointer<vtkAxesActor> m_AxesActor;
  vtkSmartPointer<vtkPoints> m_pickedPoints;
  int m_cursorPrePos[2];
  bool m_isBuildBeizerCurve;

protected:
  virtual void resizeEvent(QResizeEvent *event);
  void SetButtonColor(QPushButton *, double color[]);
  void CollectionOfConnect();
  void GetPickPoint(double inputpt[2], double outputpt[3]);
  virtual void mouseDoubleClickEvent(QMouseEvent *event);
signals:
  void endBeizerCurve();
protected slots:
  void OnChangeBKColor1();
  void OnChangeBKColor2();
  void OnRightButtonMenu(QPoint);
  void OnFocusView();
  void OnBeginBeizerCurve();
  void OnEndBeizerCurve();
};

#endif // CUSTOMVIEWWIDGET_H
