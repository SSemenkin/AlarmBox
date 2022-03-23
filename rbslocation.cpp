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

    QStringList rows = QString(file.readAll()).split("\n");
    for (int i = 0; i < rows.size(); ++i) {
        int firstSpaceIndex = rows.at(i).indexOf(' ');

        QString object = rows.at(i).left(firstSpaceIndex);
        QString location = rows.at(i).right(rows.at(i).length() - firstSpaceIndex);
        m_locations.insert(object, location);
    }
}

QString RbsLocation::getLocation(const QString &object)
{
    QMap<QString, QString>::iterator it = m_locations.find(object);

    return it == m_locations.end() ? QString() : it.value();
}
