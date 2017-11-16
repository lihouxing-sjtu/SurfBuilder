#include "CustomViewWidget.h"
#include "ui_CustomViewWidget.h"

CustomViewWidget::CustomViewWidget(QWidget *parent)
    : QVTKWidget(parent), ui(new Ui::CustomViewWidget) {
  ui->setupUi(this);

  m_RenderWin = vtkSmartPointer<vtkRenderWindow>::New();
  m_RenderRen = vtkSmartPointer<vtkRenderer>::New();
  m_Interactor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
  m_TrackBall = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();

  m_RenderWin->AddRenderer(m_RenderRen);
  m_RenderWin->SetInteractor(m_Interactor);
  m_Interactor->SetInteractorStyle(m_TrackBall);

  m_AnnotatedCube = vtkSmartPointer<vtkAnnotatedCubeActor>::New();
  m_AxesActor = vtkSmartPointer<vtkAxesActor>::New();
  auto propAssemble = vtkSmartPointer<vtkPropAssembly>::New();
  propAssemble->AddPart(m_AnnotatedCube);
  propAssemble->AddPart(m_AxesActor);
  m_OrientationMarker = vtkSmartPointer<vtkOrientationMarkerWidget>::New();

  m_OrientationMarker->SetInteractor(m_Interactor);
  m_OrientationMarker->SetOrientationMarker(propAssemble);
  m_OrientationMarker->EnabledOn();
  this->SetRenderWindow(m_RenderWin);
  m_Interactor->Initialize();

  double bk1[3], bk2[3];
  QSettings settings;
  bk1[0] = settings.value("bk1_red").toDouble();
  bk1[1] = settings.value("bk1_green").toDouble();
  bk1[2] = settings.value("bk1_blue").toDouble();

  bk2[0] = settings.value("bk2_red").toDouble();
  bk2[1] = settings.value("bk2_green").toDouble();
  bk2[2] = settings.value("bk2_blue").toDouble();

  SetButtonColor(ui->BackGround1Button, bk1);
  SetButtonColor(ui->BackGround2Button, bk2);
  m_RenderRen->GradientBackgroundOn();
  m_RenderRen->SetBackground(bk1);
  m_RenderRen->SetBackground2(bk2);
  m_RenderWin->Render();

  this->CollectionOfConnect();
}

CustomViewWidget::~CustomViewWidget() { delete ui; }

vtkRenderer *CustomViewWidget::GetViewRenderer() { return m_RenderRen; }

void CustomViewWidget::resizeEvent(QResizeEvent *event) {
  int height = this->height();
  int button1Width = ui->BackGround1Button->width();
  int button1Height = ui->BackGround1Button->height();
  ui->BackGround1Button->setGeometry(10, height - button1Height - 20,
                                     button1Width, button1Height);
  ui->BackGround2Button->setGeometry(10, 20, button1Width, button1Height);
  QWidget::resizeEvent(event);
}

void CustomViewWidget::SetButtonColor(QPushButton *button, double color[]) {
  QString commonStyle = "border-color: rgb(241, 241, "
                        "241);border-width:5px;border-style:dotted;border-"
                        "radius:2px;";
  int colorInt[3];
  for (int i = 0; i < 3; i++) {
    colorInt[i] = color[i] * 255;
  }
  QString buttonStyle = "background-color:rgb(" + QString::number(colorInt[0]) +
                        "," + QString::number(colorInt[1]) + "," +
                        QString::number(colorInt[2]) + ")" + ";" + commonStyle;
  button->setStyleSheet(buttonStyle);
}

void CustomViewWidget::CollectionOfConnect() {
  connect(ui->BackGround1Button, SIGNAL(clicked(bool)), this,
          SLOT(OnChangeBKColor1()));
  connect(ui->BackGround2Button, SIGNAL(clicked(bool)), this,
          SLOT(OnChangeBKColor2()));
}

void CustomViewWidget::OnChangeBKColor1() {
  QColor selectedColor =
      QColorDialog::getColor(nullptr, this, tr("Choose Background Color 1"));
  if (!selectedColor.isValid())
    return;
  double bk1[3];
  bk1[0] = selectedColor.red() / 255.0;
  bk1[1] = selectedColor.green() / 255.0;
  bk1[2] = selectedColor.blue() / 255.0;
  m_RenderRen->SetBackground(bk1);
  m_RenderWin->Render();
  QSettings settings;
  settings.setValue("bk1_red", bk1[0]);
  settings.setValue("bk1_green", bk1[1]);
  settings.setValue("bk1_blue", bk1[2]);
  SetButtonColor(ui->BackGround1Button, bk1);
}
void CustomViewWidget::OnChangeBKColor2() {
  QColor selectedColor =
      QColorDialog::getColor(nullptr, this, tr("Choose Background Color 2"));
  if (!selectedColor.isValid())
    return;
  double bk2[3];
  bk2[0] = selectedColor.red() / 255.0;
  bk2[1] = selectedColor.green() / 255.0;
  bk2[2] = selectedColor.blue() / 255.0;
  m_RenderRen->SetBackground2(bk2);
  m_RenderWin->Render();
  QSettings settings;
  settings.setValue("bk2_red", bk2[0]);
  settings.setValue("bk2_green", bk2[1]);
  settings.setValue("bk2_blue", bk2[2]);
  SetButtonColor(ui->BackGround2Button, bk2);
}
