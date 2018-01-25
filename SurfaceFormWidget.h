#ifndef SURFACEFORMWIDGET_H
#define SURFACEFORMWIDGET_H

#include <QWidget>

namespace Ui {
class SurfaceFormWidget;
}

class SurfaceFormWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SurfaceFormWidget(QWidget *parent = 0);
    ~SurfaceFormWidget();

private:
    Ui::SurfaceFormWidget *ui;
};

#endif // SURFACEFORMWIDGET_H
