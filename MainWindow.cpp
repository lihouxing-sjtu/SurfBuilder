#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), m_SelectItemIndex(-1) {
  ui->setupUi(this);

  int widthOfTree = ui->ModelTreeWidget->width();
  ui->ModelTreeWidget->setColumnCount(2);
  ui->ModelTreeWidget->setColumnWidth(0, widthOfTree - 200);
  ui->ModelTreeWidget->setColumnWidth(1, widthOfTree - widthOfTree / 2);
  ui->ModelTreeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
  m_Render = vtkSmartPointer<vtkRenderer>::New();
  m_Render = ui->ViewWidget->GetViewRenderer();
  m_PreviewActor = vtkSmartPointer<vtkActor>::New();
  m_Render->AddActor(m_PreviewActor);

  m_HingeActor = vtkSmartPointer<vtkActor>::New();
  m_Render->AddActor(m_HingeActor);

  m_StrechWidget = new NormalWidget(0, m_Render);
  m_StrechWidget->hide();
  this->CollectionOfConnect();
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::CollectionOfConnect() {
  connect(ui->ImportSTLButton, SIGNAL(clicked(bool)), this,
          SLOT(OnImportSTL()));
  connect(ui->ModelTreeWidget,
          SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)),
          SLOT(OnChangeModelColor(QTreeWidgetItem *)));

  connect(ui->ModelTreeWidget, SIGNAL(customContextMenuRequested(QPoint)), this,
          SLOT(OnModelItemRightClicked(QPoint)));

  m_ModelRightMenu = new QMenu(this);
  QAction *deleteModelAction = new QAction(tr("Delete"), m_ModelRightMenu);
  QAction *strechModelAction = new QAction(tr("Strech"), m_ModelRightMenu);
  QAction *saveModelAction = new QAction(tr("Save"), m_ModelRightMenu);
  m_ModelRightMenu->addAction(deleteModelAction);
  m_ModelRightMenu->addAction(strechModelAction);
  m_ModelRightMenu->addAction(saveModelAction);
  connect(deleteModelAction, SIGNAL(triggered(bool)), this,
          SLOT(OnDeleteModel()));
  connect(strechModelAction, SIGNAL(triggered(bool)), this,
          SLOT(OnStrechTopo_Shape()));
  connect(saveModelAction, SIGNAL(triggered(bool)), this, SLOT(OnSaveModel()));
  connect(ui->ViewWidget, SIGNAL(endBSplineCurve()), this,
          SLOT(OnDrawBSplineCurve()));
  connect(ui->ViewWidget, SIGNAL(endSelectLoop()), this,
          SLOT(OnSelectPolyData()));
  connect(ui->ViewWidget, SIGNAL(endPickPoint()), this,
          SLOT(OnEndPickBasePoint()));
  //  double pt1[3] = {-154.15, -117.03, 1122.21};
  //  double pt2[3] = {-171.63, -109.71, 932.34};
  //  double dis;
  //  dis = sqrt(vtkMath::Distance2BetweenPoints(pt1, pt2));
  //  qDebug() << dis;
  connect(m_StrechWidget, SIGNAL(pickBasePoint()), this,
          SLOT(OnStartPickBasePoint()));
  connect(m_StrechWidget, SIGNAL(strechDone()), this, SLOT(OnStrechModel()));

  connect(m_StrechWidget, SIGNAL(startArcCut()), this, SLOT(OnStartArcCut()));
  connect(m_StrechWidget, SIGNAL(cancleArcCut()), this, SLOT(OnCancleArcCut()));
  connect(ui->ViewWidget, SIGNAL(endArcCut()), this, SLOT(OnEndArcCut()));

  connect(ui->UpDateHingeButton, SIGNAL(clicked(bool)), this,
          SLOT(OnUpDateHingeButton()));
  connect(ui->PositionHingeSlider, SIGNAL(valueChanged(int)), this,
          SLOT(OnUpDateHingeButton()));
}

