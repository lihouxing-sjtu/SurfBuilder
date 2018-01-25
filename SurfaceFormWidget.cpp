#include "SurfaceFormWidget.h"
#include "ui_SurfaceFormWidget.h"

SurfaceFormWidget::SurfaceFormWidget(QWidget *parent, vtkRenderer *ren)
    : QWidget(parent), ui(new Ui::SurfaceFormWidget) {
  ui->setupUi(this);

  m_ForCutData = vtkSmartPointer<vtkPolyData>::New();

  m_Render = vtkSmartPointer<vtkRenderer>::New();
  m_Render = ren;

  m_ContourActor = vtkSmartPointer<vtkActor>::New();
  m_ContourActor->GetProperty()->SetColor(1, 0, 0);
  m_SurfaceActor = vtkSmartPointer<vtkActor>::New();
  m_SurfaceActor->GetProperty()->SetColor(0, 1, 0);

  m_TubesActor = vtkSmartPointer<vtkActor>::New();
  m_TubesActor->GetProperty()->SetColor(0, 0, 1);

  m_fromOrigonActor = vtkSmartPointer<vtkActor>::New();
  m_fromOrigonActor->GetProperty()->SetColor(0, 0, 1);
  m_Render->AddActor(m_fromOrigonActor);
  m_Render->AddActor(m_ContourActor);
  m_Render->AddActor(m_SurfaceActor);
  m_Render->AddActor(m_TubesActor);

  for (int i = 0; i < 3; i++) {
    m_Direction[i] = 0;
    m_StartPos[i] = 0;
    m_EndPosp[i] = 0;
    m_UpDirection[i] = 0;
    m_DownDirection[i] = 0;
  }

  m_DownHingeSurface = new TopoDS_HShape();
  m_UpHingeSurface = new TopoDS_HShape();
  m_DownOffSetDown = new TopoDS_HShape();
  m_DownOffSetUp = new TopoDS_HShape();

  m_UpOffSetUp = new TopoDS_HShape();
  m_UpOffSetDown = new TopoDS_HShape();

  m_UpOffSetUpActor = vtkSmartPointer<vtkActor>::New();
  m_UpOffSetUpActor->GetProperty()->SetColor(1, 0, 0);
  m_Render->AddActor(m_UpOffSetUpActor);

  m_UpOffSetDownActor = vtkSmartPointer<vtkActor>::New();
  m_UpOffSetDownActor->GetProperty()->SetColor(0, 1, 0);
  m_Render->AddActor(m_UpOffSetDownActor);

  m_DownOffSetUpActor = vtkSmartPointer<vtkActor>::New();
  m_DownOffSetUpActor->GetProperty()->SetColor(1, 0, 0);
  m_Render->AddActor(m_DownOffSetUpActor);

  m_DownOffSetDownActor = vtkSmartPointer<vtkActor>::New();
  m_DownOffSetDownActor->GetProperty()->SetColor(0, 1, 0);
  m_Render->AddActor(m_DownOffSetDownActor);

  connect(ui->pickTwoPointsButton, SIGNAL(clicked(bool)), this,
          SLOT(OnPickTwoPoints()));
  connect(ui->pickCancleButton, SIGNAL(clicked(bool)), this,
          SLOT(OnPickCancle()));
  connect(ui->ContourNumSpinBox, SIGNAL(valueChanged(int)), this,
          SLOT(OnChangeContourNum(int)));
  connect(ui->EndPositionSlider, SIGNAL(valueChanged(int)), this,
          SLOT(OnChangeEndPos(int)));
  connect(ui->SampleNumSpinBox, SIGNAL(valueChanged(int)), this,
          SLOT(OnChangeSampleNum(int)));
  connect(ui->StartPositionSlider, SIGNAL(valueChanged(int)), this,
          SLOT(OnChangeStartPos(int)));
  connect(ui->UpRadioButton, SIGNAL(clicked(bool)), this,
          SLOT(OnSetUpDirection()));
  connect(ui->DownRadioButton, SIGNAL(clicked(bool)), this,
          SLOT(OnSetDownDirection()));
  connect(ui->TubesVisRadioButton, SIGNAL(toggled(bool)), this,
          SLOT(OnTubesVisibility()));

  connect(ui->HingeMinUDoubleSpinBox, SIGNAL(valueChanged(double)), this,
          SLOT(OnBuildHingeRegion()));
  connect(ui->HingeMinVDoubleSpinBox, SIGNAL(valueChanged(double)), this,
          SLOT(OnBuildHingeRegion()));
  connect(ui->HingeMaxUDoubleSpinBox, SIGNAL(valueChanged(double)), this,
          SLOT(OnBuildHingeRegion()));
  connect(ui->HingeMaxVDoubleSpinBox, SIGNAL(valueChanged(double)), this,
          SLOT(OnBuildHingeRegion()));
}

