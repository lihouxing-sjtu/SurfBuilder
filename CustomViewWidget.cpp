#include "CustomViewWidget.h"
#include "ui_CustomViewWidget.h"

CustomViewWidget::CustomViewWidget(QWidget *parent)
    : QVTKWidget(parent), ui(new Ui::CustomViewWidget) {
  ui->setupUi(this);
}

CustomViewWidget::~CustomViewWidget() { delete ui; }