void MainWindow::AddModelItem(ModelItem *item) {
  QTreeWidgetItem *topItem = new QTreeWidgetItem(ui->ModelTreeWidget);
  topItem->setText(0, item->GetModelName());
  topItem->setFlags(Qt::ItemIsEditable | Qt::ItemIsSelectable |
                    Qt::ItemIsEnabled);
  topItem->setFirstColumnSpanned(1);
  QFont topFont;
  topFont.setBold(1);
  topItem->setFont(0, topFont);
  QTreeWidgetItem *colorItem = new QTreeWidgetItem(topItem);
  colorItem->setText(0, "Color");
  colorItem->setFlags(Qt::ItemIsEnabled);
  QPixmap pixmap(40, 40);
  pixmap.fill(Qt::white);
  QIcon icon;
  icon.addPixmap(pixmap);
  colorItem->setIcon(1, icon);

  QTreeWidgetItem *opacityItem = new QTreeWidgetItem(topItem);
  opacityItem->setText(0, "Opacity");
  opacityItem->setFlags(Qt::ItemIsEnabled);
  QSlider *opacitySlider = new QSlider(Qt::Horizontal, ui->ModelTreeWidget);
  opacitySlider->setRange(0, 100);
  opacitySlider->setValue(100);
  ui->ModelTreeWidget->setItemWidget(opacityItem, 1, opacitySlider);
  m_SliderList.append(opacitySlider);
  // mapping opacity signal
  QSignalMapper *opacityMapper = new QSignalMapper(ui->ModelTreeWidget);
  opacityMapper->setMapping(opacitySlider, opacitySlider);
  connect(opacitySlider, SIGNAL(valueChanged(int)), opacityMapper, SLOT(map()));
  connect(opacityMapper, SIGNAL(mapped(QWidget *)), this,
          SLOT(OnChangeModelOpacity(QWidget *)));

  QTreeWidgetItem *visibilityItem = new QTreeWidgetItem(topItem);
  visibilityItem->setText(0, "Vis");
  visibilityItem->setFlags(Qt::ItemIsEnabled);
  QRadioButton *visRadio = new QRadioButton(ui->ModelTreeWidget);
  visRadio->setCheckable(1);
  visRadio->setChecked(1);
  visRadio->setAutoExclusive(0);
  ui->ModelTreeWidget->setItemWidget(visibilityItem, 1, visRadio);
  m_RadioList.append(visRadio);
  // mapping vis signal
  QSignalMapper *visMapper = new QSignalMapper(ui->ModelTreeWidget);
  visMapper->setMapping(visRadio, visRadio);
  connect(visRadio, SIGNAL(toggled(bool)), visMapper, SLOT(map()));
  connect(visMapper, SIGNAL(mapped(QWidget *)), this,
          SLOT(OnChangeModelVis(QWidget *)));

  QTreeWidgetItem *repItem = new QTreeWidgetItem(topItem);
  repItem->setText(0, "Rep");
  repItem->setFlags(Qt::ItemIsEnabled);
  QComboBox *repBox = new QComboBox(this);
  repBox->addItem("surface");
  repBox->addItem("points");
  repBox->addItem("frame");
  m_ComboxList.append(repBox);
  ui->ModelTreeWidget->setItemWidget(repItem, 1, repBox);

  // boolean item
  QTreeWidgetItem *boolItem = new QTreeWidgetItem(topItem);
  boolItem->setFlags(Qt::ItemIsEnabled);
  QComboBox *typeCombox = new QComboBox(this);
  typeCombox->addItem(tr("No Opotion"));
  typeCombox->addItem(tr("Common"));
  typeCombox->addItem(tr("Cut"));
  typeCombox->addItem(tr("Fuse"));
  typeCombox->addItem(tr("Section"));
  QSpinBox *serialNum = new QSpinBox(this);
  serialNum->setRange(-1, 99);
  serialNum->setValue(-1);
  ui->ModelTreeWidget->setItemWidget(boolItem, 1, typeCombox);
  ui->ModelTreeWidget->setItemWidget(boolItem, 0, serialNum);
  // mapping rep signal
  QSignalMapper *repMapper = new QSignalMapper(ui->ModelTreeWidget);
  repMapper->setMapping(repBox, repBox);
  connect(repBox, SIGNAL(currentIndexChanged(int)), repMapper, SLOT(map()));
  connect(repMapper, SIGNAL(mapped(QWidget *)), this,
          SLOT(OnChangeModelRep(QWidget *)));

  QList<QTreeWidgetItem *> addList;
  addList << topItem;
  ui->ModelTreeWidget->addTopLevelItems(addList);
  m_Render->ResetCamera();
  m_Render->GetRenderWindow()->Render();
}

void MainWindow::ConvertTopoDS2PolyData(TopoDS_Shape input,
                                        vtkPolyData *output) {
  IVtkOCC_Shape::Handle aShapeImpl = new IVtkOCC_Shape(input);

  auto DS = vtkSmartPointer<IVtkTools_ShapeDataSource>::New();
  DS->SetShape(aShapeImpl);
  DS->Update();

  output->DeepCopy(DS->GetOutput());
}

