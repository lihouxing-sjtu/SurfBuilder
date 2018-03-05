#ifndef MODELITEM_H
#define MODELITEM_H
#include "stdafx.h"
#include <QObject>

class ModelItem : public QObject {
  Q_OBJECT
public:
  explicit ModelItem(QObject *parent = nullptr, vtkRenderer *ren = nullptr,
                     QString name = 0, vtkPolyData *inputdata = nullptr);
  void SetModelName(QString name);
  void SetModelData(vtkPolyData *data);
  QString GetModelName();
  void GetModelName(QString &name);
  vtkPolyData *GetModelData();
  vtkActor *GetActor();
  void GetModelData(vtkPolyData *data);
  void SetOpacity(int num);
  void SetVisibility(bool vis);
  void SetColor(double color[]);
  void SetRep(int num);
  void RemoveActor();
  void SetTopoDS_Shape(const TopoDS_Shape &ds);
  Handle(TopoDS_HShape) GetTopoDS_Shape();
  void SetGeomSurface(Handle_Geom_Surface gs, double Umin, double Umax,
                      double Vmin, double Vmax);
  Handle_Geom_Surface GetGeomSurface(double bounds[]);

private:
  QString m_ModelName;
  int m_ModelOpacity;
  bool m_ModelVisibility;
  double m_ModelColor[3];
  int m_ModelRep; // 0--surface,1--points,2--frame
  Handle(TopoDS_HShape) m_DS;
  Handle_Geom_Surface m_GeomSurface;
  double m_Bounds[4];
  vtkSmartPointer<vtkPolyData> m_ModelData;
  vtkSmartPointer<vtkRenderer> m_Renderer;
  vtkSmartPointer<vtkPolyDataMapper> m_ModelMapper;
  vtkSmartPointer<vtkActor> m_ModelActor;

protected:
  void RenderWin();
signals:

protected slots:
};

#endif // MODELITEM_H
