#include "NormalWidget.h"
#include "ui_NormalWidget.h"

NormalWidget::NormalWidget(QWidget *parent, vtkRenderer *ren)
    : QDialog(parent), isPickPoint(false), m_Distance(0),
      ui(new Ui::NormalWidget) {
  ui->setupUi(this);
  this->setWindowFlags(Qt::WindowStaysOnTopHint);
  m_render = vtkSmartPointer<vtkRenderer>::New();
  m_render = ren;
  m_ArrowActor = vtkSmartPointer<vtkActor>::New();
  m_ArrowActor->GetProperty()->SetColor(0.6, 0.2, 0.4);
  m_render->AddActor(m_ArrowActor);

  for (int i = 0; i < 3; i++) {
    m_direction[i] = 0;
    m_basePoint[i] = 0;
    m_OrigionDirection[i] = 0;
  }
  connect(ui->PickBaseButton, SIGNAL(clicked(bool)), this,
          SLOT(OnPickBasePoint()));
  connect(ui->ResetDirectionButton, SIGNAL(clicked(bool)), this,
          SLOT(OnResetDirection()));
  connect(ui->LeftButton, SIGNAL(clicked(bool)), this, SLOT(OnLeftButton()));
  connect(ui->RightButton, SIGNAL(clicked(bool)), this, SLOT(OnRightButton()));
  connect(ui->UpButton, SIGNAL(clicked(bool)), this, SLOT(OnUpButton()));
  connect(ui->DownButton, SIGNAL(clicked(bool)), this, SLOT(OnDownButton()));
  connect(ui->ApplyButton, SIGNAL(clicked(bool)), this, SLOT(OnApply()));
}

NormalWidget::~NormalWidget() { delete ui; }

void NormalWidget::SetPoints(double direction[], double location[]) {
  for (int i = 0; i < 3; i++) {
    m_direction[i] = direction[i];
    m_basePoint[i] = location[i];
    m_OrigionDirection[i] = m_direction[i];
  }
  this->BuildArrow();
}

double NormalWidget::GetDirectionAndDistance(double dir[]) {
  for (int i = 0; i < 3; i++) {
    dir[i] = m_direction[i];
  }
  return m_Distance;
}

void NormalWidget::GetArrow(double startPt[], double direction[], double length,
                            vtkPolyData *output) {
  double normalizedX[3];
  for (int i = 0; i < 3; i++) {
    normalizedX[i] = direction[i];
  }
  vtkMath::Normalize(normalizedX);

  double arbitrary[3];
  double normalizedY[3];
  double normalizedZ[3];
  arbitrary[0] = vtkMath::Random(-10, 10);
  arbitrary[1] = vtkMath::Random(-10, 10);
  arbitrary[2] = vtkMath::Random(-10, 10);
  vtkMath::Cross(normalizedX, arbitrary, normalizedZ);
  vtkMath::Normalize(normalizedZ);

  // The Y axis is Z cross X
  vtkMath::Cross(normalizedZ, normalizedX, normalizedY);
  vtkSmartPointer<vtkMatrix4x4> matrix = vtkSmartPointer<vtkMatrix4x4>::New();
  // Create the direction cosine matrix
  matrix->Identity();
  for (unsigned int i = 0; i < 3; i++) {
    matrix->SetElement(i, 0, normalizedX[i]);
    matrix->SetElement(i, 1, normalizedY[i]);
    matrix->SetElement(i, 2, normalizedZ[i]);
  }

  auto arrowSource = vtkSmartPointer<vtkArrowSource>::New();
  // Apply the transforms
  vtkSmartPointer<vtkTransform> transform =
      vtkSmartPointer<vtkTransform>::New();
  transform->Translate(startPt);
  transform->Concatenate(matrix);
  transform->Scale(length, length, length);

  // Transform the polydata
  vtkSmartPointer<vtkTransformPolyDataFilter> transformPD =
      vtkSmartPointer<vtkTransformPolyDataFilter>::New();
  transformPD->SetTransform(transform);
  transformPD->SetInputConnection(arrowSource->GetOutputPort());
  transformPD->Update();
  output->DeepCopy(transformPD->GetOutput());
}

