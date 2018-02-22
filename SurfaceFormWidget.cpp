#include "SurfaceFormWidget.h"
#include "ui_SurfaceFormWidget.h"

SurfaceFormWidget::SurfaceFormWidget(QWidget *parent, vtkRenderer *ren)
    : QWidget(parent), ui(new Ui::SurfaceFormWidget) {
  ui->setupUi(this);
  this->setWindowFlag(Qt::WindowStaysOnTopHint);
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
        aFace, 10 * ui->OffSetDoubleSpinBox->value());
    myOffsetAlgo.SetBuildSolidFlag(Standard_True);
    myOffsetAlgo.Perform();
    TopoDS_Shape downFace = myOffsetAlgo.GetResultShape();
    downFace.Reverse();
    // offset UP
    BRepOffset_MakeSimpleOffset offsetdown(aFace,
                                           -ui->OffSetDoubleSpinBox->value());
    offsetdown.SetBuildSolidFlag(Standard_True);
    offsetdown.Perform();
    TopoDS_Shape offsetface = offsetdown.GetResultShape();
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
  auto decim = vtkSmartPointer<vtkDecimatePro>::New();
  decim->SetInputData(normalFilter->GetOutput());
  decim->SetTargetReduction(0.8);
  decim->Update();
  m_ForCutData->DeepCopy(decim->GetOutput());
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
