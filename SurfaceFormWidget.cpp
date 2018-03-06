#include "SurfaceFormWidget.h"
#include "ui_SurfaceFormWidget.h"

SurfaceFormWidget::SurfaceFormWidget(QWidget *parent, vtkRenderer *ren)
    : QWidget(parent), ui(new Ui::SurfaceFormWidget) {
  ui->setupUi(this);
  this->setWindowFlag(Qt::WindowStaysOnTopHint);
  m_ForCutData = vtkSmartPointer<vtkPolyData>::New();
  m_ForCutActor = vtkSmartPointer<vtkActor>::New();

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

  m_Belt1Actor = vtkSmartPointer<vtkActor>::New();
  m_Belt1Actor->GetProperty()->SetColor(0.2, 0.4, 0.6);
  m_Render->AddActor(m_Belt1Actor);

  m_Belt2Actor = vtkSmartPointer<vtkActor>::New();
  m_Belt2Actor->GetProperty()->SetColor(0.6, 0.4, 0.2);
  m_Render->AddActor(m_Belt2Actor);

  m_Belt3Actor = vtkSmartPointer<vtkActor>::New();
  m_Belt3Actor->GetProperty()->SetColor(0.3, 0.1, 0.7);
  m_Render->AddActor(m_Belt3Actor);

  m_Belt4Actor = vtkSmartPointer<vtkActor>::New();
  m_Belt4Actor->GetProperty()->SetColor(0.7, 0.1, 0.3);
  m_Render->AddActor(m_Belt4Actor);

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

  m_DownBelt1 = new TopoDS_HShape();
  m_DownBelt2 = new TopoDS_HShape();
  m_DownBelt3 = new TopoDS_HShape();
  m_DownBelt4 = new TopoDS_HShape();

  m_ElbowGuideShape = new TopoDS_HShape();
  m_WristGuideShape = new TopoDS_HShape();

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

  m_ElbowHookActor = vtkSmartPointer<vtkActor>::New();
  m_ElbowHookActor->GetProperty()->SetColor(0, 0, 1);
  m_Render->AddActor(m_ElbowHookActor);

  m_WristHookActor = vtkSmartPointer<vtkActor>::New();
  m_WristHookActor->GetProperty()->SetColor(0, 0, 1);
  m_Render->AddActor(m_WristHookActor);

  connect(ui->pickTwoPointsButton, SIGNAL(clicked(bool)), this,
          SLOT(OnPickTwoPoints()));
  connect(ui->pickCancleButton, SIGNAL(clicked(bool)), this,
          SLOT(OnPickCancle()));

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

  connect(ui->TubeHeightSpinBox, SIGNAL(valueChanged(int)), this,
          SLOT(OnBuildTubeRegion()));
  connect(ui->TubeMaxUDoubleSpinBox, SIGNAL(valueChanged(double)), this,
          SLOT(OnBuildTubeRegion()));
  connect(ui->TubeMaxVDoubleSpinBox, SIGNAL(valueChanged(double)), this,
          SLOT(OnBuildTubeRegion()));
  connect(ui->TubeMinUDoubleSpinBox, SIGNAL(valueChanged(double)), this,
          SLOT(OnBuildTubeRegion()));
  connect(ui->TubeMinVDoubleSpinBox, SIGNAL(valueChanged(double)), this,
          SLOT(OnBuildTubeRegion()));

  connect(ui->PickHookPointButton, SIGNAL(clicked(bool)), this,
          SLOT(OnPickHookPoint()));
  connect(ui->HookHightSpinBox, SIGNAL(valueChanged(int)), this,
          SLOT(BuildHook()));
  connect(ui->HookLengthSlider, SIGNAL(valueChanged(int)), this,
          SLOT(BuildHook()));
  connect(ui->HookRadiusDoubleSpinBox, SIGNAL(valueChanged(double)), this,
          SLOT(BuildHook()));
  connect(ui->HookRotateHightSpinBox, SIGNAL(valueChanged(int)), this,
          SLOT(BuildHook()));
  connect(ui->HookRotationSlider, SIGNAL(valueChanged(int)), this,
          SLOT(BuildHook()));
  connect(ui->HookGuideRadiusDoubleSpinBox, SIGNAL(valueChanged(double)), this,
          SLOT(BuildHook()));
  connect(ui->HookSupportHightSpinBox, SIGNAL(valueChanged(int)), this,
          SLOT(BuildHook()));

  connect(ui->HookCubeXSpinBox, SIGNAL(valueChanged(int)), this,
          SLOT(BuildHook()));
  connect(ui->HookCubeYSpinBox, SIGNAL(valueChanged(int)), this,
          SLOT(BuildHook()));
  connect(ui->HookCubeZSpinBox, SIGNAL(valueChanged(int)), this,
          SLOT(BuildHook()));

  connect(ui->HookRotateLeftButton, SIGNAL(clicked(bool)), this,
          SLOT(OnHookRotateLeft()));
  connect(ui->HookRotateRightButton, SIGNAL(clicked(bool)), this,
          SLOT(OnHookRotateRight()));
  connect(ui->HookMoveUpButton, SIGNAL(clicked(bool)), this,
          SLOT(OnHookMoveUp()));
  connect(ui->HookMoveDownButton, SIGNAL(clicked(bool)), this,
          SLOT(OnHookMoveDown()));
  connect(ui->HookMoveLeftButton, SIGNAL(clicked(bool)), this,
          SLOT(OnHookMoveLeft()));
  connect(ui->HookMoveRightButton, SIGNAL(clicked(bool)), this,
          SLOT(OnHookMoveRight()));

  connect(ui->ElbowRadioButton, SIGNAL(clicked(bool)), this,
          SLOT(OnElbowRadioButton()));
  connect(ui->WristRadioButton, SIGNAL(clicked(bool)), this,
          SLOT(OnWristRadioButton()));
  SetBeltConnet(ui->Belt1MaxUDoubleSpinBox);
  SetBeltConnet(ui->Belt1MaxVDoubleSpinBox);
  SetBeltConnet(ui->Belt1MinUDoubleSpinBox);
  SetBeltConnet(ui->Belt1MinVDoubleSpinBox);

  SetBeltConnet(ui->Belt2MaxUDoubleSpinBox);
  SetBeltConnet(ui->Belt2MaxVDoubleSpinBox);
  SetBeltConnet(ui->Belt2MinUDoubleSpinBox);
  SetBeltConnet(ui->Belt2MinVDoubleSpinBox);

  SetBeltConnet(ui->Belt3MaxUDoubleSpinBox);
  SetBeltConnet(ui->Belt3MaxVDoubleSpinBox);
  SetBeltConnet(ui->Belt3MinUDoubleSpinBox);
  SetBeltConnet(ui->Belt3MinVDoubleSpinBox);

  SetBeltConnet(ui->Belt4MaxUDoubleSpinBox);
  SetBeltConnet(ui->Belt4MaxVDoubleSpinBox);
  SetBeltConnet(ui->Belt4MinUDoubleSpinBox);
  SetBeltConnet(ui->Belt4MinVDoubleSpinBox);

  InitializeContourWidget();
  connect(ui->ContourSpinBox, SIGNAL(valueChanged(int)), this,
          SLOT(OnContourSpinboxChanged()));
  connect(ui->ApplyButton, SIGNAL(clicked(bool)), this, SLOT(BuildSurface()));
}

