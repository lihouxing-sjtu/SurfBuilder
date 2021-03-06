#include "CustomViewWidget.h"
#include "ui_CustomViewWidget.h"

CustomViewWidget::CustomViewWidget(QWidget *parent)
    : QVTKWidget(parent), ui(new Ui::CustomViewWidget),
      m_pickState(pickState::None), m_keyState(Nokey) {
  ui->setupUi(this);
  this->setContextMenuPolicy(Qt::CustomContextMenu);
  m_RenderWin = vtkSmartPointer<vtkRenderWindow>::New();
  m_RenderRen = vtkSmartPointer<vtkRenderer>::New();
  m_Interactor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
  m_InteractorStyle2D = vtkSmartPointer<vtkInteractorStyleImage>::New();
  m_TrackBall = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();

  m_RenderWin->AddRenderer(m_RenderRen);
  // m_RenderWin->SetInteractor(m_Interactor);
  m_Interactor->SetRenderWindow(m_RenderWin);
  m_Interactor->SetInteractorStyle(m_TrackBall);
  m_Interactor->RemoveObservers(qPrintable("RightButtonPressEvent"));
  m_AnnotatedCube = vtkSmartPointer<vtkAnnotatedCubeActor>::New();
  m_AxesActor = vtkSmartPointer<vtkAxesActor>::New();
  auto propAssemble = vtkSmartPointer<vtkPropAssembly>::New();
  propAssemble->AddPart(m_AnnotatedCube);
  propAssemble->AddPart(m_AxesActor);
  m_OrientationMarker = vtkSmartPointer<vtkOrientationMarkerWidget>::New();

  m_OrientationMarker->SetInteractor(m_Interactor);
  m_OrientationMarker->SetOrientationMarker(propAssemble);
  m_OrientationMarker->EnabledOn();
  m_OrientationMarker->InteractiveOff();
  this->SetRenderWindow(m_RenderWin);

  m_Interactor->Initialize();
  this->setMouseTracking(false);
  m_pickedPoints = vtkSmartPointer<vtkPoints>::New();
  m_pickedPoints->Initialize();
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
  m_RenderRen->UseDepthPeelingOn();
  m_RenderWin->Render();

  this->CollectionOfConnect();
}

CustomViewWidget::~CustomViewWidget() { delete ui; }

vtkRenderer *CustomViewWidget::GetViewRenderer() { return m_RenderRen; }

void CustomViewWidget::GetPickPoints(vtkPoints *output) {
  output->DeepCopy(m_pickedPoints);
  m_pickedPoints->Initialize();
  foreach (vtkActor *var, m_pickSpheres) { m_RenderRen->RemoveActor(var); }
  m_pickSpheres.clear();
  m_Interactor->SetInteractorStyle(m_TrackBall);
  m_RenderWin->Render();
}

void CustomViewWidget::SetPickPoint() {
  if (m_pickState != None) {
    QMessageBox::information(this, tr("Error"),
                             tr("Please finish the pick firstly!"));
    return;
  }
  m_pickState = PickPoint;
  this->setCursor(Qt::CrossCursor);
}

void CustomViewWidget::StartArcCut() {
  if (m_pickState != None) {
    QMessageBox::information(this, tr("Error"),
                             tr("Please finish the pick firstly!"));
    return;
  }
  m_pickState = ArcCut;
  m_Interactor->SetInteractorStyle(m_InteractorStyle2D);
  this->setCursor(Qt::CrossCursor);
}

void CustomViewWidget::CancleArcCut() {
  if (m_pickState != ArcCut)
    return;
  m_pickState = None;
  this->setCursor(Qt::ArrowCursor);
  foreach (vtkActor *var, m_pickSpheres) { m_RenderRen->RemoveActor(var); }
  m_pickSpheres.clear();
  m_Interactor->SetInteractorStyle(m_TrackBall);
  m_RenderWin->Render();
  m_pickedPoints->Reset();
  m_pickedPoints->Initialize();
}

void CustomViewWidget::SetPickHook() {
  if (m_pickState != None) {
    QMessageBox::information(this, tr("Error"),
                             tr("Please finish the pick firstly!"));
    return;
  }
  m_pickState = pickHook;
  this->setCursor(Qt::CrossCursor);
}