SurfaceFormWidget::~SurfaceFormWidget() { delete ui; }
void SurfaceFormWidget::BuildSurface() {
  if (m_Direction[0] == 0 && m_Direction[1] == 0 && m_Direction[2] == 0)
    return;
  if (m_StartPos[0] == 0 && m_StartPos[1] == 0 && m_StartPos[2] == 0)
    return;
  if (m_EndPosp[0] == 0 && m_EndPosp[1] == 0 && m_EndPosp[2] == 0)
    return;

  qDebug() << "1";
  int numOfContours = ui->ContourNumSpinBox->value();
  int numOfSamples = ui->SampleNumSpinBox->value();
  double _startPos[3], _endPos[3];
  for (int i = 0; i < 3; i++) {
    _startPos[i] =
        m_StartPos[i] + ui->StartPositionSlider->value() * m_Direction[i];
    _endPos[i] = m_EndPosp[i] + ui->EndPositionSlider->value() * m_Direction[i];
  }
  auto appd = vtkSmartPointer<vtkAppendPolyData>::New();

  TColgp_Array2OfPnt poles(1, numOfContours, 1, numOfSamples);
  double totalDistance =
      sqrt(vtkMath::Distance2BetweenPoints(_startPos, _endPos));

  double singleDistance = totalDistance / (numOfContours - 1);
  auto cutplane = vtkSmartPointer<vtkPlane>::New();
  cutplane->SetOrigin(_startPos);
  cutplane->SetNormal(m_Direction);
  for (int i = 0; i < numOfContours; i++) {
    if (i > 0)
      cutplane->Push(singleDistance);
    auto cutPoints = vtkSmartPointer<vtkPoints>::New();
    auto contourData = vtkSmartPointer<vtkPolyData>::New();
    auto out1 = vtkSmartPointer<vtkPolyData>::New();
    bool isdown = ui->DownRadioButton->isChecked() ? 1 : 0;
    GetContourPoints(m_ForCutData, numOfSamples, cutplane, cutPoints,
                     contourData, out1, isdown);
    auto maper = vtkSmartPointer<vtkPolyDataMapper>::New();
    maper->SetInputData(out1);
    auto actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(maper);
    m_Render->AddActor(actor);
    if (cutPoints->GetNumberOfPoints() < numOfSamples) {
      qDebug() << cutPoints->GetNumberOfPoints() << numOfSamples;
      return;
    }

    appd->AddInputData(contourData);
    for (int j = 0; j < numOfSamples; j++) {
      double pt[3];
      cutPoints->GetPoint(j, pt);
      poles(i + 1, j + 1) = gp_Pnt(pt[0], pt[1], pt[2]);
    }
  }
  appd->Update();
  auto mapperContour = vtkSmartPointer<vtkPolyDataMapper>::New();
  mapperContour->SetInputData(appd->GetOutput());
  m_ContourActor->SetMapper(mapperContour);
  m_Render->GetRenderWindow()->Render();

  Handle_Geom_BezierSurface surf1 = new Geom_BezierSurface(poles);
  if (ui->DownRadioButton->isChecked())
    m_GeomSurfaceDown = surf1;
  else
    m_GeomSurfaceUp = surf1;
  TopoDS_Face aFace = BRepBuilderAPI_MakeFace(surf1, 1e-6).Face();
  m_DownHingeSurface->Shape(aFace);

  // offset up
  BRepOffset_MakeSimpleOffset myOffsetAlgo(aFace,
                                           ui->OffSetDoubleSpinBox->value());
  myOffsetAlgo.SetBuildSolidFlag(Standard_True);
  myOffsetAlgo.Perform();
  TopoDS_Shape offsetface = myOffsetAlgo.GetResultShape();
  offsetface.Reverse();

  // offset down
  BRepOffset_MakeSimpleOffset offsetdown(
      aFace, -10 * ui->OffSetDoubleSpinBox->value());
  offsetdown.SetBuildSolidFlag(Standard_True);
  offsetdown.Perform();
  TopoDS_Shape downFace = offsetdown.GetResultShape();

  if (ui->DownRadioButton->isChecked()) {
    m_DownOffSetUp->Shape(offsetface);
    m_DownOffSetDown->Shape(downFace);

    auto uppd = vtkSmartPointer<vtkPolyData>::New();
    ConvertTopoDS2PolyData(offsetface, uppd);
    auto upmapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    upmapper->SetInputData(uppd);
    m_DownOffSetUpActor->SetMapper(upmapper);

    //    auto downpd = vtkSmartPointer<vtkPolyData>::New();
    //    ConvertTopoDS2PolyData(downFace, downpd);
    //    auto downmapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    //    downmapper->SetInputData(downpd);
    //    m_DownOffSetDownActor->SetMapper(downmapper);
  }
  if (ui->UpRadioButton->isChecked()) {
    m_UpOffSetUp->Shape(offsetface);
    m_UpOffSetDown->Shape(downFace);

    auto uppd = vtkSmartPointer<vtkPolyData>::New();
    ConvertTopoDS2PolyData(offsetface, uppd);
    auto upmapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    upmapper->SetInputData(uppd);
    m_UpOffSetUpActor->SetMapper(upmapper);

    //    auto downpd = vtkSmartPointer<vtkPolyData>::New();
    //    ConvertTopoDS2PolyData(downFace, downpd);
    //    auto downmapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    //    downmapper->SetInputData(downpd);
    //    m_UpOffSetDownActor->SetMapper(downmapper);
  }
  m_Render->GetRenderWindow()->Render();
  return;

  double tubeUmin, tubeUmax, tubeVmin, tubeVmax;
  double tubeSample;
  tubeUmin = ui->TubeMinUDoubleSpinBox->value();
  tubeUmax = ui->TubeMaxUDoubleSpinBox->value();
  tubeVmin = ui->TubeMinVDoubleSpinBox->value();
  tubeVmax = ui->TubeMaxVDoubleSpinBox->value();
  tubeSample = ui->TubeSampleSpinBox->value();
  double tubeRadius = ui->TubeRadisDoubleSpinBox->value();
  double tubeHeight = ui->TubeHeightSpinBox->value();
  double Umin, Umax, Vmin, Vmax;
  surf1->Bounds(Umin, Umax, Vmin, Vmax);
  if (tubeSample > Umax || tubeSample > Vmax) {
    if (Vmax > Umax) {
      tubeSample = Umax / 10.0;
      ui->TubeSampleSpinBox->setValue(tubeSample);
    } else {
      tubeSample = Vmax / 10.0;
      ui->TubeSampleSpinBox->setValue(tubeSample);
    }
    ui->TubeMaxUDoubleSpinBox->setSingleStep(tubeSample / 5);
    ui->TubeMaxVDoubleSpinBox->setSingleStep(tubeSample / 5);
    ui->TubeMinUDoubleSpinBox->setSingleStep(tubeSample / 5);
    ui->TubeMinVDoubleSpinBox->setSingleStep(tubeSample / 5);

    ui->HingeMaxUDoubleSpinBox->setSingleStep(tubeSample / 5);
    ui->HingeMaxVDoubleSpinBox->setSingleStep(tubeSample / 5);
    ui->HingeMinUDoubleSpinBox->setSingleStep(tubeSample / 5);
    ui->HingeMinVDoubleSpinBox->setSingleStep(tubeSample / 5);
  }
  TopoDS_Shape tubesShape;
  TopoDS_Compound tubesCompound;
  BRep_Builder compoundBuilder;
  compoundBuilder.MakeCompound(tubesCompound);

  for (double m = Umin + tubeUmin; m < Umax - tubeUmax;
       m = m + Umax / 10.0 + tubeSample) {
    for (double n = Vmin + tubeVmin; n < Vmax - tubeVmax;
         n = n + Vmax / 10.0 + tubeSample) {
      double uNormal[3], vNormal[3], direction[3];

      gp_Pnt origion;
      gp_Vec diu;
      gp_Vec div;
      surf1->D1(m, n, origion, diu, div);
      uNormal[0] = diu.X();
      uNormal[1] = diu.Y();
      uNormal[2] = diu.Z();
      vtkMath::Normalize(uNormal);

      vNormal[0] = div.X();
      vNormal[1] = div.Y();
      vNormal[2] = div.Z();
      vtkMath::Normalize(vNormal);
      vtkMath::Cross(uNormal, vNormal, direction);

      double _x = origion.X() - direction[0] * tubeHeight / 2;
      double _y = origion.Y() - direction[1] * tubeHeight / 2;
      double _z = origion.Z() - direction[2] * tubeHeight / 2;
      gp_Pnt center(_x, _y, _z);

      gp_Dir dir(direction[0], direction[1], direction[2]);
      GC_MakeCircle circle(center, dir, tubeRadius);
      BRepBuilderAPI_MakeEdge circleEge(circle.Value());
      TopoDS_Edge circleShape = circleEge.Edge();
      BRepBuilderAPI_MakeWire circleWire(circleShape);
      BRepBuilderAPI_MakeFace circleFace(circleWire.Wire());
      gp_Vec v1 = dir;
      v1.Scale(tubeHeight);
      TopoDS_Shape prismFace = BRepPrimAPI_MakePrism(circleFace, v1).Shape();
      compoundBuilder.Add(tubesCompound, prismFace);

      qDebug() << _x << "   " << _y << "   " << _z << "   ";
    }
  }
  BRepAlgoAPI_Cut booleanCutter(offsetface, tubesCompound);
  booleanCutter.Build();
  offsetface = booleanCutter.Shape();
  //  auto tubesPd = vtkSmartPointer<vtkPolyData>::New();
  //  ConvertTopoDS2PolyData(tubesShape, tubesPd);
  //  auto tubesMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  //  tubesMapper->SetInputData(tubesPd);
  //  m_TubesActor->SetMapper(tubesMapper);

  auto pd = vtkSmartPointer<vtkPolyData>::New();
  ConvertTopoDS2PolyData(offsetface, pd);
  auto mapperSurface = vtkSmartPointer<vtkPolyDataMapper>::New();
  mapperSurface->SetInputData(pd);
  m_SurfaceActor->SetMapper(mapperSurface);

  m_Render->GetRenderWindow()->Render();
}

