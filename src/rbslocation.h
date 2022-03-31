#ifndef RBSLOCATION_H
#define RBSLOCATION_H

#include <QMap>

enum class Status : int {
    NoError = 0,
    Duplicate
};

struct UpdateStatus {
    Status m_status {Status::NoError};
    QString m_description;
};

class RbsLocation
{
public:
    RbsLocation(const QString &filename);

    UpdateStatus updateLocations(const QString &filename);
    QString getLocation(const QString &object);

private:
    void insert(const QString& object, const QString &location, UpdateStatus& status);
private:
    QMap<QString, QString> m_locations;
};

#endif // RBSLOCATION_H