void MainWindow::OnImportSTL() {
  QString fileName = QFileDialog::getOpenFileName(this, tr("Open STL"), "/home",
                                                  tr("STL (*.stl)"));
  if (fileName.isEmpty())
    return;
  auto stlReader = vtkSmartPointer<vtkSTLReader>::New();
  stlReader->SetFileName(qPrintable(fileName));
  stlReader->Update();
  int index = fileName.lastIndexOf("/");
  QString modelName = fileName.right(fileName.size() - index - 1);
  ModelItem *item =
      new ModelItem(this, m_Render, modelName, stlReader->GetOutput());
  m_ModelList.append(item);

  this->AddModelItem(m_ModelList.last());
}

void MainWindow::OnChangeModelColor(QTreeWidgetItem *doubleClickedItem) {
  int indexOfTopItem =
      ui->ModelTreeWidget->indexOfTopLevelItem(doubleClickedItem->parent());
  if (indexOfTopItem == -1)
    return;
  if (doubleClickedItem->parent()->indexOfChild(doubleClickedItem) != 0)
    return;
  QColor selectedColor =
      QColorDialog::getColor(nullptr, this, tr("Change Model Color"));
  if (!selectedColor.isValid())
    return;
  double color[3];
  color[0] = selectedColor.red() / 255.0;
  color[1] = selectedColor.green() / 255.0;
  color[2] = selectedColor.blue() / 255.0;
  m_ModelList.at(indexOfTopItem)->SetColor(color);
  QPixmap pixmap(20, 20);
  pixmap.fill(selectedColor);
  QIcon icon;
  icon.addPixmap(pixmap);
  doubleClickedItem->setIcon(1, icon);
}
void MainWindow::OnChangeModelOpacity(QWidget *item) {
  QSlider *slider = (QSlider *)(item);
  int index = m_SliderList.indexOf(slider);
  m_ModelList.at(index)->SetOpacity(slider->value());
}

void MainWindow::OnChangeModelRep(QWidget *item) {
  QComboBox *repBox = (QComboBox *)(item);
  int index = m_ComboxList.indexOf(repBox);
  int repIndex = repBox->currentIndex();
  m_ModelList.at(index)->SetRep(repIndex);
}

void MainWindow::OnChangeModelVis(QWidget *item) {
  QRadioButton *visRadio = (QRadioButton *)(item);
  int index = m_RadioList.indexOf(visRadio);
  m_ModelList.at(index)->SetVisibility(visRadio->isChecked());
}

void MainWindow::OnModelItemRightClicked(const QPoint &pos) {
  int index = ui->ModelTreeWidget->indexOfTopLevelItem(
      ui->ModelTreeWidget->itemAt(pos));
  if (index == -1)
    return;
  m_SelectItemIndex = index;
  m_ModelRightMenu->exec(QCursor::pos());
}

void MainWindow::OnDeleteModel() {
  ui->ModelTreeWidget->takeTopLevelItem(m_SelectItemIndex);

  m_SliderList.removeAt(m_SelectItemIndex);
  m_ComboxList.removeAt(m_SelectItemIndex);
  m_RadioList.removeAt(m_SelectItemIndex);
  m_ModelList.at(m_SelectItemIndex)->RemoveActor();
  m_ModelList.removeAt(m_SelectItemIndex);
  m_SelectItemIndex = -1;
}

void MainWindow::OnDrawBSplineCurve() {
  auto points = vtkSmartPointer<vtkPoints>::New();
  ui->ViewWidget->GetPickPoints(points);
  int numOfPoints = points->GetNumberOfPoints();

  Handle_TColgp_HArray1OfPnt bsplinePoints =
      new TColgp_HArray1OfPnt(1, numOfPoints);
  for (int i = 0; i < numOfPoints; i++) {
    double pt[3];
    points->GetPoint(i, pt);
    gp_Pnt p(pt[0], pt[1], pt[2]);
    bsplinePoints->SetValue(i + 1, p);
  }
  GeomAPI_Interpolate interp(bsplinePoints, 0, Precision::Approximation());
  interp.Perform();
  Handle(Geom_BSplineCurve) bsplineCurve = interp.Curve();
  // Handle(Geom_BezierCurve) bezierCurve = new Geom_BezierCurve(bezierPoints);

  TopoDS_Edge aEdge = BRepBuilderAPI_MakeEdge(bsplineCurve);

  auto pd = vtkSmartPointer<vtkPolyData>::New();
  ConvertTopoDS2PolyData(aEdge, pd);
  QTime time = QTime::currentTime();
  QString modelName = "BSpline-" + QString::number(time.hour()) + "-" +
                      QString::number(time.minute()) + "-" +
                      QString::number(time.second());
  ModelItem *item = new ModelItem(this, m_Render, modelName, pd);
  m_ModelList.append(item);
  item->SetTopoDS_Shape(aEdge);
  this->AddModelItem(m_ModelList.last());
}