void SurfaceFormWidget::ConvertTopoDS2PolyData(TopoDS_Shape input,
                                               vtkPolyData *output) {
  IVtkOCC_Shape::Handle aShapeImpl = new IVtkOCC_Shape(input);
  auto DS = vtkSmartPointer<IVtkTools_ShapeDataSource>::New();
  DS->SetShape(aShapeImpl);
  DS->Update();

  output->DeepCopy(DS->GetOutput());
}
void SurfaceFormWidget::GetContourPoints(vtkPolyData *inputData, int sampleNum,
                                         vtkPlane *cutplane, vtkPoints *output,
                                         vtkPolyData *outData,
                                         vtkPolyData *outdata1, bool down) {

  //  double xAxis[3], yAxis[3];
  //  vtkMath::Perpendiculars(cutplane->GetNormal(), xAxis, yAxis, 0);
  //  double center[3];
  //  cutplane->GetOrigin(center);
  //  double point1[3], point2[3], origin[3];
  //  for (int i = 0; i < 3; i++) {
  //    point1[i] = center[i] - 100 * xAxis[i] + 100 * yAxis[i];
  //    point2[i] = center[i] + 100 * xAxis[i] - 100 * yAxis[i];
  //    origin[i] = center[i] - 100 * xAxis[i] - 100 * yAxis[i];
  //  }
  //  auto planesource = vtkSmartPointer<vtkPlaneSource>::New();
  //  planesource->SetOrigin(origin);
  //  planesource->SetPoint1(point1);
  //  planesource->SetPoint2(point2);
  //  planesource->Update();
  //  auto triangleFilter = vtkSmartPointer<vtkTriangleFilter>::New();
  //  triangleFilter->SetInputData(planesource->GetOutput());
  //  triangleFilter->Update();
  //  auto normalFilter = vtkSmartPointer<vtkPolyDataNormals>::New();
  //  normalFilter->SetInputData(triangleFilter->GetOutput());
  //  normalFilter->ComputeCellNormalsOn();
  //  normalFilter->ComputePointNormalsOn();
  //  normalFilter->Update();

  //  auto intersectFilter =
  //  vtkSmartPointer<vtkIntersectionPolyDataFilter>::New();
  //  intersectFilter->SetInputData(0, inputData);
  //  intersectFilter->SetInputData(1, normalFilter->GetOutput());
  //  intersectFilter->Update();
  auto cutter = vtkSmartPointer<vtkCutter>::New();
  cutter->SetInputData(inputData);
  cutter->SetCutFunction(cutplane);
  cutter->Update();
  auto stripper = vtkSmartPointer<vtkStripper>::New();
  stripper->SetInputData(cutter->GetOutput());
  stripper->Update();
  auto testpoints = vtkSmartPointer<vtkPoints>::New();
  testpoints->Initialize();
  int numoflines = stripper->GetOutput()->GetNumberOfLines();
  auto inputpoints = vtkSmartPointer<vtkPoints>::New();
  inputpoints = stripper->GetOutput()->GetPoints();
  auto stripcells = vtkSmartPointer<vtkCellArray>::New();
  stripcells = stripper->GetOutput()->GetLines();
  vtkIdType *indices;
  vtkIdType numberOfPoints;
  unsigned int lineCount = 0;
  for (stripcells->InitTraversal();
       stripcells->GetNextCell(numberOfPoints, indices); lineCount++) {
    for (vtkIdType i = 0; i < numberOfPoints; i++) {
      double point[3];
      inputpoints->GetPoint(indices[i], point);
      testpoints->InsertNextPoint(point);
    }
  }
  // begin reverse
  double direction1_m[3], direction2_m[3], direction1m2[3];
  double p1[3], pm[3], p2[3];
  int numofpoints = testpoints->GetNumberOfPoints();
  testpoints->GetPoint(0, p1);
  testpoints->GetPoint(numofpoints / 2, pm);
  testpoints->GetPoint(numofpoints - 1, p2);
  for (int i = 0; i < 3; i++) {
    direction1_m[i] = p1[i] - pm[i];
    direction2_m[i] = p2[i] - pm[i];
  }
  vtkMath::Normalize(direction1_m);
  vtkMath::Normalize(direction2_m);

  double updirection[3];
  vtkMath::Cross(m_UpDirection, m_DownDirection, updirection);
  double upangle = vtkMath::DegreesFromRadians(
      vtkMath::AngleBetweenVectors(updirection, direction1_m));
  if (upangle > 90) {
    int numofpoints = testpoints->GetNumberOfPoints();
    auto newtestpoints = vtkSmartPointer<vtkPoints>::New();
    newtestpoints->Initialize();
    for (int i = 0; i < numofpoints; i++) {
      double pt[3];
      testpoints->GetPoint(numofpoints - 1 - i, pt);
      newtestpoints->InsertNextPoint(pt);
    }
    qDebug() << "reversed";
    testpoints->DeepCopy(newtestpoints);
  }

  // end reverse
  auto cells = vtkSmartPointer<vtkCellArray>::New();
  cells->Initialize();
  auto idlist = vtkSmartPointer<vtkIdList>::New();
  idlist->Initialize();
  for (int i = 0; i < testpoints->GetNumberOfPoints(); i++) {
    idlist->InsertNextId(i);
  }
  cells->InsertNextCell(idlist);
  auto pd = vtkSmartPointer<vtkPolyData>::New();
  pd->SetLines(cells);
  pd->SetPoints(testpoints);

  auto spline = vtkSmartPointer<vtkParametricSpline>::New();
  spline->SetPoints(testpoints);
  spline->SetLeftConstraint(2);
  spline->SetLeftValue(0);
  spline->SetRightConstraint(2);
  spline->SetRightValue(0);
  spline->ClosedOff();

  auto functionSource = vtkSmartPointer<vtkParametricFunctionSource>::New();
  functionSource->SetParametricFunction(spline);
  functionSource->SetUResolution(sampleNum - 1);
  functionSource->Update();

  auto totalPoints = vtkSmartPointer<vtkPoints>::New();
  totalPoints = functionSource->GetOutput()->GetPoints();
  outData->DeepCopy(pd);
  output->DeepCopy(totalPoints);
  qDebug() << cutter->GetOutput()->GetNumberOfLines();
}

