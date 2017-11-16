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

private:
  Ui::CustomViewWidget *ui;

  vtkSmartPointer<vtkRenderWindow> m_RenderWin;
  vtkSmartPointer<vtkRenderer> m_RenderRen;
  vtkSmartPointer<vtkRenderWindowInteractor> m_Interactor;
  vtkSmartPointer<vtkInteractorStyleTrackballCamera> m_TrackBall;

  vtkSmartPointer<vtkOrientationMarkerWidget> m_OrientationMarker;
  vtkSmartPointer<vtkAnnotatedCubeActor> m_AnnotatedCube;
  vtkSmartPointer<vtkAxesActor> m_AxesActor;

protected:
  virtual void resizeEvent(QResizeEvent *event);
  void SetButtonColor(QPushButton *, double color[]);
  void CollectionOfConnect();
protected slots:
  void OnChangeBKColor1();
  void OnChangeBKColor2();
};

#endif // CUSTOMVIEWWIDGET_H
