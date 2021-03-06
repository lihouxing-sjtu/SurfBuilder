#include "NormalWidget.h"
#include "ui_NormalWidget.h"

NormalWidget::NormalWidget(QWidget *parent, vtkRenderer *ren)
    : QDialog(parent), isPickPoint(false), m_Distance(0), isArcCutStart(0),
      isArcCutEnd(0), ui(new Ui::NormalWidget) {
  ui->setupUi(this);
  this->setWindowFlags(Qt::WindowStaysOnTopHint);
  m_render = vtkSmartPointer<vtkRenderer>::New();
  m_render = ren;
  m_ArrowActor = vtkSmartPointer<vtkActor>::New();
  m_ArrowActor->GetProperty()->SetColor(0.6, 0.2, 0.4);
  m_render->AddActor(m_ArrowActor);
  m_StrechActor = vtkSmartPointer<vtkActor>::New();
  m_render->AddActor(m_StrechActor);
  m_ArcCutActor = vtkSmartPointer<vtkActor>::New();
  m_ArcCutActor->GetProperty()->SetColor(0.4, 0.2, 0.6);
  m_ArcCutActor->GetProperty()->SetOpacity(0.5);
  m_render->AddActor(m_ArcCutActor);

  m_StrechData = vtkSmartPointer<vtkPolyData>::New();
  m_ArcCutPoints = vtkSmartPointer<vtkPoints>::New();
  for (int i = 0; i < 3; i++) {
    m_direction[i] = 0;
    m_basePoint[i] = 0;
    m_OrigionDirection[i] = 0;
    m_ArcCutDirction[i] = 0;
  }
  for (int i = 0; i < 4; i++) {
    m_PlateBounds[i] = 0;
  }

  m_PlateDS = new TopoDS_HShape();
  m_TubeDS = new TopoDS_HShape();
  m_ArcCutDS = new TopoDS_HShape();
  m_FinalDS = new TopoDS_HShape();

  connect(ui->PickBaseButton, SIGNAL(clicked(bool)), this,
          SLOT(OnPickBasePoint()));
  connect(ui->ResetDirectionButton, SIGNAL(clicked(bool)), this,
          SLOT(OnResetDirection()));
  connect(ui->LeftButton, SIGNAL(clicked(bool)), this, SLOT(OnLeftButton()));
  connect(ui->RightButton, SIGNAL(clicked(bool)), this, SLOT(OnRightButton()));
  connect(ui->UpButton, SIGNAL(clicked(bool)), this, SLOT(OnUpButton()));
  connect(ui->DownButton, SIGNAL(clicked(bool)), this, SLOT(OnDownButton()));
  connect(ui->ApplyButton, SIGNAL(clicked(bool)), this, SLOT(OnApply()));
  connect(ui->UminSpinBox, SIGNAL(valueChanged(double)), this,
          SLOT(BuildPlate()));
  connect(ui->UmaxSpinBox, SIGNAL(valueChanged(double)), this,
          SLOT(BuildPlate()));
  connect(ui->VminSpinBox, SIGNAL(valueChanged(double)), this,
          SLOT(BuildPlate()));
  connect(ui->VmaxSpinBox, SIGNAL(valueChanged(double)), this,
          SLOT(BuildPlate()));
  connect(ui->SampleDoubleSpinBox, SIGNAL(valueChanged(double)), this,
          SLOT(BuildPlate()));
  connect(ui->TubeHeightSpinBox, SIGNAL(valueChanged(int)), this,
          SLOT(BuildPlate()));
  connect(ui->TubeRadiusSpinBox, SIGNAL(valueChanged(int)), this,
          SLOT(BuildPlate()));

  connect(ui->VminOffSetDoubleSpinBox, SIGNAL(valueChanged(double)), this,
          SLOT(BuildPlate()));
  connect(ui->VmaxOffSetDoubleSpinBox, SIGNAL(valueChanged(double)), this,
          SLOT(BuildPlate()));
  connect(ui->UminOffSetDoubleSpinBox, SIGNAL(valueChanged(double)), this,
          SLOT(BuildPlate()));
  connect(ui->UmaxOffSetDoubleSpinBox, SIGNAL(valueChanged(double)), this,
          SLOT(BuildPlate()));
  connect(ui->DistanceSpinBox, SIGNAL(valueChanged(double)), this,
          SLOT(BuildPlate()));
  connect(ui->ArcCutButton, SIGNAL(clicked(bool)), this, SLOT(OnArcCut()));
  connect(ui->CancleArcButton, SIGNAL(clicked(bool)), this,
          SLOT(OnCancleArcCut()));
  connect(ui->ArcCutLengthSlider, SIGNAL(valueChanged(int)), this,
          SLOT(BuildArcCut()));
  connect(ui->ArcCutPositionSlider, SIGNAL(valueChanged(int)), this,
          SLOT(BuildArcCut()));
}