void SurfaceFormWidget::SetGP(gp_Pnt &gp, double p[]) {
  gp.SetX(p[0]);
  gp.SetY(p[1]);
  gp.SetZ(p[2]);
}

void SurfaceFormWidget::GetGP(gp_Pnt gp, double p[]) {
  p[0] = gp.X();
  p[1] = gp.Y();
  p[2] = gp.Z();
}

void SurfaceFormWidget::OnChangeContourNum(int) { BuildSurface(); }

void SurfaceFormWidget::OnChangeEndPos(int) { BuildSurface(); }

void SurfaceFormWidget::OnChangeSampleNum(int) { BuildSurface(); }

void SurfaceFormWidget::OnChangeStartPos(int) { BuildSurface(); }

void SurfaceFormWidget::OnPickTwoPoints() { emit pickTwoPoint(); }

void SurfaceFormWidget::OnPickCancle() { emit canclePick(); }

void SurfaceFormWidget::OnSetUpDirection() {
  for (int i = 0; i < 3; i++)
    m_Direction[i] = m_UpDirection[i];
  vtkMath::Normalize(m_Direction);
}

void SurfaceFormWidget::OnSetDownDirection() {
  for (int i = 0; i < 3; i++)
    m_Direction[i] = m_DownDirection[i];
  vtkMath::Normalize(m_Direction);
}

