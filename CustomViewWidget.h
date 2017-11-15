#ifndef CUSTOMVIEWWIDGET_H
#define CUSTOMVIEWWIDGET_H
#include "stdafx.h"
#include <QWidget>

namespace Ui {
class CustomViewWidget;
}

class CustomViewWidget : public QVTKWidget {
  Q_OBJECT

public:
  explicit CustomViewWidget(QWidget *parent = 0);
  ~CustomViewWidget();

private:
  Ui::CustomViewWidget *ui;
};

#endif // CUSTOMVIEWWIDGET_H