NormalWidget::~NormalWidget() { delete ui; }

void NormalWidget::SetPoints(double direction[], double location[]) {
  for (int i = 0; i < 3; i++) {
    m_direction[i] = direction[i];
    m_basePoint[i] = location[i];
    m_OrigionDirection[i] = m_direction[i];
  }
  this->BuildArrow();
  this->BuildPlate();
}

double NormalWidget::GetDirectionAndDistance(double dir[]) {
  for (int i = 0; i < 3; i++) {
    dir[i] = m_direction[i];
  }
  return m_Distance;
}

void NormalWidget::SetGeomSurface(GeomPlate_MakeApprox plate, double Umin,
                                  double Umax, double Vmin, double Vmax) {
  m_GeomSurface = plate.Surface();
  m_PlateBounds[0] = Umin;
  m_PlateBounds[1] = Umax;
  m_PlateBounds[2] = Vmin;
  m_PlateBounds[3] = Vmax;
}

void NormalWidget::SetGeomSurface(Handle_Geom_Surface gs, double Umin,
                                  double Umax, double Vmin, double Vmax) {
  m_GeomSurface = gs;
  m_PlateBounds[0] = Umin;
  m_PlateBounds[1] = Umax;
  m_PlateBounds[2] = Vmin;
  m_PlateBounds[3] = Vmax;
}

Handle_TopoDS_HShape NormalWidget::GetData(vtkPolyData *out) {
  out->DeepCopy(m_StrechData);
  return m_FinalDS;
}

void NormalWidget::SetArcCutPoints(vtkPoints *pts) {
  double focusPoint[3];
  m_render->GetActiveCamera()->GetDirectionOfProjection(m_ArcCutDirction);
  vtkMath::MultiplyScalar(m_ArcCutDirction, -1);
  m_render->GetActiveCamera()->GetFocalPoint(focusPoint);
  auto plane = vtkSmartPointer<vtkPlane>::New();
  plane->SetOrigin(focusPoint);
  plane->SetNormal(m_ArcCutDirction);
  m_ArcCutPoints->Initialize();
  for (int i = 0; i < pts->GetNumberOfPoints(); i++) {
    double p[3];
    pts->GetPoint(i, p);
    plane->ProjectPoint(p, p);
    m_ArcCutPoints->InsertNextPoint(p);
    qDebug() << i;
  }
  isArcCutEnd = true;
  this->BuildArcCut();
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
  m_ArrowActor->VisibilityOn();
  m_render->GetRenderWindow()->Render();
}