void SurfaceFormWidget::OnTubesVisibility() {
  if (ui->TubesVisRadioButton->isChecked()) {
    m_TubesActor->VisibilityOn();
  } else {
    m_TubesActor->VisibilityOff();
  }
  m_Render->GetRenderWindow()->Render();
}

void SurfaceFormWidget::OnBuildHingeRegion() {
  Standard_Real vmin = ui->HingeMinVDoubleSpinBox->value();
  Standard_Real vmax = ui->HingeMaxVDoubleSpinBox->value();

  Standard_Real umin = ui->HingeMinUDoubleSpinBox->value();
  Standard_Real umax = ui->HingeMaxUDoubleSpinBox->value();

  //  TopoDS_Face aface =
  //      BRepBuilderAPI_MakeFace(m_GeomSurfaceDown, umin, umax, vmin, vmax,
  //      1e-6);
  gp_Pnt gp1, gp2, gp3, gp4;

  if (ui->DownRadioButton->isChecked()) {
    double p1[3], p2[3], p3[3], p4[3];
    gp1 = m_GeomSurfaceDown->Value(umin, vmin);
    gp2 = m_GeomSurfaceDown->Value(umin, vmax);
    gp3 = m_GeomSurfaceDown->Value(umax, vmax);
    gp4 = m_GeomSurfaceDown->Value(umax, vmin);
    GetGP(gp1, m_DownWirePoints[0]);
    GetGP(gp2, m_DownWirePoints[1]);
    GetGP(gp3, m_DownWirePoints[2]);
    GetGP(gp4, m_DownWirePoints[3]);
    double axisv[3], axisu[3];
    for (int i = 0; i < 3; i++) {
      m_DownCenter[i] = (m_DownWirePoints[0][i] + m_DownWirePoints[1][i] +
                         m_DownWirePoints[2][i] + m_DownWirePoints[3][i]) /
                        4;
      axisv[i] = m_DownWirePoints[0][i] - m_DownWirePoints[1][i];
      axisu[i] = m_DownWirePoints[2][i] - m_DownWirePoints[1][i];
    }
    vtkMath::Normalize(axisv);
    vtkMath::Normalize(axisu);
    vtkMath::Cross(axisv, axisu, m_DownWireDir);
  } else {
    gp1 = m_GeomSurfaceUp->Value(umin, vmin);
    gp2 = m_GeomSurfaceUp->Value(umin, vmax);
    gp3 = m_GeomSurfaceUp->Value(umax, vmax);
    gp4 = m_GeomSurfaceUp->Value(umax, vmin);
    m_UpCenter[0] = (gp1.X() + gp2.X() + gp3.X() + gp4.X()) / 4;
    m_UpCenter[1] = (gp1.Y() + gp2.Y() + gp3.Y() + gp4.Y()) / 4;
    m_UpCenter[2] = (gp1.Z() + gp2.Z() + gp3.Z() + gp4.Z()) / 4;
  }

  TopoDS_Edge aEdge1 = BRepBuilderAPI_MakeEdge(gp1, gp2);
  TopoDS_Edge aEdge2 = BRepBuilderAPI_MakeEdge(gp2, gp3);
  TopoDS_Edge aEdge3 = BRepBuilderAPI_MakeEdge(gp3, gp4);
  TopoDS_Edge aEdge4 = BRepBuilderAPI_MakeEdge(gp4, gp1);

  BRepBuilderAPI_MakeWire wirebuilder;
  wirebuilder.Add(aEdge1);
  wirebuilder.Add(aEdge2);
  wirebuilder.Add(aEdge3);
  wirebuilder.Add(aEdge4);
  wirebuilder.Build();
  TopoDS_Wire aWire = wirebuilder.Wire();
  if (ui->DownRadioButton->isChecked())
    m_DownHingeSurface->Shape(aWire);
  if (ui->UpRadioButton->isChecked())
    m_UpHingeSurface->Shape(aWire);
  auto pd = vtkSmartPointer<vtkPolyData>::New();
  this->ConvertTopoDS2PolyData(aWire, pd);
  auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper->SetInputData(pd);
  m_fromOrigonActor->SetMapper(mapper);
  m_Render->GetRenderWindow()->Render();
}