SurfaceFormWidget::~SurfaceFormWidget() { delete ui; }
void SurfaceFormWidget::BuildSurface() {
  if (m_ContourPointsList.size() < 10)
    return;
  int numOfContours = 10;
  int numOfSamples = ui->SampleNumSpinBox->value();
  TColgp_Array2OfPnt poles(1, numOfContours, 1, numOfSamples);
  for (int i = 0; i < numOfContours; i++) {
    auto spline = vtkSmartPointer<vtkParametricSpline>::New();
    spline->SetPoints(m_ContourPointsList.at(i));
    spline->SetLeftConstraint(2);
    spline->SetLeftValue(0);
    spline->SetRightConstraint(2);
    spline->SetRightValue(0);
    spline->ClosedOff();

    auto functionSource = vtkSmartPointer<vtkParametricFunctionSource>::New();
    functionSource->SetParametricFunction(spline);
    functionSource->SetUResolution(numOfSamples - 1);
    functionSource->Update();

    for (int j = 0; j < numOfSamples; j++) {
      double pt[3];
      functionSource->GetOutput()->GetPoints()->GetPoint(j, pt);
      poles(i + 1, j + 1) = gp_Pnt(pt[0], pt[1], pt[2]);
    }
  }

  Handle_Geom_BezierSurface surf1 = new Geom_BezierSurface(poles);
  if (ui->DownRadioButton->isChecked())
    m_GeomSurfaceDown = surf1;
  else
    m_GeomSurfaceUp = surf1;
  TopoDS_Face aFace = BRepBuilderAPI_MakeFace(surf1, 1e-6).Face();
  aFace.Reverse();
  m_DownHingeSurface->Shape(aFace);

  if (ui->DownRadioButton->isChecked()) {
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
    // offset DOWN
    BRepOffset_MakeSimpleOffset myOffsetAlgo(
        aFace, -10 * ui->OffSetDoubleSpinBox->value());
    myOffsetAlgo.SetBuildSolidFlag(Standard_True);
    myOffsetAlgo.Perform();
    TopoDS_Shape downFace = myOffsetAlgo.GetResultShape();
    // downFace.Reverse();
    // offset UP
    BRepOffset_MakeSimpleOffset offsetdown(aFace,
                                           ui->OffSetDoubleSpinBox->value());
    offsetdown.SetBuildSolidFlag(Standard_True);
    offsetdown.Perform();
    TopoDS_Shape offsetface = offsetdown.GetResultShape();
    offsetface.Reverse();
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
  //  BRepAlgoAPI_Cut booleanCutter(offsetface, tubesCompound);
  //  booleanCutter.Build();
  //  offsetface = booleanCutter.Shape();
  //  //  auto tubesPd = vtkSmartPointer<vtkPolyData>::New();
  //  //  ConvertTopoDS2PolyData(tubesShape, tubesPd);
  //  //  auto tubesMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  //  //  tubesMapper->SetInputData(tubesPd);
  //  //  m_TubesActor->SetMapper(tubesMapper);

  //  auto pd = vtkSmartPointer<vtkPolyData>::New();
  //  ConvertTopoDS2PolyData(offsetface, pd);
  //  auto mapperSurface = vtkSmartPointer<vtkPolyDataMapper>::New();
  //  mapperSurface->SetInputData(pd);
  //  m_SurfaceActor->SetMapper(mapperSurface);

  //  m_Render->GetRenderWindow()->Render();
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
  double cupdirection[3];
  vtkMath::Cross(direction1_m, m_DownDirection, cupdirection);
  vtkMath::Cross(m_UpDirection, m_DownDirection, updirection);
  double upangle = vtkMath::DegreesFromRadians(
      vtkMath::AngleBetweenVectors(updirection, cupdirection));
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

TopoDS_Shape SurfaceFormWidget::CalculateBeltShape(double uvRegion[],
                                                   double height,
                                                   QDoubleSpinBox *uspin,
                                                   QDoubleSpinBox *vspin) {
  Handle(Geom_Surface) surf;
  if (ui->DownRadioButton->isChecked())
    surf = m_GeomSurfaceDown;
  else
    surf = m_GeomSurfaceUp;
  // begin direction
  double dirPos[2];
  dirPos[0] = (uvRegion[0] + uvRegion[1]) / 2;
  dirPos[1] = (uvRegion[2] + uvRegion[3]) / 2;
  double udir[3], vdir[3], movedir[3];
  gp_Pnt origion;
  gp_Vec diu;
  gp_Vec div;
  surf->D1(dirPos[0], dirPos[1], origion, diu, div);
  udir[0] = diu.X();
  udir[1] = diu.Y();
  udir[2] = diu.Z();
  vtkMath::Normalize(udir);
  vdir[0] = div.X();
  vdir[1] = div.Y();
  vdir[2] = div.Z();
  vtkMath::Normalize(vdir);
  vtkMath::Cross(udir, vdir, movedir);
  // end direction

  // begin move
  gp_Pnt p1, p2, p3, p4;
  gp_Pnt tp1, tp2, tp3, tp4;
  p1 = surf->Value(uvRegion[0], uvRegion[2]); // umin vmin
  p2 = surf->Value(uvRegion[0], uvRegion[3]); // umin vmax
  p3 = surf->Value(uvRegion[1], uvRegion[2]); // umax vmin
  p4 = surf->Value(uvRegion[1], uvRegion[3]); // umax vmax
  gp_Vec moveVec(gp_Dir(movedir[0], movedir[1], movedir[2]));
  moveVec.Scale(height / 2);

  double up1[3], up2[3];
  double vp1[3], vp2[3];

  GetGP(p1, up1);
  GetGP(p3, up2);
  double distanceu = sqrt(vtkMath::Distance2BetweenPoints(up1, up2));
  uspin->setValue(distanceu);
  GetGP(p1, vp1);
  GetGP(p2, vp2);
  double distancev = sqrt(vtkMath::Distance2BetweenPoints(vp1, vp2));
  vspin->setValue(distancev);

  p1.Translate(moveVec);
  p2.Translate(moveVec);
  p3.Translate(moveVec);
  p4.Translate(moveVec);
  // end move

  // begin face
  TopoDS_Edge edge1 = BRepBuilderAPI_MakeEdge(p1, p2);
  TopoDS_Edge edge2 = BRepBuilderAPI_MakeEdge(p2, p3);
  TopoDS_Edge edge3 = BRepBuilderAPI_MakeEdge(p3, p1);

  TopoDS_Edge edge4 = BRepBuilderAPI_MakeEdge(p3, p2);
  TopoDS_Edge edge5 = BRepBuilderAPI_MakeEdge(p2, p4);
  TopoDS_Edge edge6 = BRepBuilderAPI_MakeEdge(p4, p3);

  TopoDS_Wire wire1 = BRepBuilderAPI_MakeWire(edge1, edge2, edge3);
  TopoDS_Wire wire2 = BRepBuilderAPI_MakeWire(edge4, edge5, edge6);

  TopoDS_Face face1 = BRepBuilderAPI_MakeFace(wire1);
  TopoDS_Face face2 = BRepBuilderAPI_MakeFace(wire2);

  BRepBuilderAPI_Sewing sewing;
  sewing.Add(face1);
  sewing.Add(face2);
  sewing.Perform();

  TopoDS_Shape sewedFace = sewing.SewedShape();
  sewedFace.Reverse();
  // end face
  gp_Vec prismVec(gp_Dir(-movedir[0], -movedir[1], -movedir[2]));
  prismVec.Scale(height);
  // begin extrusion
  TopoDS_Shape prismFace = BRepPrimAPI_MakePrism(sewedFace, prismVec).Shape();
  // end extrusion
  return prismFace;
}

void SurfaceFormWidget::SetBeltConnet(QDoubleSpinBox *box) {
  connect(box, SIGNAL(valueChanged(double)), this, SLOT(OnBuildBeltRegion()));
}

void SurfaceFormWidget::SaveParameter(double parameter[]) {
  parameter[0] = ui->HookHightSpinBox->value();
  parameter[1] = ui->HookSupportHightSpinBox->value();
  parameter[2] = ui->HookRotateHightSpinBox->value();
  parameter[3] = ui->HookRadiusDoubleSpinBox->value();
  parameter[4] = ui->HookGuideRadiusDoubleSpinBox->value();
  parameter[5] = ui->HookLengthSlider->value();
  parameter[6] = ui->HookCubeXSpinBox->value();
  parameter[7] = ui->HookCubeYSpinBox->value();
  parameter[8] = ui->HookCubeZSpinBox->value();
  parameter[9] = ui->HookRotationSlider->value();
}

void SurfaceFormWidget::SetParameter(double parameter[]) {
  disconnect(ui->HookHightSpinBox, SIGNAL(valueChanged(int)), this,
             SLOT(BuildHook()));
  disconnect(ui->HookLengthSlider, SIGNAL(valueChanged(int)), this,
             SLOT(BuildHook()));
  disconnect(ui->HookRadiusDoubleSpinBox, SIGNAL(valueChanged(double)), this,
             SLOT(BuildHook()));
  disconnect(ui->HookRotateHightSpinBox, SIGNAL(valueChanged(int)), this,
             SLOT(BuildHook()));
  disconnect(ui->HookRotationSlider, SIGNAL(valueChanged(int)), this,
             SLOT(BuildHook()));
  disconnect(ui->HookGuideRadiusDoubleSpinBox, SIGNAL(valueChanged(double)),
             this, SLOT(BuildHook()));
  disconnect(ui->HookSupportHightSpinBox, SIGNAL(valueChanged(int)), this,
             SLOT(BuildHook()));

  ui->HookHightSpinBox->setValue(parameter[0]);
  ui->HookSupportHightSpinBox->setValue(parameter[1]);
  ui->HookRotateHightSpinBox->setValue(parameter[2]);
  ui->HookRadiusDoubleSpinBox->setValue(parameter[3]);
  ui->HookGuideRadiusDoubleSpinBox->setValue(parameter[4]);
  ui->HookLengthSlider->setValue(parameter[5]);
  ui->HookCubeXSpinBox->setValue(parameter[6]);
  ui->HookCubeYSpinBox->setValue(parameter[7]);
  ui->HookCubeZSpinBox->setValue(parameter[8]);
  ui->HookRotationSlider->setValue(parameter[9]);

  connect(ui->HookCubeXSpinBox, SIGNAL(valueChanged(int)), this,
          SLOT(BuildHook()));
  connect(ui->HookCubeYSpinBox, SIGNAL(valueChanged(int)), this,
          SLOT(BuildHook()));
  connect(ui->HookCubeZSpinBox, SIGNAL(valueChanged(int)), this,
          SLOT(BuildHook()));

  connect(ui->HookHightSpinBox, SIGNAL(valueChanged(int)), this,
          SLOT(BuildHook()));
  connect(ui->HookLengthSlider, SIGNAL(valueChanged(int)), this,
          SLOT(BuildHook()));
  connect(ui->HookRadiusDoubleSpinBox, SIGNAL(valueChanged(double)), this,
          SLOT(BuildHook()));
  connect(ui->HookRotateHightSpinBox, SIGNAL(valueChanged(int)), this,
          SLOT(BuildHook()));
  connect(ui->HookRotationSlider, SIGNAL(valueChanged(int)), this,
          SLOT(BuildHook()));
  connect(ui->HookGuideRadiusDoubleSpinBox, SIGNAL(valueChanged(double)), this,
          SLOT(BuildHook()));
  connect(ui->HookSupportHightSpinBox, SIGNAL(valueChanged(int)), this,
          SLOT(BuildHook()));

  connect(ui->HookCubeXSpinBox, SIGNAL(valueChanged(int)), this,
          SLOT(BuildHook()));
  connect(ui->HookCubeYSpinBox, SIGNAL(valueChanged(int)), this,
          SLOT(BuildHook()));
  connect(ui->HookCubeZSpinBox, SIGNAL(valueChanged(int)), this,
          SLOT(BuildHook()));
}

void SurfaceFormWidget::InitializeContourWidget() {
  m_VtkQtConnector = vtkSmartPointer<vtkEventQtSlotConnect>::New();
  m_ContourLabelFollower = vtkSmartPointer<vtkFollower>::New();
  auto label = vtkSmartPointer<vtkVectorText>::New();
  label->SetText("F");

  label->Update();
  auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper->SetInputData(label->GetOutput());
  m_ContourLabelFollower->SetMapper(mapper);
  m_ContourLabelFollower->GetProperty()->SetColor(1, 0, 0);
  m_ContourLabelFollower->SetScale(5, 5, 5);
  m_ContourLabelFollower->VisibilityOff();
  m_Render->AddActor(m_ContourLabelFollower);

  m_ContourRep = vtkSmartPointer<vtkOrientedGlyphContourRepresentation>::New();
  m_ContourRep->GetLinesProperty()->SetColor(0, 0, 1);
  m_ContourRep->GetProperty()->SetColor(0, 1, 0);

  m_ContourPointPlacer = vtkSmartPointer<vtkPolygonalSurfacePointPlacer>::New();

  m_ContourWidget = vtkSmartPointer<vtkContourWidget>::New();
  m_ContourWidget->SetRepresentation(m_ContourRep);
  m_ContourWidget->SetInteractor(m_Render->GetRenderWindow()->GetInteractor());

  m_VtkQtConnector->Connect(m_ContourWidget, vtkCommand::EndInteractionEvent,
                            this, SLOT(OnContourWidgetChanged()));
}

void SurfaceFormWidget::CutContours() {
  if (m_Direction[0] == 0 && m_Direction[1] == 0 && m_Direction[2] == 0)
    return;
  if (m_StartPos[0] == 0 && m_StartPos[1] == 0 && m_StartPos[2] == 0)
    return;
  if (m_EndPosp[0] == 0 && m_EndPosp[1] == 0 && m_EndPosp[2] == 0)
    return;

  int numOfContours = 10;
  int numOfSamples = ui->SampleNumSpinBox->value();
  double _startPos[3], _endPos[3];
  for (int i = 0; i < 3; i++) {
    _startPos[i] =
        m_StartPos[i] + ui->StartPositionSlider->value() * m_Direction[i];
    _endPos[i] = m_EndPosp[i] + ui->EndPositionSlider->value() * m_Direction[i];
  }

  m_ContourPointsList.clear();

  double totalDistance =
      sqrt(vtkMath::Distance2BetweenPoints(_startPos, _endPos));

  double singleDistance = totalDistance / (numOfContours - 1);
  auto cutplane = vtkSmartPointer<vtkPlane>::New();
  cutplane->SetOrigin(_startPos);
  cutplane->SetNormal(m_Direction);

  auto appd = vtkSmartPointer<vtkAppendPolyData>::New();

  for (int i = 0; i < numOfContours; i++) {
    if (i > 0)
      cutplane->Push(singleDistance);
    auto cutPoints = vtkSmartPointer<vtkPoints>::New();
    auto contourData = vtkSmartPointer<vtkPolyData>::New();
    auto out1 = vtkSmartPointer<vtkPolyData>::New();
    bool isdown = ui->DownRadioButton->isChecked() ? 1 : 0;
    GetContourPoints(m_ForCutData, numOfSamples, cutplane, cutPoints,
                     contourData, out1, isdown);

    int numOfNodes = 5;
    int singleLenth = cutPoints->GetNumberOfPoints() / numOfNodes;
    auto contourPoints = vtkSmartPointer<vtkPoints>::New();
    contourPoints->Initialize();

    for (int j = 0; j < cutPoints->GetNumberOfPoints(); j = j + singleLenth) {
      double pt[3];
      cutPoints->GetPoint(j, pt);
      contourPoints->InsertNextPoint(pt);
    }

    m_ContourPointsList.append(contourPoints);
    appd->AddInputData(contourData);
  }
  appd->Update();
  qDebug() << m_ContourPointsList.first()->GetNumberOfPoints();
  auto forContourPd = vtkSmartPointer<vtkPolyData>::New();
  forContourPd->SetPoints(m_ContourPointsList.first());
  auto cell = vtkSmartPointer<vtkCellArray>::New();
  cell->Initialize();
  auto idlist = vtkSmartPointer<vtkIdList>::New();
  idlist->Initialize();
  for (int i = 0; i < m_ContourPointsList.first()->GetNumberOfPoints(); i++) {
    idlist->InsertNextId(i);
  }
  cell->InsertNextCell(idlist);
  forContourPd->SetPoints(m_ContourPointsList.first());
  forContourPd->SetLines(cell);

  m_ContourWidget->SetWidgetState(vtkContourWidget::Manipulate);
  m_ContourWidget->On();
  m_ContourWidget->Initialize(forContourPd);

  auto mapperContour = vtkSmartPointer<vtkPolyDataMapper>::New();
  mapperContour->SetInputData(appd->GetOutput());
  m_ContourActor->SetMapper(mapperContour);
  m_Render->GetRenderWindow()->Render();
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
    m_DownWireRegion[0] = umin;
    m_DownWireRegion[1] = umax;
    m_DownWireRegion[2] = vmin;
    m_DownWireRegion[3] = vmax;
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
    m_UpWireRegion[0] = umin;
    m_UpWireRegion[1] = umax;
    m_UpWireRegion[2] = vmin;
    m_UpWireRegion[3] = vmax;
    gp1 = m_GeomSurfaceUp->Value(umin, vmin);
    gp2 = m_GeomSurfaceUp->Value(umin, vmax);
    gp3 = m_GeomSurfaceUp->Value(umax, vmax);
    gp4 = m_GeomSurfaceUp->Value(umax, vmin);
    GetGP(gp1, m_UpWirePoints[0]);
    GetGP(gp2, m_UpWirePoints[1]);
    GetGP(gp3, m_UpWirePoints[2]);
    GetGP(gp4, m_UpWirePoints[3]);

    m_UpCenter[0] = (gp1.X() + gp2.X() + gp3.X() + gp4.X()) / 4;
    m_UpCenter[1] = (gp1.Y() + gp2.Y() + gp3.Y() + gp4.Y()) / 4;
    m_UpCenter[2] = (gp1.Z() + gp2.Z() + gp3.Z() + gp4.Z()) / 4;

    double axisv[3], axisu[3];
    for (int i = 0; i < 3; i++) {
      axisv[i] = m_UpWirePoints[0][i] - m_UpWirePoints[1][i];
      axisu[i] = m_UpWirePoints[2][i] - m_UpWirePoints[1][i];
    }
    vtkMath::Normalize(axisv);
    vtkMath::Normalize(axisu);
    vtkMath::Cross(axisv, axisu, m_UpWireDir);
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

void SurfaceFormWidget::OnBuildTubeRegion() {
  double tubeUmin = ui->TubeMinUDoubleSpinBox->value();
  double tubeUmax = ui->TubeMaxUDoubleSpinBox->value();

  double tubeVmin = ui->TubeMinVDoubleSpinBox->value();
  double tubeVmax = ui->TubeMaxVDoubleSpinBox->value();

  double tubeRadius = ui->TubeRadisDoubleSpinBox->value();
  double tubeHeight = ui->TubeHeightSpinBox->value();

  double tubeSample = ui->TubeSampleSpinBox->value();

  m_TubeRegion[0] = tubeUmin;
  m_TubeRegion[1] = tubeUmax;
  m_TubeRegion[2] = tubeVmin;
  m_TubeRegion[3] = tubeVmax;

  m_TubeInformation[0] = tubeRadius;
  m_TubeInformation[1] = tubeHeight;
  m_TubeInformation[2] = tubeSample;

  TopoDS_Shape tubesShape;
  TopoDS_Compound tubesCompound;
  BRep_Builder compoundBuilder;
  compoundBuilder.MakeCompound(tubesCompound);

  for (double m = tubeUmin; m < tubeUmax; m = m + tubeSample) {
    for (double n = tubeVmin; n < tubeVmax; n = n + tubeSample) {
      double uNormal[3], vNormal[3], direction[3];

      gp_Pnt origion;
      gp_Vec diu;
      gp_Vec div;
      if (ui->DownRadioButton->isChecked())
        m_GeomSurfaceDown->D1(m, n, origion, diu, div);
      else
        m_GeomSurfaceUp->D1(m, n, origion, diu, div);
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
      gp_Ax2 axs(center, dir);
      TopoDS_Shape prismFace =
          BRepPrimAPI_MakeCylinder(axs, tubeRadius, tubeHeight);
      //      GC_MakeCircle circle(center, dir, tubeRadius);
      //      BRepBuilderAPI_MakeEdge circleEge(circle.Value());
      //      TopoDS_Edge circleShape = circleEge.Edge();
      //      BRepBuilderAPI_MakeWire circleWire(circleShape);
      //      BRepBuilderAPI_MakeFace circleFace(circleWire.Wire());
      //      gp_Vec v1 = dir;
      //      v1.Scale(tubeHeight);
      //      TopoDS_Shape prismFace = BRepPrimAPI_MakePrism(circleFace,
      //      v1).Shape();
      compoundBuilder.Add(tubesCompound, prismFace);
    }
  }
  auto tubepd = vtkSmartPointer<vtkPolyData>::New();
  ConvertTopoDS2PolyData(tubesCompound, tubepd);
  auto tubemapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  tubemapper->SetInputData(tubepd);

  m_TubesActor->SetMapper(tubemapper);
  m_Render->GetRenderWindow()->Render();
  qDebug() << "tubes";
}

void SurfaceFormWidget::OnBuildBeltRegion() {

  double belt1[4];
  belt1[0] = ui->Belt1MinUDoubleSpinBox->value();
  belt1[1] = ui->Belt1MaxUDoubleSpinBox->value();
  belt1[2] = ui->Belt1MinVDoubleSpinBox->value();
  belt1[3] = ui->Belt1MaxVDoubleSpinBox->value();
  if (belt1[0] >= belt1[1])
    return;
  if (belt1[2] >= belt1[3])
    return;
  double belt2[4];
  belt2[0] = ui->Belt2MinUDoubleSpinBox->value();
  belt2[1] = ui->Belt2MaxUDoubleSpinBox->value();
  belt2[2] = ui->Belt2MinVDoubleSpinBox->value();
  belt2[3] = ui->Belt2MaxVDoubleSpinBox->value();
  if (belt2[0] >= belt2[1])
    return;
  if (belt2[2] >= belt2[3])
    return;
  double belt3[4];
  belt3[0] = ui->Belt3MinUDoubleSpinBox->value();
  belt3[1] = ui->Belt3MaxUDoubleSpinBox->value();
  belt3[2] = ui->Belt3MinVDoubleSpinBox->value();
  belt3[3] = ui->Belt3MaxVDoubleSpinBox->value();
  if (belt3[0] >= belt3[1])
    return;
  if (belt3[2] >= belt3[3])
    return;
  double belt4[4];
  belt4[0] = ui->Belt4MinUDoubleSpinBox->value();
  belt4[1] = ui->Belt4MaxUDoubleSpinBox->value();
  belt4[2] = ui->Belt4MinVDoubleSpinBox->value();
  belt4[3] = ui->Belt4MaxVDoubleSpinBox->value();
  if (belt4[0] >= belt4[1])
    return;
  if (belt4[2] >= belt4[3])
    return;

  double height = 10;

  TopoDS_Shape beltShape1 = CalculateBeltShape(
      belt1, height, ui->Belt1UWidthSpinBox, ui->Belt1VWidthSpinBox);
  m_DownBelt1->Shape(beltShape1);

  auto beltpd1 = vtkSmartPointer<vtkPolyData>::New();
  ConvertTopoDS2PolyData(beltShape1, beltpd1);
  auto beltmapper1 = vtkSmartPointer<vtkPolyDataMapper>::New();
  beltmapper1->SetInputData(beltpd1);
  m_Belt1Actor->SetMapper(beltmapper1);

  TopoDS_Shape beltShape2 = CalculateBeltShape(
      belt2, height, ui->Belt2UWidthSpinBox, ui->Belt2VWidthSpinBox);
  m_DownBelt2->Shape(beltShape2);
  auto beltpd2 = vtkSmartPointer<vtkPolyData>::New();
  ConvertTopoDS2PolyData(beltShape2, beltpd2);
  auto beltmapper2 = vtkSmartPointer<vtkPolyDataMapper>::New();
  beltmapper2->SetInputData(beltpd2);
  m_Belt2Actor->SetMapper(beltmapper2);

  TopoDS_Shape beltShape3 = CalculateBeltShape(
      belt3, height, ui->Belt3UWidthSpinBox, ui->Belt3VWidthSpinBox);
  m_DownBelt3->Shape(beltShape3);
  auto beltpd3 = vtkSmartPointer<vtkPolyData>::New();
  ConvertTopoDS2PolyData(beltShape3, beltpd3);
  auto beltmapper3 = vtkSmartPointer<vtkPolyDataMapper>::New();
  beltmapper3->SetInputData(beltpd3);
  m_Belt3Actor->SetMapper(beltmapper3);

  TopoDS_Shape beltShape4 = CalculateBeltShape(
      belt4, height, ui->Belt4UWidthSpinBox, ui->Belt4VWidthSpinBox);
  m_DownBelt4->Shape(beltShape4);
  auto beltpd4 = vtkSmartPointer<vtkPolyData>::New();
  ConvertTopoDS2PolyData(beltShape4, beltpd4);
  auto beltmapper4 = vtkSmartPointer<vtkPolyDataMapper>::New();
  beltmapper4->SetInputData(beltpd4);
  m_Belt4Actor->SetMapper(beltmapper4);
  this->SetBeltVisibility(true);
  m_Render->GetRenderWindow()->Render();
}

void SurfaceFormWidget::OnPickHookPoint() {
  if (ui->WristRadioButton->isChecked() | ui->ElbowRadioButton->isChecked()) {
    ui->WristRadioButton->isChecked() ? m_WristHookActor->VisibilityOff() : 1;
    ui->ElbowRadioButton->isChecked() ? m_ElbowHookActor->VisibilityOff() : 1;
    m_Render->GetRenderWindow()->Render();
    emit pickHook();
  }
  return;
}

void SurfaceFormWidget::BuildHook() {

  //高度
  double tubehight1 = ui->HookHightSpinBox->value();
  double hight1 = ui->HookSupportHightSpinBox->value();

  double tubeRadius = ui->HookRadiusDoubleSpinBox->value();

  double guideRadius = ui->HookGuideRadiusDoubleSpinBox->value();
  double tubehight2 = ui->HookLengthSpinBox->value();

  double tubehight4 = ui->HookRotateHightSpinBox->value();
  //方向
  double direction1[3];
  for (int i = 0; i < 3; i++) {
    if (ui->ElbowRadioButton->isChecked())
      direction1[i] = m_ElbowHookDirection[i];
    if (ui->WristRadioButton->isChecked())
      direction1[i] = m_WristHookDirection[i];
  }
  //开始构建圆柱

  // clinder1
  double center1[3];
  for (int i = 0; i < 3; i++) {
    if (ui->ElbowRadioButton->isChecked())
      center1[i] = m_ElbowHookPoint[i] - hight1 * direction1[i];
    if (ui->WristRadioButton->isChecked())
      center1[i] = m_WristHookPoint[i] - hight1 * direction1[i];
  }

  gp_Pnt clinder1center(center1[0], center1[1], center1[2]);
  gp_Dir clinder1dir(direction1[0], direction1[1], direction1[2]);
  gp_Ax2 clinder1axs(clinder1center, clinder1dir);
  TopoDS_Shape clinder1Face =
      BRepPrimAPI_MakeCylinder(clinder1axs, tubeRadius, tubehight1 + hight1);

  auto clinder1pd = vtkSmartPointer<vtkPolyData>::New();
  ConvertTopoDS2PolyData(clinder1Face, clinder1pd);

  // clinder2
  double rotateDegree = ui->HookRotationSlider->value();
  double direction2[3];
  vtkMath::Perpendiculars(direction1, direction2, nullptr,
                          rotateDegree / 180 * vtkMath::Pi());

  double center2[3];
  for (int i = 0; i < 3; i++) {
    if (ui->ElbowRadioButton->isChecked())
      center2[i] =
          m_ElbowHookPoint[i] - hight1 * direction1[i] - 5 * direction2[i];
    if (ui->WristRadioButton->isChecked())
      center2[i] =
          m_WristHookPoint[i] - hight1 * direction1[i] - 5 * direction2[i];
  }

  gp_Pnt clinder2center(center2[0], center2[1], center2[2]);
  gp_Dir clinder2dir(direction2[0], direction2[1], direction2[2]);
  gp_Ax2 clinder2axs(clinder2center, clinder2dir);
  TopoDS_Shape clinder2Face =
      BRepPrimAPI_MakeCylinder(clinder2axs, tubeRadius, tubehight2 + 10 + 15);
  // clinder3
  double center3[3];
  double direction3[3];
  for (int i = 0; i < 3; i++) {
    center3[i] = center2[i] + direction2[i] * 6;
    direction3[i] = -direction2[i];
  }
  gp_Pnt clinder3center(center3[0], center3[1], center3[2]);
  gp_Dir clinder3dir(direction3[0], direction3[1], direction3[2]);
  gp_Ax2 clinder3axs(clinder3center, clinder3dir);
  TopoDS_Shape clinder3Face =
      BRepPrimAPI_MakeCylinder(clinder3axs, guideRadius, tubehight2 + 6);

  //卡位圆柱
  double clinderstopCenter[3], clinderstopDirection[3];
  for (int i = 0; i < 3; i++) {
    clinderstopCenter[i] = center2[i] + direction2[i] * 5.3;
    clinderstopDirection[i] = direction2[i];
  }
  gp_Pnt clinderstopPnt(clinderstopCenter[0], clinderstopCenter[1],
                        clinderstopCenter[2]);
  gp_Dir clinderstopDir(clinderstopDirection[0], clinderstopDirection[1],
                        clinderstopDirection[2]);
  gp_Ax2 clinderstopAx(clinderstopPnt, clinderstopDir);
  TopoDS_Shape clinderstopShape =
      BRepPrimAPI_MakeCylinder(clinderstopAx, guideRadius + 0.7, 1.7);

  TopoDS_Shape clinder3Fused;
  BRepAlgoAPI_Fuse clinder3Fuser(clinder3Face, clinderstopShape);
  clinder3Fuser.Build();
  if (!clinder3Fuser.IsDone())
    return;
  clinder3Fused = clinder3Fuser.Shape();

  // clinder4
  double center4[3];
  double direction4[3];
  for (int i = 0; i < 3; i++) {
    center4[i] = center2[i] + direction2[i] * (tubehight2 - tubeRadius);
    direction4[i] = direction1[i];
  }

  gp_Pnt clinder4center(center4[0], center4[1], center4[2]);
  gp_Dir clinder4dir(direction4[0], direction4[1], direction4[2]);
  gp_Ax2 clinder4axs(clinder4center, clinder4dir);
  TopoDS_Shape clinder4Face =
      BRepPrimAPI_MakeCylinder(clinder4axs, tubeRadius, tubehight4);
  auto clinder4pd = vtkSmartPointer<vtkPolyData>::New();
  ConvertTopoDS2PolyData(clinder4Face, clinder4pd);

  TopoDS_Shape ClinderFused;
  BRepAlgoAPI_Fuse clinderFuser1(clinder1Face, clinder2Face);
  clinderFuser1.Build();
  if (!clinderFuser1.IsDone())
    return;
  ClinderFused = clinderFuser1.Shape();

  BRepAlgoAPI_Fuse clinderFuser2(ClinderFused, clinder4Face);
  clinderFuser2.Build();
  if (!clinderFuser2.IsDone())
    return;
  ClinderFused = clinderFuser2.Shape();

  // for cut clinder
  double center4cutClinder[3];
  for (int i = 0; i < 3; i++)
    center4cutClinder[i] = center2[i] + 5 * direction2[i];

  gp_Pnt clinderCutcenter(center4cutClinder[0], center4cutClinder[1],
                          center4cutClinder[2]);
  gp_Dir clinderCutdir(direction2[0], direction2[1], direction2[2]);
  gp_Ax2 clinderCutaxs(clinderCutcenter, clinderCutdir);
  TopoDS_Shape clinder2cut = BRepPrimAPI_MakeCylinder(
      clinderCutaxs, guideRadius + 2, tubehight2 + 2 + 15);

  TopoDS_Shape ClinderCutted;
  BRepAlgoAPI_Cut cutter1(ClinderFused, clinder2cut);
  cutter1.Build();
  if (!cutter1.IsDone())
    return;
  ClinderCutted = cutter1.Shape();

  // cutter2
  double cutClinder2center[3], cutClinder2direction[3];
  for (int i = 0; i < 3; i++) {
    cutClinder2center[i] = center2[i] - direction2[i] * 2;
    cutClinder2direction[i] = direction2[i];
  }
  gp_Pnt cutClinder2Pnt(cutClinder2center[0], cutClinder2center[1],
                        cutClinder2center[2]);
  gp_Dir cutClinder2Dir(cutClinder2direction[0], cutClinder2direction[1],
                        cutClinder2direction[2]);
  gp_Ax2 cutClinder2Ax(cutClinder2Pnt, cutClinder2Dir);
  TopoDS_Shape cutClinder2Shape =
      BRepPrimAPI_MakeCylinder(cutClinder2Ax, guideRadius + 0.3, 10);

  TopoDS_Shape cuttedShape2;
  BRepAlgoAPI_Cut cutter2(ClinderCutted, cutClinder2Shape);
  cutter2.Build();
  if (!cutter2.IsDone())
    return;
  cuttedShape2 = cutter2.Shape();

  double openCubeDirection[3];
  for (int i = 0; i < 3; i++)
    openCubeDirection[i] = direction2[i];
  vtkMath::Normalize(openCubeDirection);

  double openCubexDir[3], openCubeyDir[3];
  vtkMath::Perpendiculars(openCubeDirection, openCubexDir, openCubeyDir, 0);

  double openCubeDx, openCubeDy, openCubeDz;
  openCubeDx = ui->HookCubeXSpinBox->value();
  openCubeDy = ui->HookCubeYSpinBox->value();
  openCubeDz = ui->HookCubeZSpinBox->value();

  double centerForOpenCube[3];
  for (int i = 0; i < 3; i++) {
    centerForOpenCube[i] = center2[i] - openCubexDir[i] * openCubeDx / 2.0 -
                           openCubeyDir[i] * openCubeDy / 2.0;
  }
  gp_Pnt openCubePnt(centerForOpenCube[0], centerForOpenCube[1],
                     centerForOpenCube[2]);

  gp_Dir openCubeNDir(openCubeDirection[0], openCubeDirection[1],
                      openCubeDirection[2]);
  gp_Dir openCubeVxDir(openCubexDir[0], openCubexDir[1], openCubexDir[2]);
  gp_Dir openCubeVyDir(openCubeyDir[0], openCubeyDir[1], openCubeyDir[2]);

  gp_Ax2 openCubeAxes(openCubePnt, openCubeNDir);
  openCubeAxes.SetXDirection(openCubeVxDir);
  openCubeAxes.SetYDirection(openCubeVyDir);

  TopoDS_Shape openCubeShape =
      BRepPrimAPI_MakeBox(openCubeAxes, openCubeDx, openCubeDy, openCubeDz);

  TopoDS_Shape afterCutCube;
  BRepAlgoAPI_Cut cubeCutter(cuttedShape2, openCubeShape);
  cubeCutter.Build();
  if (!cubeCutter.IsDone())
    return;
  afterCutCube = cubeCutter.Shape();

  // fix cut
  double cutClinderFixRadius =
      (openCubeDx > openCubeDy ? openCubeDx : openCubeDy) + 1;
  double cutClinderFixCenter[3], cutClinderFixDirecton[3];
  for (int i = 0; i < 3; i++) {
    cutClinderFixCenter[i] = center2[i] + direction2[i] * openCubeDz / 4;
    cutClinderFixDirecton[i] = direction2[i];
  }
  gp_Pnt cutClinderFixPnt(cutClinderFixCenter[0], cutClinderFixCenter[1],
                          cutClinderFixCenter[2]);
  gp_Dir cutClinderFixDir(cutClinderFixDirecton[0], cutClinderFixDirecton[1],
                          cutClinderFixDirecton[2]);
  gp_Ax2 cutClinderFixAx(cutClinderFixPnt, cutClinderFixDir);
  TopoDS_Shape cutClinderFixShape = BRepPrimAPI_MakeCylinder(
      cutClinderFixAx, cutClinderFixRadius / 2, 3 * openCubeDz / 4.0);

  TopoDS_Shape cutFixShape;
  BRepAlgoAPI_Cut cutFixCutter(afterCutCube, cutClinderFixShape);
  cutFixCutter.Build();
  if (!cutFixCutter.IsDone())
    return;
  cutFixShape = cutFixCutter.Shape();

  auto clinder2pd = vtkSmartPointer<vtkPolyData>::New();
  ConvertTopoDS2PolyData(cutFixShape, clinder2pd);

  // fix on clinder3
  double cubeOnClinder3Center[3];
  for (int i = 0; i < 3; i++)
    cubeOnClinder3Center[i] = centerForOpenCube[i] -
                              direction2[i] * (tubehight2 - 10) +
                              openCubexDir[i] * 0.25 + openCubeyDir[i] * 0.25;

  gp_Ax2 cubeOnClinder3Ax = openCubeAxes;
  cubeOnClinder3Ax.SetLocation(gp_Pnt(cubeOnClinder3Center[0],
                                      cubeOnClinder3Center[1],
                                      cubeOnClinder3Center[2]));

  TopoDS_Shape cubeOnClinder3 =
      BRepPrimAPI_MakeBox(cubeOnClinder3Ax, openCubeDx - 0.5, openCubeDy - 0.5,
                          openCubeDz / 2 - 0.5);

  // fuse fix on clinder3
  TopoDS_Shape fusedClinder3WithFix;
  BRepAlgoAPI_Fuse clinder3FuserWithFix(clinder3Fused, cubeOnClinder3);
  clinder3FuserWithFix.Build();
  if (!clinder3FuserWithFix.IsDone())
    return;
  fusedClinder3WithFix = clinder3FuserWithFix.Shape();
  auto clinder3pd = vtkSmartPointer<vtkPolyData>::New();
  ConvertTopoDS2PolyData(fusedClinder3WithFix, clinder3pd);
  auto Appender = vtkSmartPointer<vtkAppendPolyData>::New();
  // elbowAppender->AddInputData(clinder1pd);
  Appender->AddInputData(clinder2pd);
  Appender->AddInputData(clinder3pd);
  //  elbowAppender->AddInputData(clinder4pd);
  Appender->Update();
  BRepAlgoAPI_Fuse guideFuser(fusedClinder3WithFix, cutFixShape);
  guideFuser.Build();
  TopoDS_Shape guideShape;
  if (!guideFuser.IsDone())
    return;
  guideShape = guideFuser.Shape();

  if (ui->ElbowRadioButton->isChecked()) {
    m_ElbowGuideShape->Shape(guideShape);
    auto clinder1mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    clinder1mapper->SetInputData(Appender->GetOutput());

    m_ElbowHookActor->SetMapper(clinder1mapper);
    m_ElbowHookActor->VisibilityOn();
    m_Render->GetRenderWindow()->Render();

    SaveParameter(m_ElbowParameter);
  }
  if (ui->WristRadioButton->isChecked()) {
    m_WristGuideShape->Shape(guideShape);
    auto clinder1mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    clinder1mapper->SetInputData(Appender->GetOutput());

    m_WristHookActor->SetMapper(clinder1mapper);
    m_WristHookActor->VisibilityOn();
    m_Render->GetRenderWindow()->Render();
    SaveParameter(m_WristParameter);
  }
  //结束构建圆柱
}

void SurfaceFormWidget::OnHookRotateLeft() {
  double direction[3];
  m_Render->GetActiveCamera()->GetDirectionOfProjection(direction);
  vtkMath::MultiplyScalar(direction, -1);

  auto transform = vtkSmartPointer<vtkTransform>::New();
  transform->RotateWXYZ(1, direction);
  transform->Update();
  if (ui->ElbowRadioButton->isChecked()) {
    double changedDirection[3];
    for (int i = 0; i < 3; i++)
      changedDirection[i] =
          transform->TransformDoubleVector(m_ElbowHookDirection)[i];
    for (int i = 0; i < 3; i++)
      m_ElbowHookDirection[i] = changedDirection[i];
    BuildHook();
  }
  if (ui->WristRadioButton->isChecked()) {
    double changedDirection[3];
    for (int i = 0; i < 3; i++)
      changedDirection[i] =
          transform->TransformDoubleVector(m_WristHookDirection)[i];
    for (int i = 0; i < 3; i++)
      m_WristHookDirection[i] = changedDirection[i];
    BuildHook();
  }
}

void SurfaceFormWidget::OnHookRotateRight() {
  double direction[3];
  m_Render->GetActiveCamera()->GetDirectionOfProjection(direction);
  vtkMath::MultiplyScalar(direction, -1);

  auto transform = vtkSmartPointer<vtkTransform>::New();
  transform->RotateWXYZ(-1, direction);
  transform->Update();
  if (ui->ElbowRadioButton->isChecked()) {
    double changedDirection[3];
    for (int i = 0; i < 3; i++)
      changedDirection[i] =
          transform->TransformDoubleVector(m_ElbowHookDirection)[i];
    for (int i = 0; i < 3; i++)
      m_ElbowHookDirection[i] = changedDirection[i];
    BuildHook();
  }
  if (ui->WristRadioButton->isChecked()) {
    double changedDirection[3];
    for (int i = 0; i < 3; i++)
      changedDirection[i] =
          transform->TransformDoubleVector(m_WristHookDirection)[i];
    for (int i = 0; i < 3; i++)
      m_WristHookDirection[i] = changedDirection[i];
    BuildHook();
  }
}

void SurfaceFormWidget::OnHookMoveUp() {
  double viewUp[3];
  m_Render->GetActiveCamera()->GetViewUp(viewUp);
  if (ui->ElbowRadioButton->isChecked()) {
    for (int i = 0; i < 3; i++) {
      m_ElbowHookPoint[i] += viewUp[i];
    }
    BuildHook();
  }
  if (ui->WristRadioButton->isChecked()) {
    for (int i = 0; i < 3; i++) {
      m_WristHookPoint[i] += viewUp[i];
    }
    BuildHook();
  }
}

void SurfaceFormWidget::OnHookMoveDown() {
  double viewUp[3];
  m_Render->GetActiveCamera()->GetViewUp(viewUp);
  if (ui->ElbowRadioButton->isChecked()) {
    for (int i = 0; i < 3; i++) {
      m_ElbowHookPoint[i] -= viewUp[i];
    }
    BuildHook();
  }
  if (ui->WristRadioButton->isChecked()) {
    for (int i = 0; i < 3; i++) {
      m_WristHookPoint[i] -= viewUp[i];
    }
    BuildHook();
  }
}

void SurfaceFormWidget::OnHookMoveLeft() {
  double leftDirection[3];
  double viewUp[3], viewInside[3];
  m_Render->GetActiveCamera()->GetDirectionOfProjection(viewInside);
  m_Render->GetActiveCamera()->GetViewUp(viewUp);

  vtkMath::Cross(viewUp, viewInside, leftDirection);
  if (ui->ElbowRadioButton->isChecked()) {
    for (int i = 0; i < 3; i++)
      m_ElbowHookPoint[i] += leftDirection[i];
    BuildHook();
  }
  if (ui->WristRadioButton->isChecked()) {
    for (int i = 0; i < 3; i++)
      m_WristHookPoint[i] += leftDirection[i];
    BuildHook();
  }
}

void SurfaceFormWidget::OnHookMoveRight() {
  double leftDirection[3];
  double viewUp[3], viewInside[3];
  m_Render->GetActiveCamera()->GetDirectionOfProjection(viewInside);
  m_Render->GetActiveCamera()->GetViewUp(viewUp);

  vtkMath::Cross(viewUp, viewInside, leftDirection);
  if (ui->ElbowRadioButton->isChecked()) {
    for (int i = 0; i < 3; i++)
      m_ElbowHookPoint[i] -= leftDirection[i];
    BuildHook();
  }
  if (ui->WristRadioButton->isChecked()) {
    for (int i = 0; i < 3; i++)
      m_WristHookPoint[i] -= leftDirection[i];
    BuildHook();
  }
}

void SurfaceFormWidget::OnElbowRadioButton() {
  if (ui->ElbowRadioButton->isChecked())
    if (!m_ElbowGuideShape->Shape().IsNull())
      SetParameter(m_ElbowParameter);
}

void SurfaceFormWidget::OnWristRadioButton() {
  if (ui->WristRadioButton->isChecked()) {
    if (!m_WristGuideShape->Shape().IsNull())
      SetParameter(m_WristParameter);
  }
}

void SurfaceFormWidget::OnContourSpinboxChanged() {
  int indexOfCoutour = ui->ContourSpinBox->value();
  auto forContourPd = vtkSmartPointer<vtkPolyData>::New();
  auto cell = vtkSmartPointer<vtkCellArray>::New();
  cell->Initialize();
  auto idlist = vtkSmartPointer<vtkIdList>::New();
  idlist->Initialize();
  for (int i = 0;
       i < m_ContourPointsList.at(indexOfCoutour)->GetNumberOfPoints(); i++) {
    idlist->InsertNextId(i);
  }
  cell->InsertNextCell(idlist);
  forContourPd->SetPoints(m_ContourPointsList.at(indexOfCoutour));
  forContourPd->SetLines(cell);
  double firstPt[3];
  m_ContourPointsList.at(indexOfCoutour)->GetPoint(0, firstPt);
  m_ContourLabelFollower->VisibilityOn();
  m_ContourLabelFollower->SetPosition(firstPt);

  m_ContourWidget->SetWidgetState(vtkContourWidget::Manipulate);
  m_ContourWidget->On();
  m_ContourWidget->Initialize(forContourPd);
  m_Render->GetRenderWindow()->Render();
}

void SurfaceFormWidget::OnContourWidgetChanged() {
  if (m_ContourPointsList.size() < 10)
    return;
  double firstPt[3];
  auto nodeData = vtkSmartPointer<vtkPolyData>::New();
  m_ContourRep->GetNodePolyData(nodeData);
  auto points = vtkSmartPointer<vtkPoints>::New();
  points = nodeData->GetPoints();
  points->GetPoint(0, firstPt);
  m_ContourLabelFollower->VisibilityOn();
  m_ContourLabelFollower->SetPosition(firstPt);
  m_Render->GetRenderWindow()->Render();
  int changedContour = ui->ContourSpinBox->value();
  m_ContourPointsList.at(changedContour)->DeepCopy(points);
}

void SurfaceFormWidget::SetCutData(vtkPolyData *data, vtkActor *actor) {
  m_ForCutActor = actor;

  m_ContourPointPlacer->AddProp(actor);
  m_ContourRep->SetPointPlacer(m_ContourPointPlacer);

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
  normalFilter->AutoOrientNormalsOn();
  normalFilter->Update();
  //  auto decim = vtkSmartPointer<vtkDecimatePro>::New();
  //  decim->SetInputData(normalFilter->GetOutput());
  //  decim->SetTargetReduction(0.7);
  //  decim->Update();
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
  CutContours();
}

void SurfaceFormWidget::SetStartPos(double *pos) {
  for (int i = 0; i < 3; i++)
    m_StartPos[i] = pos[i];
}

void SurfaceFormWidget::SetHookPoint(double point[], double direction[]) {
  if (ui->ElbowRadioButton->isChecked()) {
    for (int i = 0; i < 3; i++) {
      m_ElbowHookPoint[i] = point[i];
      m_ElbowHookDirection[i] = direction[i];
    }
    this->BuildHook();
  }
  if (ui->WristRadioButton->isChecked()) {
    for (int i = 0; i < 3; i++) {
      m_WristHookPoint[i] = point[i];
      m_WristHookDirection[i] = direction[i];
    }
    this->BuildHook();
  }
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

void SurfaceFormWidget::SetTubesVisibility(bool vi) {
  m_TubesActor->SetVisibility(vi);
  m_Render->GetRenderWindow()->Render();
}

void SurfaceFormWidget::SetBeltVisibility(bool vi) {
  m_Belt1Actor->SetVisibility(vi);
  m_Belt2Actor->SetVisibility(vi);
  m_Belt3Actor->SetVisibility(vi);
  m_Belt4Actor->SetVisibility(vi);
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
