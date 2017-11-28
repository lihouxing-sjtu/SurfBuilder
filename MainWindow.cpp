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
  m_ModelRightMenu->addAction(deleteModelAction);
  m_ModelRightMenu->addAction(strechModelAction);
  connect(deleteModelAction, SIGNAL(triggered(bool)), this,
          SLOT(OnDeleteModel()));
  connect(strechModelAction, SIGNAL(triggered(bool)), this,
          SLOT(OnStrechTopo_Shape()));
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
  connect(m_StrechWidget, SIGNAL(strechDirection()), this,
          SLOT(OnStrechModel()));
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
  int nps = triangleFilter->GetOutput()->GetNumberOfPoints();
  GeomPlate_BuildPlateSurface bpsrf;
  for (int i = 0; i < nps; i = i + 10) {
    double pt[3];
    triangleFilter->GetOutput()->GetPoint(i, pt);
    gp_Pnt pnt(pt[0], pt[1], pt[2]);
    Handle(GeomPlate_PointConstraint) ptconstraint =
        new GeomPlate_PointConstraint(pnt, 0);
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

  Handle(Geom_Surface) Surf(plate.Surface());
  Standard_Real Umin, Umax, Vmin, Vmax;
  PSurf->Bounds(Umin, Umax, Vmin, Vmax);
  BRepBuilderAPI_MakeFace MF(Surf, Umin, Umax, Vmin, Vmax, 0.01);

  TopoDS_Face face;
  face = MF.Face();

  if (face.IsNull())
    qDebug() << "face";

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
  item->SetTopoDS_Shape(face);
  m_ModelList.append(item);

  this->AddModelItem(m_ModelList.last());
}

void MainWindow::OnStrechTopo_Shape() {
  if (m_ModelList.at(m_SelectItemIndex)->GetTopoDS_Shape()->Shape().IsNull())
    return;
  m_StrechWidget->show();
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
  if (m_ModelList.at(m_SelectItemIndex)->GetTopoDS_Shape()->Shape().IsNull())
    return;
  qDebug() << "strech 0";
  double direction[3], distance;
  distance = m_StrechWidget->GetDirectionAndDistance(direction);
  vtkMath::Normalize(direction);
  gp_Dir dir(direction[0], direction[1], direction[2]);
  qDebug() << direction[0] << "  " << direction[1] << "  " << direction[2];
  gp_Vec v = dir;
  v.Scale(distance);
  qDebug() << v.X() << "  " << v.Y() << "  " << v.Z();
  qDebug() << "strech 1";
  Handle(TopoDS_HShape) hshape =
      m_ModelList.at(m_SelectItemIndex)->GetTopoDS_Shape();
  const TopoDS_Shape face = hshape->Shape();
  //  BRepTools_ReShape reshape;
  //  TopExp_Explorer ex(face, TopAbs_WIRE);
  //  int ie = 5;
  //  while (ex.More()) {
  //    ie--;
  //    if (ie < 3) {
  //      reshape.Remove(ex.Current());
  //    }
  //    ex.Next();
  //  }
  //  qDebug() << ie;
  qDebug() << "strech 2";
  const TopoDS_Shape solid = BRepPrimAPI_MakePrism(face, v).Shape();
  qDebug() << "strech 3";

  BRepFilletAPI_MakeFillet filet(solid);
  TopExp_Explorer ex(solid, TopAbs_EDGE);
  while (ex.More()) {
    filet.Add(2, TopoDS::Edge(ex.Current()));
    ex.Next();
  }
  filet.Build();
  TopoDS_Shape ds;
  ds = filet.Shape();
  auto pd = vtkSmartPointer<vtkPolyData>::New();
  ConvertTopoDS2PolyData(ds, pd);
  qDebug() << "strech 4";
  QTime time = QTime::currentTime();
  QString modelName = "Strech-" + QString::number(time.hour()) + "-" +
                      QString::number(time.minute()) + "-" +
                      QString::number(time.second());
  pd->BuildCells();
  pd->GetLines()->Reset();

  ModelItem *item = new ModelItem(this, m_Render, modelName, pd);
  m_ModelList.append(item);
  item->SetTopoDS_Shape(solid);
  this->AddModelItem(m_ModelList.last());
}