void SurfaceFormWidget::SetCutData(vtkPolyData *data) {
  auto clean = vtkSmartPointer<vtkCleanPolyData>::New();
  clean->SetInputData(data);
  clean->Update();
  auto triangleFilter = vtkSmartPointer<vtkTriangleFilter>::New();
  triangleFilter->SetInputData(clean->GetOutput());
  triangleFilter->Update();
  auto normalFilter = vtkSmartPointer<vtkPolyDataNormals>::New();
  normalFilter->SetInputData(triangleFilter->GetOutput());
  normalFilter->ComputeCellNormalsOn();
  normalFilter->ComputePointNormalsOn();
  normalFilter->Update();
  m_ForCutData->DeepCopy(normalFilter->GetOutput());
}

void SurfaceFormWidget::SetDirection(double *updir, double *downdir) {
  for (int i = 0; i < 3; i++) {
    m_UpDirection[i] = updir[i];
    m_DownDirection[i] = downdir[i];
  }
  if (ui->UpRadioButton->isChecked())
    for (int i = 0; i < 3; i++) {
      m_Direction[i] = m_UpDirection[i];
      vtkMath::Normalize(m_Direction);
    }
  else
    for (int i = 0; i < 3; i++) {
      m_Direction[i] = m_DownDirection[i];
      vtkMath::Normalize(m_Direction);
    }

  // BuildSurface();
}

