#ifndef MODELITEM_H
#define MODELITEM_H

#include <QObject>

class ModelItem : public QObject
{
    Q_OBJECT
public:
    explicit ModelItem(QObject *parent = nullptr);

signals:

public slots:
};

#endif // MODELITEM_H