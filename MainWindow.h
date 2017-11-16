#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "ModelItem.h"
#include "stdafx.h"
#include <QComboBox>
#include <QMainWindow>
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
  vtkSmartPointer<vtkRenderer> m_Render;

protected:
  void CollectionOfConnect();
  void AddModelItem(ModelItem *item);
protected slots:
  void OnImportSTL();
  void OnChangeModelColor(QTreeWidgetItem *doubleClickedItem);
  void OnChangeModelOpacity(QTreeWidgetItem *doubleClickedItem, int column);
};

#endif // MAINWINDOW_H
