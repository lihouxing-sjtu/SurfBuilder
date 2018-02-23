#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "ModelItem.h"
#include "NormalWidget.h"
#include "SurfaceFormWidget.h"
#include "stdafx.h"
#include <QComboBox>
#include <QDebug>
#include <QMainWindow>
#include <QRadioButton>
#include <QSignalMapper>
#include <QSpinBox>
#include <QTreeWidgetItem>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

private:
  Ui::MainWindow *ui;
  QList<ModelItem *> m_ModelList;
  QList<QSlider *> m_SliderList;
  QList<QComboBox *> m_ComboxList;
  QList<QRadioButton *> m_RadioList;
  vtkSmartPointer<vtkRenderer> m_Render;
  vtkSmartPointer<vtkActor> m_PreviewActor;
  QMenu *m_ModelRightMenu;
  int m_SelectItemIndex;
  NormalWidget *m_StrechWidget;
  vtkSmartPointer<vtkActor> m_HingeActor;
  Handle_TopoDS_HShape m_HingeShape;
  SurfaceFormWidget *m_SurfaceForm;
  double m_UpDirection[3], m_DownDirection[3];

  Handle(TopoDS_HShape) m_downHingeSurface;
  vtkSmartPointer<vtkActor> m_downHingeActor;
  double m_downHingeCenter[3];
  vtkSmartPointer<vtkActor> m_downConnectActor;

  Handle(TopoDS_HShape) m_upHingeSurface;
  vtkSmartPointer<vtkActor> m_upHingeActor;
  double m_upHingeCenter[3];
  vtkSmartPointer<vtkActor> m_upConnectActor;

  Handle(TopoDS_HShape) m_upOffSetUp;
  Handle(TopoDS_HShape) m_upOffSetDown;
  Handle(TopoDS_HShape) m_downOffSetUp;
  Handle(TopoDS_HShape) m_downOffSetDown;

protected:
  void CollectionOfConnect();
  void AddModelItem(ModelItem *item);
  void ConvertTopoDS2PolyData(TopoDS_Shape input, vtkPolyData *output);

protected slots:
  void OnImportSTL();
  void OnAddToModel();
  void OnChangeModelColor(QTreeWidgetItem *doubleClickedItem);
  void OnChangeModelOpacity(QWidget *);
  void OnChangeModelRep(QWidget *);
  void OnChangeModelVis(QWidget *);
  void OnModelItemRightClicked(const QPoint &pos);
  void OnDeleteModel();
  void OnDrawBSplineCurve();
  void OnSelectPolyData();
  void OnStrechTopo_Shape();
  void OnStartPickBasePoint();
  void OnEndPickBasePoint();
  void OnStrechModel();
  void OnSaveModel();
  void OnStartArcCut();
  void OnCancleArcCut();
  void OnEndArcCut();
  void OnUpDateHingeButton();
  void OnSurfaceForm();
  void OnSetSurfaceFormPoints();

  void Onvisibility();
  void OnConnectDown();
  void OnConnectUp();
  void OnFilletUp();
  void OnFilletDown();
  void OnTubeUp();
  void OnTubeDown();
  void OnBeltDown();
  void OnBeltUp();

  void OnCombineShape();

  void OnStartPickHook();
  void OnEndPickHook();
};

#endif // MAINWINDOW_H
