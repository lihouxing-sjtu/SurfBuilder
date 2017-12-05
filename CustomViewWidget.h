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
enum pickState { None, BSplineCurve, SelectLoop, PickPoint, ArcCut };
enum keyState { Nokey, F1 };
class CustomViewWidget : public QVTKWidget {
  Q_OBJECT

public:
  explicit CustomViewWidget(QWidget *parent = 0);
  ~CustomViewWidget();
  vtkRenderer *GetViewRenderer();
  void GetPickPoints(vtkPoints *output);
  void SetPickPoint();
  void StartArcCut();
  void CancleArcCut();

private:
  Ui::CustomViewWidget *ui;
  QMenu *m_RightButtonMenu;
  vtkSmartPointer<vtkRenderWindow> m_RenderWin;
  vtkSmartPointer<vtkRenderer> m_RenderRen;
  vtkSmartPointer<vtkRenderWindowInteractor> m_Interactor;
  vtkSmartPointer<vtkInteractorStyleImage> m_InteractorStyle2D;
  vtkSmartPointer<vtkInteractorStyleTrackballCamera> m_TrackBall;

  vtkSmartPointer<vtkOrientationMarkerWidget> m_OrientationMarker;
  vtkSmartPointer<vtkAnnotatedCubeActor> m_AnnotatedCube;
  vtkSmartPointer<vtkAxesActor> m_AxesActor;
  vtkSmartPointer<vtkPoints> m_pickedPoints;
  int m_cursorPrePos[2];
  int m_pickState;
  int m_keyState;
  QList<vtkActor *> m_pickSpheres;

protected:
  virtual void resizeEvent(QResizeEvent *event);
  void SetButtonColor(QPushButton *, double color[]);
  void CollectionOfConnect();
  void GetPickPoint(double inputpt[2], double outputpt[3]);
  virtual void mouseDoubleClickEvent(QMouseEvent *event);
  virtual void keyPressEvent(QKeyEvent *event);
  virtual void keyReleaseEvent(QKeyEvent *event);
  // virtual mousePressEvent(QMouseEvent *event);
  virtual void mouseMoveEvent(QMouseEvent *event);
  void endArcCutPoints();
signals:
  void endBSplineCurve();
  void endSelectLoop();
  void endPickPoint();
  void endArcCut();
protected slots:
  void OnChangeBKColor1();
  void OnChangeBKColor2();
  void OnRightButtonMenu(QPoint);
  void OnFocusView();
  void OnBeginBSplineCurve();
  void OnEndBSplineCurve();
  void OnBeginSelectLoop();
  void OnEndSelectLoop();
  void OnEndPickPoint();
  void OnRemoveLastPoint();
};

#endif // CUSTOMVIEWWIDGET_H
