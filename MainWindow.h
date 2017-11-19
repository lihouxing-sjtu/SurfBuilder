#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "ModelItem.h"
#include "stdafx.h"
#include <QComboBox>
#include <QDebug>
#include <QMainWindow>
#include <QRadioButton>
#include <QSignalMapper>
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
  int m_DeleteItemIndex;

protected:
  void CollectionOfConnect();
  void AddModelItem(ModelItem *item);
  void ConvertTopoDS2PolyData(TopoDS_Shape input, vtkPolyData *output);

protected slots:
  void OnImportSTL();
  void OnChangeModelColor(QTreeWidgetItem *doubleClickedItem);
  void OnChangeModelOpacity(QWidget *);
  void OnChangeModelRep(QWidget *);
  void OnChangeModelVis(QWidget *);
  void OnModelItemRightClicked(const QPoint &pos);
  void OnDeleteModel();
  void OnDrawBeizerCurve();
};

#endif // MAINWINDOW_H