// void CustomViewWidget::resizeEvent(QResizeEvent *event) {
//  int height = this->height();
//  int button1Width = ui->BackGround1Button->width();
//  int button1Height = ui->BackGround1Button->height();
//  ui->BackGround1Button->setGeometry(10, height - button1Height - 20,
//                                     button1Width, button1Height);
//  ui->BackGround2Button->setGeometry(10, 20, button1Width, button1Height);
//  QWidget::resizeEvent(event);
//}

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

  m_RightButtonMenu = new QMenu(this);
  QAction *focusAction = new QAction(tr("Focus"), m_RightButtonMenu);
  m_RightButtonMenu->addAction(focusAction);
  QMenu *curveMenu = new QMenu("Build Beizer Curve", m_RightButtonMenu);
  QAction *beginBSplineCurveAction = new QAction("Begin", curveMenu);
  QAction *endBSplineCurveAction = new QAction("End", curveMenu);
  curveMenu->addAction(beginBSplineCurveAction);
  curveMenu->addAction(endBSplineCurveAction);
  m_RightButtonMenu->addMenu(curveMenu);
  connect(focusAction, SIGNAL(triggered(bool)), this, SLOT(OnFocusView()));
  connect(this, SIGNAL(customContextMenuRequested(QPoint)), this,
          SLOT(OnRightButtonMenu(QPoint)));
  connect(beginBSplineCurveAction, SIGNAL(triggered(bool)), this,
          SLOT(OnBeginBSplineCurve()));
  connect(endBSplineCurveAction, SIGNAL(triggered(bool)), this,
          SLOT(OnEndBSplineCurve()));

  QMenu *selectRegion = new QMenu("Select Region", m_RightButtonMenu);
  QAction *startLoopAction = new QAction("Begin", selectRegion);
  QAction *endLoopAction = new QAction("End", selectRegion);
  selectRegion->addAction(startLoopAction);
  selectRegion->addAction(endLoopAction);
  m_RightButtonMenu->addMenu(selectRegion);
  QAction *removeLastPoint =
      new QAction(tr("remove last Point"), m_RightButtonMenu);
  m_RightButtonMenu->addAction(removeLastPoint);
  connect(startLoopAction, SIGNAL(triggered(bool)), this,
          SLOT(OnBeginSelectLoop()));
  connect(endLoopAction, SIGNAL(triggered(bool)), this,
          SLOT(OnEndSelectLoop()));
  connect(removeLastPoint, SIGNAL(triggered(bool)), this,
          SLOT(OnRemoveLastPoint()));
}

void CustomViewWidget::GetPickPoint(double inputpt[2], double outputpt[3]) {
  auto picker = vtkSmartPointer<vtkPropPicker>::New();
  picker->Pick(inputpt[0], inputpt[1], 0, m_RenderRen);
  picker->GetPickPosition(outputpt);
  m_RenderWin->Render();
}

void CustomViewWidget::mouseDoubleClickEvent(QMouseEvent *event) {
  if (m_pickState == None) {
    QVTKWidget::mouseDoubleClickEvent(event);
    return;
  }
  double displayPos[2];
  displayPos[0] = event->pos().x();
  displayPos[1] = height() - event->pos().y();
  double pickedPos[3];
  this->GetPickPoint(displayPos, pickedPos);
  m_pickedPoints->InsertNextPoint(pickedPos);

  auto sphere = vtkSmartPointer<vtkSphereSource>::New();
  sphere->SetCenter(pickedPos);
  sphere->SetRadius(1);
  sphere->SetPhiResolution(20);
  sphere->Update();

  auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper->SetInputData(sphere->GetOutput());
  auto actor = vtkSmartPointer<vtkActor>::New();
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(0.3, 0.5, 0.8);
  m_RenderRen->AddActor(actor);
  m_pickSpheres.append(actor);
  QVTKWidget::mouseDoubleClickEvent(event);
  if (m_pickState == pickHook)
    endPickHook();
  if (m_pickState == PickPoint)
    OnEndPickPoint();
  if (m_pickState == ArcCut) {
    if (m_pickedPoints->GetNumberOfPoints() == 4)
      endArcCutPoints();
  }
  if (m_pickState == pickForSurfaceForm) {
    if (m_pickedPoints->GetNumberOfPoints() == 2) {
      m_pickState = None;
      this->setCursor(Qt::ArrowCursor);
      qDebug() << "end pick";
      emit endSurfaceFormPick();
    }
  }
}

void CustomViewWidget::keyPressEvent(QKeyEvent *event) {
  if (m_keyState != Nokey) {
    QVTKWidget::keyPressEvent(event);
    return;
  }
  if (event->key() == Qt::Key_F1) {
    m_keyState = F1;
    event->accept();
  }
  QVTKWidget::keyPressEvent(event);
}

void CustomViewWidget::keyReleaseEvent(QKeyEvent *event) {
  if (event->key() == Qt::Key_F1) {
    m_keyState = Nokey;
    event->accept();
    return;
    QVTKWidget::keyReleaseEvent(event);
  }
  QVTKWidget::keyReleaseEvent(event);
}