void MainWindow::OnSelectPolyData() {
  auto points = vtkSmartPointer<vtkPoints>::New();
  ui->ViewWidget->GetPickPoints(points);
  int numOfPoints = points->GetNumberOfPoints();

  Handle_TColgp_HArray1OfPnt bsplinePoints =
      new TColgp_HArray1OfPnt(1, numOfPoints);
  for (int i = 0; i < numOfPoints - 1; i++) {
    double pt[3];
    points->GetPoint(i, pt);
    gp_Pnt p(pt[0], pt[1], pt[2]);
    bsplinePoints->SetValue(i + 1, p);
  }
  double pt[3];
  points->GetPoint(0, pt);
  gp_Pnt p(pt[0], pt[1], pt[2]);
  bsplinePoints->SetValue(numOfPoints, p);
  GeomAPI_Interpolate interp(bsplinePoints, Standard_False,
                             Precision::Approximation());
  interp.Perform();
  Handle(Geom_BSplineCurve) bsplineCurve = interp.Curve();

  TopoDS_Edge aEdge = BRepBuilderAPI_MakeEdge(bsplineCurve);

  auto pd = vtkSmartPointer<vtkPolyData>::New();
  ConvertTopoDS2PolyData(aEdge, pd);

  auto selectPolydata = vtkSmartPointer<vtkSelectPolyData>::New();
  selectPolydata->SetInputData(m_ModelList.first()->GetModelData());
  selectPolydata->SetLoop(pd->GetPoints());
  selectPolydata->SetSelectionModeToSmallestRegion();
  selectPolydata->Update();

  auto connectFilter = vtkSmartPointer<vtkPolyDataConnectivityFilter>::New();
  connectFilter->SetInputData(selectPolydata->GetOutput());
  connectFilter->SetExtractionModeToClosestPointRegion();
  connectFilter->SetClosestPoint(points->GetPoint(numOfPoints - 1));
  connectFilter->Update();

  auto triangleFilter = vtkSmartPointer<vtkTriangleFilter>::New();
  triangleFilter->SetInputData(connectFilter->GetOutput());
  triangleFilter->Update();

  auto samplePoints = vtkSmartPointer<vtkPoints>::New();
  samplePoints = triangleFilter->GetOutput()->GetPoints();

  int nps = samplePoints->GetNumberOfPoints();
  int step = nps / 2000;
  qDebug() << nps;
  GeomPlate_BuildPlateSurface bpsrf;
  for (int i = 0; i < nps; i = i + 1 + step) {
    double pt[3];
    samplePoints->GetPoint(i, pt);
    gp_Pnt pnt(pt[0], pt[1], pt[2]);
    Handle(GeomPlate_PointConstraint) ptconstraint =
        new GeomPlate_PointConstraint(pnt, -1);
    bpsrf.Add(ptconstraint);
  }
  bpsrf.Perform();
  Standard_Integer MaxSeg = 5;
  Standard_Integer MaxDegree = 8;
  Standard_Integer CritOrder = 0;
  Standard_Real dmax, Tol;
  Handle(GeomPlate_Surface) PSurf = bpsrf.Surface();
  dmax = Max(0.001, 10 * bpsrf.G0Error());
  Tol = 0.001;
  GeomPlate_MakeApprox plate(PSurf, Tol, MaxSeg, MaxDegree, dmax, CritOrder);

  Handle_Geom_Surface Surf(plate.Surface());
  Standard_Real Umin, Umax, Vmin, Vmax;
  PSurf->Bounds(Umin, Umax, Vmin, Vmax);
  //  BRepBuilderAPI_MakeFace MF(Surf, Umin, Umax, Vmin, Vmax, 0.01);

  //  TopoDS_Face face;
  //  face = MF.Face();

  //  if (face.IsNull())
  //    qDebug() << "face";

  QTime time = QTime::currentTime();
  QString modelName = "SelectData-" + QString::number(time.hour()) + "-" +
                      QString::number(time.minute()) + "-" +
                      QString::number(time.second());
  modelName.append(".stl");
  /*  auto stlWriter = vtkSmartPointer<vtkSTLWriter>::New();
    stlWriter->SetInputData(triangleFilter->GetOutput());
    stlWriter->SetFileName(qPrintable(modelName.append(".stl")));
    stlWriter->Write();
    TopoDS_Shape ds;
    Standard_CString stlPath = qPrintable(modelName);
    StlAPI_Reader stlReader;
    if (stlReader.Read(ds, stlPath))
      qDebug() << "y";
    if (ds.IsNull())
      qDebug() << "s";
    qDebug() << "0";
    TopoDS_Face suf = TopoDS::Face(ds);
    qDebug() << "1";
    Handle(Geom_Surface) gsuf = BRep_Tool::Surface(suf);
    qDebug() << "2";
    GeomConvert::SurfaceToBSplineSurface(gsuf);
    qDebug() << "3";
    BRep_Builder builder;
    TopoDS_Face face;
    builder.MakeFace(face, gsuf, Precision::Approximation());
    qDebug() << "4";
*/
  ModelItem *item =
      new ModelItem(this, m_Render, modelName, triangleFilter->GetOutput());
  // item->SetTopoDS_Shape(face);
  item->SetGeomSurface(Surf, Umin, Umax, Vmin, Vmax);
  m_ModelList.append(item);

  this->AddModelItem(m_ModelList.last());
}

