#include "rbslocation.h"
#include <QFile>

RbsLocation::RbsLocation(const QString &filename)
{
    updateLocations(filename);
}

UpdateStatus RbsLocation::updateLocations(const QString &filename)
{
    UpdateStatus result;
    if (!m_locations.isEmpty()) {
        m_locations.clear();
    }
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly |
              QIODevice::Text)) {
        return UpdateStatus{} ;
    }

    QStringList rows = QString(file.readAll()).split("\n", Qt::SkipEmptyParts);
    for (int i = 0; i < rows.size(); ++i) {
        QStringList columns = rows.at(i).split(';');
        const QString &object = columns.first();
        const QString &location = columns.last();

        insert(object, location, result);
    }
    file.close();

    if (result.m_status == Status::Duplicate) {
        result.m_description = result.m_description.left(result.m_description.length() - 1) + QObject::tr("\nDuplicates.");
    }

    return result;
}

QString RbsLocation::getLocation(const QString &object) const
{
    QMap<QString, QString>::const_iterator iterator  = m_locations.find(object);

    return iterator == m_locations.end() ?
                QString(QObject::tr("Object location is unknown.")) :
                iterator.value();
}

void RbsLocation::insert(const QString &object, const QString &location, UpdateStatus &status)
{
    QStringList results;

    object.contains('/') ? results << object.split('/', Qt::SkipEmptyParts) : results << object;

    for (int i = 0; i < results.size(); ++i) {
        const QString &key = results.at(i);

        QString prefix = (key.length() == 3 ? "LUG" : "LU");
        if (m_locations.contains(key)) {
            status.m_status = Status::Duplicate;
            status.m_description += key + " ";
        }
        m_locations.insert(key, location);
        m_locations.insert(key + "D", location);
        m_locations.insert(key + "G", location);
        m_locations.insert(prefix + key, location);

        static QStringList cellSuffixes {"A", "B", "C", "G1", "G2", "G3", "D4", "D5", "D6"};
        for (int j = 0; j < cellSuffixes.size(); ++j) {
            m_locations.insert(prefix + key + cellSuffixes.at(j), location);
            m_locations.insert(key + cellSuffixes.at(j), location);
        }
    }
}