void NormalWidget::BuildPlate() {
  if (m_basePoint[0] == 0 && m_basePoint[1] == 0 && m_basePoint[2] == 0) {
    return;
  }
  if (m_GeomSurface.IsNull()) {
    return;
  }
  foreach (vtkActor *ac, m_TubeActorList) { m_render->RemoveActor(ac); }
  m_TubeActorList.clear();
  m_render->GetRenderWindow()->Render();
  Standard_Real Umin, Umax, Vmin, Vmax;
  Umin = m_PlateBounds[0] + ui->UminSpinBox->value();
  Umax = m_PlateBounds[1] + ui->UmaxSpinBox->value();
  Vmin = m_PlateBounds[2] + ui->VminSpinBox->value();
  Vmax = m_PlateBounds[3] + ui->VmaxSpinBox->value();

  BRepBuilderAPI_MakeFace MF(m_GeomSurface, Umin, Umax, Vmin, Vmax, 0.01);
  TopoDS_Face face;
  face = MF.Face();

  auto facepd = vtkSmartPointer<vtkPolyData>::New();
  this->ConvertTopoDS2PolyData(face, facepd);

  auto facept = vtkSmartPointer<vtkPoints>::New();
  facept = facepd->GetPoints();
  int numOffacept = facept->GetNumberOfPoints();
  qDebug() << numOffacept;

  vtkMath::Normalize(m_direction);
  gp_Dir dir(m_direction[0], m_direction[1], m_direction[2]);
  gp_Vec v = dir;
  v.Scale(ui->DistanceSpinBox->value());
  //  BRepOffsetAPI_MakeOffsetShape myOffsetAlgo(
  //      face, -ui->DistanceSpinBox->value(), Precision::Confusion(),
  //      BRepOffset_Skin, Standard_True);
  //  myOffsetAlgo.Build();

  /*
  BRepOffset_MakeSimpleOffset myOffsetAlgo(face, ui->DistanceSpinBox->value());
   myOffsetAlgo.SetBuildSolidFlag(Standard_True);
  myOffsetAlgo.Perform();
  TopoDS_Shape offsetface = myOffsetAlgo.GetResultShape();
  m_PlateDS->Shape(offsetface);

  auto offsetpd = vtkSmartPointer<vtkPolyData>::New();
  ConvertTopoDS2PolyData(offsetface, offsetpd);
  auto offsetmapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  offsetmapper->SetInputData(offsetpd);
  m_StrechActor->SetMapper(offsetmapper);
  m_StrechActor->VisibilityOn();
  m_render->GetRenderWindow()->Render();
  */

  //
  // return;
  TopoDS_Shape solid = BRepPrimAPI_MakePrism(face, v).Shape();
  TopoDS_Shape total;
  for (double m = Umin + ui->UminOffSetDoubleSpinBox->value();
       m < Umax - ui->UmaxOffSetDoubleSpinBox->value();
       m = m + 1 + ui->SampleDoubleSpinBox->value()) {
    for (double n = Vmin + ui->VminOffSetDoubleSpinBox->value();
         n < Vmax - ui->VmaxOffSetDoubleSpinBox->value();
         n = n + 1 + ui->SampleDoubleSpinBox->value()) {
      gp_Pnt origion = m_GeomSurface->Value(m, n);
      double _x =
          origion.X() - m_direction[0] * ui->TubeHeightSpinBox->value() / 2;
      double _y =
          origion.Y() - m_direction[1] * ui->TubeHeightSpinBox->value() / 2;
      double _z =
          origion.Z() - m_direction[2] * ui->TubeHeightSpinBox->value() / 2;
      gp_Pnt center(_x, _y, _z);

      gp_Dir dir(m_direction[0], m_direction[1], m_direction[2]);
      GC_MakeCircle circle(center, dir, ui->TubeRadiusSpinBox->value());
      BRepBuilderAPI_MakeEdge circleEge(circle.Value());
      TopoDS_Edge circleShape = circleEge.Edge();
      BRepBuilderAPI_MakeWire circleWire(circleShape);
      BRepBuilderAPI_MakeFace circleFace(circleWire.Wire());
      if (total.IsNull())
        total = circleFace;
      else
        total = BRepAlgoAPI_Fuse(total, circleFace);
    }
  }
  gp_Dir dir1(m_direction[0], m_direction[1], m_direction[2]);
  gp_Vec v1 = dir1;
  v1.Scale(ui->TubeHeightSpinBox->value());
  TopoDS_Shape tube = BRepPrimAPI_MakePrism(total, v1).Shape();
  m_TubeDS->Shape(tube);
  auto data = vtkSmartPointer<vtkPolyData>::New();
  this->ConvertTopoDS2PolyData(tube, data);
  auto mapper1 = vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper1->SetInputData(data);
  auto actor = vtkSmartPointer<vtkActor>::New();
  actor->SetMapper(mapper1);
  m_render->AddActor(actor);
  m_TubeActorList.append(actor);
  // const TopoDS_Shape tubes = BRepPrimAPI_MakePrism(totalShape, v).Shape();
  m_PlateDS->Shape(solid);
  auto strechData = vtkSmartPointer<vtkPolyData>::New();
  this->ConvertTopoDS2PolyData(solid, strechData);
  auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper->SetInputData(strechData);
  m_StrechActor->SetMapper(mapper);
  m_StrechActor->VisibilityOn();
  m_render->GetRenderWindow()->Render();
}