void NormalWidget::BuildArrow() {
  auto arrowData = vtkSmartPointer<vtkPolyData>::New();
  this->GetArrow(m_basePoint, m_direction, 20, arrowData);
  auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper->SetInputData(arrowData);

  m_ArrowActor->SetMapper(mapper);
  m_render->GetRenderWindow()->Render();
}

void NormalWidget::OnLeftButton() {
  double angle = ui->SingleAngleBox->value();
  double direction[3];
  m_render->GetActiveCamera()->GetViewUp(direction);
  auto transform = vtkSmartPointer<vtkTransform>::New();
  transform->RotateWXYZ(-angle, direction);
  transform->Update();
  double transDirection[3];
  for (int i = 0; i < 3; i++) {
    transDirection[i] = transform->TransformDoubleVector(m_direction)[i];
  }
  for (int i = 0; i < 3; i++) {
    m_direction[i] = transDirection[i];
  }
  this->BuildArrow();
}

void NormalWidget::OnRightButton() {
  double angle = ui->SingleAngleBox->value();
  double direction[3];
  m_render->GetActiveCamera()->GetViewUp(direction);
  vtkMath::MultiplyScalar(direction, -1);
  auto transform = vtkSmartPointer<vtkTransform>::New();
  transform->RotateWXYZ(-angle, direction);
  transform->Update();
  double transDirection[3];
  for (int i = 0; i < 3; i++) {
    transDirection[i] = transform->TransformDoubleVector(m_direction)[i];
  }
  for (int i = 0; i < 3; i++) {
    m_direction[i] = transDirection[i];
  }
  this->BuildArrow();
}

void NormalWidget::OnUpButton() {
  double angle = ui->SingleAngleBox->value();
  double direction[3], viewUp[3], viewRight[3];
  m_render->GetActiveCamera()->GetViewUp(viewUp);
  m_render->GetActiveCamera()->GetDirectionOfProjection(direction);
  vtkMath::MultiplyScalar(direction, -1);
  vtkMath::Cross(viewUp, direction, viewRight);

  auto transform = vtkSmartPointer<vtkTransform>::New();
  transform->RotateWXYZ(-angle, viewRight);
  transform->Update();
  double transDirection[3];
  for (int i = 0; i < 3; i++) {
    transDirection[i] = transform->TransformDoubleVector(m_direction)[i];
  }
  for (int i = 0; i < 3; i++) {
    m_direction[i] = transDirection[i];
  }
  this->BuildArrow();
}

void NormalWidget::OnDownButton() {
  double angle = ui->SingleAngleBox->value();
  double direction[3], viewUp[3], viewRight[3];
  m_render->GetActiveCamera()->GetViewUp(viewUp);
  m_render->GetActiveCamera()->GetDirectionOfProjection(direction);
  vtkMath::MultiplyScalar(direction, -1);
  vtkMath::Cross(viewUp, direction, viewRight);

  auto transform = vtkSmartPointer<vtkTransform>::New();
  transform->RotateWXYZ(angle, viewRight);
  transform->Update();
  double transDirection[3];
  for (int i = 0; i < 3; i++) {
    transDirection[i] = transform->TransformDoubleVector(m_direction)[i];
  }
  for (int i = 0; i < 3; i++) {
    m_direction[i] = transDirection[i];
  }
  this->BuildArrow();
}

void NormalWidget::OnPickBasePoint() { emit pickBasePoint(); }

void NormalWidget::OnResetDirection() {
  for (int i = 0; i < 3; i++) {
    m_direction[i] = m_OrigionDirection[i];
  }
  this->BuildArrow();
}

void NormalWidget::OnApply() {
  m_Distance = ui->DistanceSpinBox->value();
  emit strechDirection();
}