void MainWindow::OnStrechTopo_Shape() {
  double bounds[4];
  if (m_ModelList.at(m_SelectItemIndex)->GetTopoDS_Shape()->Shape().IsNull() &&
      m_ModelList.at(m_SelectItemIndex)->GetGeomSurface(bounds).IsNull())
    return;
  m_StrechWidget->show();
  m_StrechWidget->SetGeomSurface(
      m_ModelList.at(m_SelectItemIndex)->GetGeomSurface(bounds), bounds[0],
      bounds[1], bounds[2], bounds[3]);
}

void MainWindow::OnStartPickBasePoint() { ui->ViewWidget->SetPickPoint(); }

void MainWindow::OnEndPickBasePoint() {
  auto points = vtkSmartPointer<vtkPoints>::New();
  ui->ViewWidget->GetPickPoints(points);
  double location[3], direction[3];

  m_Render->GetActiveCamera()->GetDirectionOfProjection(direction);
  vtkMath::MultiplyScalar(direction, -1);
  points->GetPoint(0, location);
  m_StrechWidget->SetPoints(direction, location);
}

void MainWindow::OnStrechModel() {
  QTime time = QTime::currentTime();
  QString modelName = "StrechData-" + QString::number(time.hour()) + "-" +
                      QString::number(time.minute()) + "-" +
                      QString::number(time.second());
  auto pd = vtkSmartPointer<vtkPolyData>::New();
  Handle_TopoDS_HShape ds = m_StrechWidget->GetData(pd);
  ModelItem *item = new ModelItem(this, m_Render, modelName, pd);
  m_ModelList.append(item);
  item->SetTopoDS_Shape(ds->Shape());
  this->AddModelItem(m_ModelList.last());
}

void MainWindow::OnSaveModel() {
  QString IniPath = m_ModelList.at(m_SelectItemIndex)->GetModelName();
  QString savePath = QFileDialog::getSaveFileName(this, tr("Save"), IniPath);
  auto pd = vtkSmartPointer<vtkPolyData>::New();
  m_ModelList.at(m_SelectItemIndex)->GetModelData(pd);
  auto triangleFilter = vtkSmartPointer<vtkTriangleFilter>::New();
  triangleFilter->SetInputData(pd);
  triangleFilter->Update();
  ;
  auto stlWriter = vtkSmartPointer<vtkSTLWriter>::New();
  stlWriter->SetInputData(triangleFilter->GetOutput());
  stlWriter->SetFileName(qPrintable(savePath.append(".stl")));
  stlWriter->Write();
  stlWriter->Update();
}

void MainWindow::OnStartArcCut() { ui->ViewWidget->StartArcCut(); }

void MainWindow::OnCancleArcCut() { ui->ViewWidget->CancleArcCut(); }

void MainWindow::OnEndArcCut() {
  auto points = vtkSmartPointer<vtkPoints>::New();
  ui->ViewWidget->GetPickPoints(points);
  m_StrechWidget->SetArcCutPoints(points);
}

