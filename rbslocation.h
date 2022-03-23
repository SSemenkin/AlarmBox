#ifndef RBSLOCATION_H
#define RBSLOCATION_H

#include <QMap>

class RbsLocation
{
public:
    RbsLocation(const QString &filename);
    void updateLocations(const QString &filename);


    QString getLocation(const QString &object);

private:
    QMap<QString, QString> m_locations;
};

#endif // RBSLOCATION_H