void SurfaceFormWidget::SetEndPos(double *pos) {
  for (int i = 0; i < 3; i++)
    m_EndPosp[i] = pos[i];
  BuildSurface();
}

void SurfaceFormWidget::SetStartPos(double *pos) {
  for (int i = 0; i < 3; i++)
    m_StartPos[i] = pos[i];
}

void SurfaceFormWidget::GetDownCenter(double dcenter[]) {
  for (int i = 0; i < 3; i++) {
    dcenter[i] = m_DownCenter[i];
  }
}

void SurfaceFormWidget::GetUpCenter(double ucenter[]) {
  for (int i = 0; i < 3; i++) {
    ucenter[i] = m_UpCenter[i];
  }
}

void SurfaceFormWidget::SetDownVisibility(bool vi) {
  m_DownOffSetUpActor->SetVisibility(vi);
  m_Render->GetRenderWindow()->Render();
}

void SurfaceFormWidget::SetUpVisibility(bool vi) {
  m_UpOffSetUpActor->SetVisibility(vi);
  m_Render->GetRenderWindow()->Render();
}

Handle(TopoDS_HShape) SurfaceFormWidget::GetDownOffSetDown() {
  return m_DownOffSetDown;
}

Handle(TopoDS_HShape) SurfaceFormWidget::GetDownOffSetUp() {
  return m_DownOffSetUp;
}

Handle(TopoDS_HShape) SurfaceFormWidget::GetUpOffSetDown() {
  return m_UpOffSetDown;
}

Handle(TopoDS_HShape) SurfaceFormWidget::GetUpOffSetUp() {
  return m_UpOffSetUp;
}

Handle(TopoDS_HShape) SurfaceFormWidget::GetUpHingeSurface() {
  return m_UpHingeSurface;
}

Handle(TopoDS_HShape) SurfaceFormWidget::GetDownHingeSurface() {
  return m_DownHingeSurface;
}