void CustomViewWidget::mouseMoveEvent(QMouseEvent *event) {
  if (m_keyState == F1) {
    QVTKWidget::mouseMoveEvent(event);
  }
}

void CustomViewWidget::endArcCutPoints() {
  if (m_pickState != ArcCut)
    return;
  m_pickState = None;
  this->setCursor(Qt::ArrowCursor);
  emit this->endArcCut();
}

void CustomViewWidget::endPickHook() {
  if (m_pickState != pickHook)
    return;
  m_pickState = None;
  this->setCursor(Qt::ArrowCursor);
  emit this->endHook();
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

void CustomViewWidget::OnRightButtonMenu(QPoint p) {
  m_cursorPrePos[0] = p.x();
  m_cursorPrePos[1] = p.y();
  qDebug() << m_cursorPrePos[0] << "  " << m_cursorPrePos[1] << "  "
           << mapFromGlobal(QCursor::pos()).x() << "  "
           << mapFromGlobal(QCursor::pos()).y();
  m_RightButtonMenu->exec(QCursor::pos());
}

void CustomViewWidget::OnFocusView() {
  double prePositon[3];
  m_RenderRen->GetActiveCamera()->GetFocalPoint(prePositon);

  double focusPoint[3];
  double displayPos[2];
  displayPos[0] = m_cursorPrePos[0];
  displayPos[1] = height() - m_cursorPrePos[1];
  this->GetPickPoint(displayPos, focusPoint);
  m_RenderRen->GetActiveCamera()->SetFocalPoint(focusPoint);

  double offset[3];
  vtkMath::Subtract(focusPoint, prePositon, offset);

  double cameraPosition[3];
  m_RenderRen->GetActiveCamera()->GetPosition(cameraPosition);
  vtkMath::Add(cameraPosition, offset, cameraPosition);

  m_RenderRen->GetActiveCamera()->SetPosition(cameraPosition);
  m_RenderRen->GetActiveCamera()->ParallelProjectionOn();
  m_RenderRen->ResetCameraClippingRange();
  m_RenderWin->Render();
}
void CustomViewWidget::OnBeginBSplineCurve() {
  if (m_pickState != None)
    return;
  m_pickState = BSplineCurve;
  this->setCursor(Qt::CrossCursor);
}

void CustomViewWidget::OnEndBSplineCurve() {
  if (m_pickState != BSplineCurve)
    return;
  m_pickState = None;
  this->setCursor(Qt::ArrowCursor);
  emit this->endBSplineCurve();
}

void CustomViewWidget::OnBeginSelectLoop() {
  if (m_pickState != None)
    return;
  m_pickState = SelectLoop;
  this->setCursor(Qt::CrossCursor);
}

void CustomViewWidget::OnEndSelectLoop() {
  if (m_pickState != SelectLoop)
    return;
  m_pickState = None;
  this->setCursor(Qt::ArrowCursor);
  emit this->endSelectLoop();
}

void CustomViewWidget::OnEndPickPoint() {
  if (m_pickState != PickPoint)
    return;
  m_pickState = None;
  this->setCursor(Qt::ArrowCursor);
  emit this->endPickPoint();
}

void CustomViewWidget::OnRemoveLastPoint() {
  int numOfPoint = m_pickedPoints->GetNumberOfPoints();
  if (numOfPoint == 0)
    return;
  vtkSmartPointer<vtkPoints> newPoints = vtkSmartPointer<vtkPoints>::New();

  for (vtkIdType i = 0; i < numOfPoint - 1; i++) {

    double p[3];
    m_pickedPoints->GetPoint(i, p);
    newPoints->InsertNextPoint(p);
  }
  m_pickedPoints->ShallowCopy(newPoints);
  m_RenderRen->RemoveActor(m_pickSpheres.last());
  m_pickSpheres.removeLast();
  m_RenderWin->Render();
}

void CustomViewWidget::OnPickSurfaceForm() {
  if (m_pickState != None)
    return;
  m_pickState = pickForSurfaceForm;
  this->setCursor(Qt::CrossCursor);
}

void CustomViewWidget::OnCanclePickSurfaceForm() {
  if (m_pickState != pickForSurfaceForm)
    return;
  m_pickState = None;
  this->setCursor(Qt::ArrowCursor);
  foreach (vtkActor *var, m_pickSpheres) { m_RenderRen->RemoveActor(var); }
  m_pickSpheres.clear();
  m_pickedPoints->Reset();
  m_pickedPoints->Initialize();
}
