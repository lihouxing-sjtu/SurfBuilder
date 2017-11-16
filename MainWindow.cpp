#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  int widthOfTree = ui->ModelTreeWidget->width();
  ui->ModelTreeWidget->setColumnCount(2);
  ui->ModelTreeWidget->setColumnWidth(0, widthOfTree - 200);
  ui->ModelTreeWidget->setColumnWidth(1, widthOfTree - widthOfTree / 2);
  m_Render = vtkSmartPointer<vtkRenderer>::New();
  m_Render = ui->ViewWidget->GetViewRenderer();

  this->CollectionOfConnect();
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::CollectionOfConnect() {
  connect(ui->ImportSTLButton, SIGNAL(clicked(bool)), this,
          SLOT(OnImportSTL()));
  connect(ui->ModelTreeWidget,
          SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)),
          SLOT(OnChangeModelColor(QTreeWidgetItem *)));
  connect(ui->ModelTreeWidget, SIGNAL(itemChanged(QTreeWidgetItem *, int)),
          SLOT(OnChangeModelOpacity(QTreeWidgetItem *, int)));
}

void MainWindow::AddModelItem(ModelItem *item) {
  QTreeWidgetItem *topItem = new QTreeWidgetItem(ui->ModelTreeWidget);
  topItem->setText(0, item->GetModelName());
  topItem->setFlags(Qt::ItemIsEditable | Qt::ItemIsSelectable |
                    Qt::ItemIsEnabled);

  QFont topFont;
  topFont.setBold(1);
  topItem->setFont(0, topFont);
  QTreeWidgetItem *colorItem = new QTreeWidgetItem(topItem);
  colorItem->setText(0, "Color");
  colorItem->setFlags(Qt::ItemIsEnabled);
  QPixmap pixmap(20, 20);
  pixmap.fill(Qt::white);
  QIcon icon;
  icon.addPixmap(pixmap);
  colorItem->setIcon(1, icon);

  QTreeWidgetItem *opacityItem = new QTreeWidgetItem(topItem);
  opacityItem->setText(0, "Opacity");
  opacityItem->setFlags(Qt::ItemIsEnabled);
  QSlider *opacitySlider = new QSlider(Qt::Horizontal, this);
  opacitySlider->setRange(0, 100);
  opacitySlider->setValue(100);

  ui->ModelTreeWidget->setItemWidget(opacityItem, 1, opacitySlider);

  QTreeWidgetItem *visibilityItem = new QTreeWidgetItem(topItem);
  visibilityItem->setText(0, "Vis");
  visibilityItem->setText(1, "1");
  visibilityItem->setFlags(Qt::ItemIsEnabled);
  QTreeWidgetItem *repItem = new QTreeWidgetItem(topItem);
  repItem->setText(0, "Rep");
  repItem->setFlags(Qt::ItemIsEnabled);
  QComboBox *repBox = new QComboBox(this);
  repBox->addItem("surface");
  repBox->addItem("points");
  repBox->addItem("frame");
  ui->ModelTreeWidget->setItemWidget(repItem, 1, repBox);

  QList<QTreeWidgetItem *> addList;
  addList << topItem;
  ui->ModelTreeWidget->addTopLevelItems(addList);
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

void MainWindow::OnChangeModelOpacity(QTreeWidgetItem *ChangedItem,
                                      int column) {

  int indexOfTopItem =
      ui->ModelTreeWidget->indexOfTopLevelItem(ChangedItem->parent());
  if (indexOfTopItem == -1)
    return;
  if (ChangedItem->parent()->indexOfChild(ChangedItem) != 1)
    return;
  if (column == 0) {
    ChangedItem->setText(0, "Opacity");
    return;
  }
  QMessageBox::information(this, "", "");
  bool ok = true;
  int op = ChangedItem->text(1).toInt(&ok, 10);
  if (op > 100 || op < 0 || !ok) {
    op = 100;
    ChangedItem->setText(1, QString::number(op));
  }
  m_ModelList.at(indexOfTopItem)->SetOpacity(op);
}
