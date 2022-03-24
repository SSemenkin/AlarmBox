#include "rbslocation.h"
#include <QFile>

RbsLocation::RbsLocation(const QString &filename)
{
    updateLocations(filename);
}

void RbsLocation::updateLocations(const QString &filename)
{
    if (!m_locations.isEmpty()) {
        m_locations.clear();
    }
    QFile file(filename);
    file.open(QIODevice::ReadOnly |
              QIODevice::Text);

    QStringList rows = QString(file.readAll()).split("\n", Qt::SkipEmptyParts);
    for (int i = 0; i < rows.size(); ++i) {
        QStringList columns = rows.at(i).split(';');
        const QString &object = columns.first();
        const QString &location = columns.last();

        insert(object, location);
    }
    file.close();
}

QString RbsLocation::getLocation(const QString &object)
{
    QMap<QString, QString>::iterator rbs_iterator  = m_locations.find(object);
    QMap<QString, QString>::iterator cell_iterator = m_locations.find(object.left(object.length() - 1));

    if (rbs_iterator == m_locations.end() && cell_iterator == m_locations.end()) {
        return QString(QObject::tr("Object location is unknown."));
    } else if (rbs_iterator != m_locations.end()) {
        return rbs_iterator.value();
    } else {
        return cell_iterator.value();
    }
}

void RbsLocation::insert(const QString &object, const QString &location)
{
    if (object.contains("/")) {
        QStringList objects = object.split("/", Qt::SkipEmptyParts);
        for (int i = 0; i < objects.size(); ++i) {
            m_locations.insert(objects.at(i), location);
            m_locations.insert("LUG" + objects.at(i), location);
            m_locations.insert("LU" + objects.at(i), location);
        }
    } else {
        m_locations.insert(object, location);
        m_locations.insert("LUG" + object, location);
        m_locations.insert("LU" + object, location);
    }
}
