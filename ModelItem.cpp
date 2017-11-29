#include "ModelItem.h"

ModelItem::ModelItem(QObject *parent, vtkRenderer *ren, QString name,
                     vtkPolyData *data)
    : QObject(parent), m_ModelOpacity(100), m_ModelRep(0),
      m_ModelVisibility(1) {
  m_ModelName = name;
  m_ModelData = vtkSmartPointer<vtkPolyData>::New();
  m_ModelData->DeepCopy(data);
  m_ModelMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  m_ModelMapper->SetInputData(m_ModelData);
  m_ModelMapper->ScalarVisibilityOff();
  m_ModelActor = vtkSmartPointer<vtkActor>::New();
  m_ModelActor->SetMapper(m_ModelMapper);
  m_Renderer = vtkSmartPointer<vtkRenderer>::New();
  m_Renderer = ren;
  m_Renderer->AddActor(m_ModelActor);
  double _color[3];
  for (int i = 0; i < 3; i++) {
    _color[i] = 1;
  }
  this->SetColor(_color);
  this->RenderWin();

  m_DS = new TopoDS_HShape();
}
void ModelItem::SetModelName(QString name) { m_ModelName = name; }

void ModelItem::SetModelData(vtkPolyData *data) {
  if (m_ModelData == data)
    return;
  m_ModelData->DeepCopy(data);
}

QString ModelItem::GetModelName() { return m_ModelName; }

void ModelItem::GetModelName(QString &name) { name = m_ModelName; }

vtkPolyData *ModelItem::GetModelData() { return m_ModelData; }

void ModelItem::GetModelData(vtkPolyData *data) { data->DeepCopy(m_ModelData); }

void ModelItem::SetOpacity(int num) {
  m_ModelOpacity = num;
  if (num > 100)
    m_ModelOpacity = 100;
  if (num < 0)
    m_ModelOpacity = 0;
  m_ModelActor->GetProperty()->SetOpacity(m_ModelOpacity / 100.0);
  this->RenderWin();
}

void ModelItem::SetVisibility(bool vis) {
  m_ModelVisibility = vis;
  m_ModelActor->SetVisibility(m_ModelVisibility);
  this->RenderWin();
}

void ModelItem::SetColor(double color[]) {
  for (int i = 0; i < 3; i++) {
    m_ModelColor[i] = color[i];
  }
  m_ModelActor->GetProperty()->SetColor(m_ModelColor);
  this->RenderWin();
}

void ModelItem::SetRep(int num) {
  m_ModelRep = num;
  switch (m_ModelRep) {
  case 0: {
    m_ModelActor->GetProperty()->SetRepresentationToSurface();
    break;
  }
  case 1: {
    m_ModelActor->GetProperty()->SetRepresentationToPoints();
    break;
  }
  case 2: {
    m_ModelActor->GetProperty()->SetRepresentationToWireframe();
    break;
  }

  default:
    m_ModelActor->GetProperty()->SetRepresentationToSurface();
    break;
  }
  this->RenderWin();
}

void ModelItem::RemoveActor() {
  m_Renderer->RemoveActor(m_ModelActor);
  this->RenderWin();
}

void ModelItem::SetTopoDS_Shape(const TopoDS_Shape &ds) { m_DS->Shape(ds); }

void ModelItem::SetGeomSurface(Handle_Geom_Surface gs, double Umin, double Umax,
                               double Vmin, double Vmax) {
  m_GeomSurface = gs;
  m_Bounds[0] = Umin;
  m_Bounds[1] = Umax;
  m_Bounds[2] = Vmin;
  m_Bounds[3] = Vmax;
}

Handle_Geom_Surface ModelItem::GetGeomSurface(double bounds[]) {
  for (int i = 0; i < 4; i++) {
    bounds[i] = m_Bounds[i];
  }
  return m_GeomSurface;
}

Handle(TopoDS_HShape) ModelItem::GetTopoDS_Shape() { return m_DS; }

void ModelItem::RenderWin() { m_Renderer->GetRenderWindow()->Render(); }