void MainWindow::OnUpDateHingeButton() {
  double diameterHinge = ui->HingeDiameterSpinBox->value();
  double thicknessHinge = ui->HingeThicknessSpinBox->value();
  double heightHinge = ui->HingeHeightSpinBox->value();

  double positionHinge[3], directionHinge[3];
  directionHinge[0] = ui->DirectionSpinBoxX->value();
  directionHinge[1] = ui->DirectionSpinBoxY->value();
  directionHinge[2] = ui->DirectionSpinBoxZ->value();

  positionHinge[0] = ui->BasePointSpinBoxX->value();
  positionHinge[1] = ui->BasePointSpinBoxY->value();
  positionHinge[2] = ui->BasePointSpinBoxZ->value();

  for (int i = 0; i < 3; i++) {
    positionHinge[i] =
        positionHinge[i] + ui->PositionHingeSlider->value() * directionHinge[i];
  }

  double crossDirection[3];
  vtkMath::Perpendiculars(directionHinge, crossDirection, NULL,
                          vtkMath::Pi() * ui->StartCutSpinBox->value() / 180.0);

  double radialPoint1[3], radiaPoint2[3];
  for (int i = 0; i < 3; i++) {
    radialPoint1[i] = positionHinge[i] + crossDirection[i] * diameterHinge / 2;
    radiaPoint2[i] = radialPoint1[i] + crossDirection[i] * thicknessHinge;
  }

  double sectionPoint[6][3];
  for (int i = 0; i < 3; i++) {
    sectionPoint[0][i] = radialPoint1[i] - directionHinge[i] * heightHinge / 2;
    sectionPoint[5][i] = sectionPoint[0][i] + directionHinge[i] * heightHinge;

    sectionPoint[2][i] = radiaPoint2[i] - directionHinge[i] * heightHinge / 2;
    sectionPoint[3][i] = sectionPoint[2][i] + directionHinge[i] * heightHinge;

    sectionPoint[1][i] =
        (sectionPoint[0][i] + sectionPoint[2][i]) / 2 - directionHinge[i] * 5;
    sectionPoint[4][i] =
        sectionPoint[1][i] + directionHinge[i] * (heightHinge + 10);
  }

  gp_Pnt p1(sectionPoint[0][0], sectionPoint[0][1], sectionPoint[0][2]);
  gp_Pnt p2(sectionPoint[1][0], sectionPoint[1][1], sectionPoint[1][2]);
  gp_Pnt p3(sectionPoint[2][0], sectionPoint[2][1], sectionPoint[2][2]);
  gp_Pnt p4(sectionPoint[3][0], sectionPoint[3][1], sectionPoint[3][2]);
  gp_Pnt p5(sectionPoint[4][0], sectionPoint[4][1], sectionPoint[4][2]);
  gp_Pnt p6(sectionPoint[5][0], sectionPoint[5][1], sectionPoint[5][2]);

  Handle(Geom_TrimmedCurve) ArcOfCircle1 = GC_MakeArcOfCircle(p1, p2, p3);
  Handle(Geom_TrimmedCurve) aSegment1 = GC_MakeSegment(p3, p4);
  Handle(Geom_TrimmedCurve) ArcOfCircle2 = GC_MakeArcOfCircle(p4, p5, p6);
  Handle(Geom_TrimmedCurve) aSegment2 = GC_MakeSegment(p6, p1);

  TopoDS_Edge aEdge1 = BRepBuilderAPI_MakeEdge(ArcOfCircle1);
  TopoDS_Edge aEdge2 = BRepBuilderAPI_MakeEdge(aSegment1);
  TopoDS_Edge aEdge3 = BRepBuilderAPI_MakeEdge(ArcOfCircle2);
  TopoDS_Edge aEdge4 = BRepBuilderAPI_MakeEdge(aSegment2);
  TopoDS_Wire aWire = BRepBuilderAPI_MakeWire(aEdge1, aEdge2, aEdge3, aEdge4);

  TopoDS_Face aFace = BRepBuilderAPI_MakeFace(aWire);

  gp_Ax1 axis(gp_Pnt(positionHinge[0], positionHinge[1], positionHinge[2]),
              gp_Dir(directionHinge[0], directionHinge[1], directionHinge[2]));
  BRepPrimAPI_MakeRevol revol(aFace, axis);
  revol.Build();
  TopoDS_Shape aSolid = revol.Shape();

  double cutSection[8][3];
  double cutThickness, cutHeight;
  cutThickness = ui->CutThickSpinBox->value();
  cutHeight = ui->CutHeightSpinBox->value();
  double cutR = 2;
  for (int i = 0; i < 3; i++) {
    cutSection[0][i] = radiaPoint2[i] - crossDirection[i] * cutThickness -
                       directionHinge[i] * cutHeight / 2;
    cutSection[1][i] =
        cutSection[0][i] - directionHinge[i] * cutR + crossDirection[i] * cutR;
    cutSection[2][i] = cutSection[0][i] + crossDirection[i] * cutR * 2;
    cutSection[3][i] =
        cutSection[0][i] + crossDirection[i] * (cutThickness + 5);

    cutSection[4][i] = cutSection[3][i] + directionHinge[i] * cutHeight;
    cutSection[5][i] = cutSection[2][i] + directionHinge[i] * cutHeight;
    cutSection[6][i] =
        cutSection[1][i] + directionHinge[i] * (cutHeight + cutR * 2);
    cutSection[7][i] = cutSection[0][i] + directionHinge[i] * cutHeight;
  }
  gp_Pnt cutP1(cutSection[0][0], cutSection[0][1], cutSection[0][2]);
  gp_Pnt cutP2(cutSection[1][0], cutSection[1][1], cutSection[1][2]);
  gp_Pnt cutP3(cutSection[2][0], cutSection[2][1], cutSection[2][2]);
  gp_Pnt cutP4(cutSection[3][0], cutSection[3][1], cutSection[3][2]);
  gp_Pnt cutP5(cutSection[4][0], cutSection[4][1], cutSection[4][2]);
  gp_Pnt cutP6(cutSection[5][0], cutSection[5][1], cutSection[5][2]);
  gp_Pnt cutP7(cutSection[6][0], cutSection[6][1], cutSection[6][2]);
  gp_Pnt cutP8(cutSection[7][0], cutSection[7][1], cutSection[7][2]);

  Handle(Geom_TrimmedCurve) cutArc1 = GC_MakeArcOfCircle(cutP1, cutP2, cutP3);
  Handle(Geom_TrimmedCurve) cutSeg1 = GC_MakeSegment(cutP3, cutP4);
  Handle(Geom_TrimmedCurve) cutSeg2 = GC_MakeSegment(cutP4, cutP5);
  Handle(Geom_TrimmedCurve) cutSeg3 = GC_MakeSegment(cutP5, cutP6);
  Handle(Geom_TrimmedCurve) cutArc2 = GC_MakeArcOfCircle(cutP6, cutP7, cutP8);
  Handle(Geom_TrimmedCurve) cutSeg4 = GC_MakeSegment(cutP8, cutP1);

  TopoDS_Edge cutEdge1 = BRepBuilderAPI_MakeEdge(cutArc1);
  TopoDS_Edge cutEdge2 = BRepBuilderAPI_MakeEdge(cutSeg1);
  TopoDS_Edge cutEdge3 = BRepBuilderAPI_MakeEdge(cutSeg2);
  TopoDS_Edge cutEdge4 = BRepBuilderAPI_MakeEdge(cutSeg3);
  TopoDS_Edge cutEdge5 = BRepBuilderAPI_MakeEdge(cutArc2);
  TopoDS_Edge cutEdge6 = BRepBuilderAPI_MakeEdge(cutSeg4);
  BRepBuilderAPI_MakeWire cutMakeWire;
  cutMakeWire.Add(cutEdge1);
  cutMakeWire.Add(cutEdge2);
  cutMakeWire.Add(cutEdge3);
  cutMakeWire.Add(cutEdge4);
  cutMakeWire.Add(cutEdge5);
  cutMakeWire.Add(cutEdge6);
  cutMakeWire.Build();
  TopoDS_Wire cutWire = cutMakeWire.Wire();

  TopoDS_Shape cutFace = BRepBuilderAPI_MakeFace(cutWire);
  double cutAngle = vtkMath::Pi() * ui->EndCutSpinBox->value() / 180.0;
  BRepPrimAPI_MakeRevol cutRevol(cutFace, axis, cutAngle);
  cutRevol.Build();
  TopoDS_Shape cutSolid = cutRevol.Shape();
  TopoDS_Shape withCut = BRepAlgoAPI_Cut(aSolid, cutSolid);

  double movingSection[8][3];
  double deltaMove = 0.5;
  double movingHeight = cutHeight - 2 * deltaMove;
  double movingThickness = ui->MoveThicknessSpinBox->value();
  double moveCrossDirection[3], moveRadialPoint[3];
  vtkMath::Perpendiculars(directionHinge, moveCrossDirection, NULL,
                          vtkMath::Pi() * ui->StartMoveSpinBox->value() /
                              180.0);

  for (int i = 0; i < 3; i++) {
    moveRadialPoint[i] =
        positionHinge[i] +
        moveCrossDirection[i] *
            (diameterHinge / 2 + deltaMove + thicknessHinge - cutThickness);
    movingSection[0][i] =
        moveRadialPoint[i] + directionHinge[i] * movingHeight / 2;
    movingSection[1][i] = movingSection[0][i] +
                          moveCrossDirection[i] * (cutR - deltaMove) +
                          directionHinge[i] * (cutR - deltaMove);
    movingSection[2][i] =
        movingSection[0][i] + moveCrossDirection[i] * (cutR - deltaMove) * 2;
    movingSection[3][i] =
        movingSection[0][i] + moveCrossDirection[i] * movingThickness;
    movingSection[4][i] =
        movingSection[3][i] - directionHinge[i] * movingHeight;
    movingSection[7][i] =
        movingSection[0][i] - directionHinge[i] * movingHeight;
    movingSection[6][i] = movingSection[7][i] +
                          moveCrossDirection[i] * (cutR - deltaMove) -
                          directionHinge[i] * (cutR - deltaMove);
    movingSection[5][i] =
        movingSection[7][i] + moveCrossDirection[i] * (cutR - deltaMove) * 2;
  }

  gp_Pnt moveP1(movingSection[0][0], movingSection[0][1], movingSection[0][2]);
  gp_Pnt moveP2(movingSection[1][0], movingSection[1][1], movingSection[1][2]);
  gp_Pnt moveP3(movingSection[2][0], movingSection[2][1], movingSection[2][2]);
  gp_Pnt moveP4(movingSection[3][0], movingSection[3][1], movingSection[3][2]);
  gp_Pnt moveP5(movingSection[4][0], movingSection[4][1], movingSection[4][2]);
  gp_Pnt moveP6(movingSection[5][0], movingSection[5][1], movingSection[5][2]);
  gp_Pnt moveP7(movingSection[6][0], movingSection[6][1], movingSection[6][2]);
  gp_Pnt moveP8(movingSection[7][0], movingSection[7][1], movingSection[7][2]);

  Handle(Geom_TrimmedCurve) moveArc1 =
      GC_MakeArcOfCircle(moveP8, moveP7, moveP6);
  Handle(Geom_TrimmedCurve) moveSeg1 = GC_MakeSegment(moveP6, moveP5);
  Handle(Geom_TrimmedCurve) moveSeg2 = GC_MakeSegment(moveP5, moveP4);
  Handle(Geom_TrimmedCurve) moveSeg3 = GC_MakeSegment(moveP4, moveP3);
  Handle(Geom_TrimmedCurve) moveArc2 =
      GC_MakeArcOfCircle(moveP3, moveP2, moveP1);
  Handle(Geom_TrimmedCurve) moveSeg4 = GC_MakeSegment(moveP1, moveP8);

  TopoDS_Edge moveEdge1 = BRepBuilderAPI_MakeEdge(moveArc1);
  TopoDS_Edge moveEdge2 = BRepBuilderAPI_MakeEdge(moveSeg1);
  TopoDS_Edge moveEdge3 = BRepBuilderAPI_MakeEdge(moveSeg2);
  TopoDS_Edge moveEdge4 = BRepBuilderAPI_MakeEdge(moveSeg3);
  TopoDS_Edge moveEdge5 = BRepBuilderAPI_MakeEdge(moveArc2);
  TopoDS_Edge moveEdge6 = BRepBuilderAPI_MakeEdge(moveSeg4);

  BRepBuilderAPI_MakeWire moveMakeWire;
  moveMakeWire.Add(moveEdge1);
  moveMakeWire.Add(moveEdge2);
  moveMakeWire.Add(moveEdge3);
  moveMakeWire.Add(moveEdge4);
  moveMakeWire.Add(moveEdge5);
  moveMakeWire.Add(moveEdge6);
  moveMakeWire.Build();
  TopoDS_Wire moveWire = moveMakeWire.Wire();
  TopoDS_Shape moveFace = BRepBuilderAPI_MakeFace(moveWire);
  double moveAngle = vtkMath::Pi() * ui->EndMoveSpinBox->value() / 180.0;
  BRepPrimAPI_MakeRevol moveRevol(moveFace, axis, moveAngle);
  moveRevol.Build();
  TopoDS_Shape moveSolid = moveRevol.Shape();
  TopoDS_Shape withMove = BRepAlgoAPI_Fuse(withCut, moveSolid);

  auto pd = vtkSmartPointer<vtkPolyData>::New();
  ConvertTopoDS2PolyData(withMove, pd);
  auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper->SetInputData(pd);
  m_HingeActor->SetMapper(mapper);
  m_Render->GetRenderWindow()->Render();
}