void NormalWidget::OnArcCut() {
  if (ui->ArcCutButton->isChecked()) {
    // define the arc points
    if (!isArcCutStart) {
      isArcCutStart = true;
      emit startArcCut();
    }
  } else {
    if (!isArcCutEnd) {
      ui->ArcCutButton->setChecked(true);
      return;
    }
    qDebug() << "dadf";
    TopoDS_Shape left =
        BRepAlgoAPI_Cut(m_PlateDS->Shape(), m_ArcCutDS->Shape());
    m_PlateDS->Shape(left);

    auto plateData = vtkSmartPointer<vtkPolyData>::New();
    this->ConvertTopoDS2PolyData(m_PlateDS->Shape(), plateData);
    auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputData(plateData);

    m_StrechActor->SetMapper(mapper);
    m_StrechActor->VisibilityOn();

    isArcCutEnd = false;
    isArcCutStart = false;
    qDebug() << "dadf";
    m_ArcCutActor->VisibilityOff();
    m_render->GetRenderWindow()->Render();
  }
}

void NormalWidget::OnCancleArcCut() {
  // cancle the arc cut
  ui->ArcCutButton->setChecked(false);
  isArcCutEnd = false;
  isArcCutStart = false;
  m_ArcCutActor->VisibilityOff();
  m_render->GetRenderWindow()->Render();
  emit cancleArcCut();
}

void NormalWidget::BuildArcCut() {
  if (m_ArcCutPoints->GetNumberOfPoints() == 0)
    return;
  double lengthOfArcCut = ui->ArcCutLengthSlider->value();
  double positionOfArcCut = ui->ArcCutPositionSlider->value();
  double arcPts[4][3];
  for (int j = 0; j < 4; j++) {
    for (int i = 0; i < 3; i++) {
      arcPts[j][i] = m_ArcCutPoints->GetPoint(j)[i] +
                     positionOfArcCut * m_ArcCutDirction[i];
    }
  }
  gp_Pnt p1(arcPts[0][0], arcPts[0][1], arcPts[0][2]);
  gp_Pnt p2(arcPts[1][0], arcPts[1][1], arcPts[1][2]);
  gp_Pnt p3(arcPts[2][0], arcPts[2][1], arcPts[2][2]);
  gp_Pnt p4(arcPts[3][0], arcPts[3][1], arcPts[3][2]);

  GC_MakeCircle innerCircle(p1, p2, p3);

  GC_MakeCircle outCircle(p1, p4, p3);

  if (innerCircle.IsDone() && outCircle.IsDone()) {
    BRepBuilderAPI_MakeEdge innerCircleEdgeBuilder(innerCircle.Value());
    TopoDS_Edge innerCircleEdge = innerCircleEdgeBuilder.Edge();
    BRepBuilderAPI_MakeWire innerCirclWireBuilder(innerCircleEdge);
    BRepBuilderAPI_MakeFace innerCircleFaceBuilder(
        innerCirclWireBuilder.Wire());
    TopoDS_Shape innerFace = innerCircleFaceBuilder.Face();
    gp_Dir dir0(-m_ArcCutDirction[0], -m_ArcCutDirction[1],
                -m_ArcCutDirction[2]);
    gp_Vec v0 = dir0;
    v0.Scale(5);
    gp_Trsf T;
    T.SetTranslation(v0);
    BRepBuilderAPI_Transform theTrsf(T);
    theTrsf.Perform(innerFace);
    TopoDS_Shape newInnerFace = theTrsf.Shape();

    gp_Dir dir1(m_ArcCutDirction[0], m_ArcCutDirction[1], m_ArcCutDirction[2]);
    gp_Vec v1 = dir1;
    v1.Scale(lengthOfArcCut);

    gp_Vec v2 = dir1;
    v2.Scale(lengthOfArcCut + 10);

    TopoDS_Shape innerSolid = BRepPrimAPI_MakePrism(newInnerFace, v2).Shape();

    BRepBuilderAPI_MakeEdge outCircleEdgeBuilder(outCircle.Value());
    TopoDS_Edge outCircleEdge = outCircleEdgeBuilder.Edge();
    BRepBuilderAPI_MakeWire outCirclWireBuilder(outCircleEdge);
    BRepBuilderAPI_MakeFace outCircleFaceBuilder(outCirclWireBuilder.Wire());
    TopoDS_Shape outFace = outCircleFaceBuilder.Face();
    TopoDS_Shape outSolid = BRepPrimAPI_MakePrism(outFace, v1).Shape();

    // boolean operation
    m_ArcCutDS->Shape(BRepAlgoAPI_Cut(outSolid, innerSolid));
    auto arcpd = vtkSmartPointer<vtkPolyData>::New();
    this->ConvertTopoDS2PolyData(m_ArcCutDS->Shape(), arcpd);
    auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputData(arcpd);
    m_ArcCutActor->SetMapper(mapper);
    m_ArcCutActor->VisibilityOn();
    m_render->GetRenderWindow()->Render();
  }
}

void NormalWidget::ConvertTopoDS2PolyData(TopoDS_Shape input,
                                          vtkPolyData *output) {
  IVtkOCC_Shape::Handle aShapeImpl = new IVtkOCC_Shape(input);
  auto DS = vtkSmartPointer<IVtkTools_ShapeDataSource>::New();
  DS->SetShape(aShapeImpl);
  DS->Update();

  output->DeepCopy(DS->GetOutput());
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
  this->BuildPlate();
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
  this->BuildPlate();
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
  this->BuildPlate();
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
  this->BuildPlate();
}

void NormalWidget::OnPickBasePoint() { emit pickBasePoint(); }

void NormalWidget::OnResetDirection() {
  for (int i = 0; i < 3; i++) {
    m_direction[i] = m_OrigionDirection[i];
  }
  this->BuildArrow();
}

void NormalWidget::OnApply() {
  m_FinalDS->Shape(BRepAlgoAPI_Cut(m_PlateDS->Shape(), m_TubeDS->Shape()));
  BRepFilletAPI_MakeFillet filet(m_FinalDS->Shape());
  TopExp_Explorer ex(m_FinalDS->Shape(), TopAbs_EDGE);
  while (ex.More()) {
    filet.Add(ui->FilletSpinBox->value(), TopoDS::Edge(ex.Current()));
    ex.Next();
  }
  try {
    filet.Build();
  } catch (Standard_Failure) {
    Handle(Standard_Failure) E = Standard_Failure::Caught();
    qDebug() << "failed";
  }

  if (filet.IsDone()) {
    m_FinalDS->Shape(filet.Shape());
    this->ConvertTopoDS2PolyData(m_FinalDS->Shape(), m_StrechData);
    m_StrechData->BuildCells();
    m_StrechData->GetLines()->Reset();
    auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputData(m_StrechData);
    m_StrechActor->SetMapper(mapper);
    m_StrechActor->VisibilityOff();
    foreach (vtkActor *ac, m_TubeActorList) { m_render->RemoveActor(ac); }
    m_TubeActorList.clear();
    m_ArrowActor->VisibilityOff();
    m_render->GetRenderWindow()->Render();
    m_Distance = ui->DistanceSpinBox->value();
    emit strechDone();
  } else
    qDebug() << "failed";
}
