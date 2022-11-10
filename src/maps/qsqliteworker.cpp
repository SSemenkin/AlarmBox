#include "qsqliteworker.h"
#include <QMessageBox>

QSqliteWorker::QSqliteWorker(const QString &filepath, const QString &connectionName, QObject *parent) :
    QObject (parent),
    m_filepath (QDir::currentPath() + '/' + filepath),
    m_connectionName (connectionName)
{
    QSqlDatabase db;
    QSqlQuery q = connectAndCreateQuery (db);
    if (!q.exec ("create table if not exists rbs (rbs_name varchar(20) unique, position_X real, position_Y real, adress varchar(60));" )) {
        qDebug() << q.lastError ().text ();
    }
    if (!q.exec ("create table if not exists cell (rbs_name varchar(20), cellname varchar(20) unique, angle integer);")) {
        qDebug() << q.lastError ().text ();
    }
}

QSqliteWorker::~QSqliteWorker()
{
    if (QSqlDatabase::contains (m_connectionName)) {
        QSqlDatabase::removeDatabase (m_connectionName);
    }
}

QMap<QString, QPointF> QSqliteWorker::getRbsList() const
{
    QSqlDatabase db;

    QSqlQuery query = connectAndCreateQuery (db);

    if (query.exec ("select rbs_name, position_X, position_Y from rbs")) {
        QMap <QString, QPointF> result;
        while (query.next ()) {
            result[query.value (0).toString ()] = QPointF(query.value (1).toDouble (), query.value (2).toDouble ());
        }
        return result;
    } else {
        emit error(query.lastError().text());
        return QMap <QString, QPointF> ();
    }
}

QMap<QString, QVector< CellInfo>> QSqliteWorker::getCellList() const
{
    QSqlDatabase db;
    QSqlQuery query = connectAndCreateQuery (db);

    if (query.exec ("select * from cell")) {
        QMap <QString, QVector<CellInfo>> result;
        while (query.next ()) {
            result[query.value (0).toString ()] .push_back(CellInfo{query.value (1).toString (),
                                                                    query.value (2).toInt ()});
        }
        return result;
    } else {
        emit error(query.lastError().text());
        return QMap <QString, QVector<CellInfo>> ();
    }
}

QMap<QString, QString> QSqliteWorker::getAdresses() const
{
    QSqlDatabase db;

    QSqlQuery query = connectAndCreateQuery (db);

    if (query.exec ("select rbs_name, adress from rbs")) {
        QMap <QString, QString> result;
        while (query.next ()) {
            result[query.value (0).toString ()] = query.value (1).toString ();
        }
        return result;
    } else {
        emit error(query.lastError().text());
        return QMap <QString, QString> ();
    }
}

bool QSqliteWorker::addRbs(const QString &name, const QPointF &point, const QString &adress) const
{
    QSqlDatabase db;
    QSqlQuery q = connectAndCreateQuery (db);
    if (q.exec (QString ("insert into rbs (rbs_name, position_X, position_Y, adress) values ('%1', '%2', '%3', '%4')")
                   .arg (name, QString::number (point.x ()), QString::number (point.y ()), adress))) {
        return true;
    } else {
        emit error(q.lastError().text());
        return false;
    }
}

bool QSqliteWorker::addCell(const QString &rbsName, const QString &cellName, int angle) const
{
    QSqlDatabase db;
    QSqlQuery q = connectAndCreateQuery (db);
    if(q.exec (QString ("insert into cell (rbs_name, cellname, angle) values ('%1', '%2', '%3')")
                   .arg (rbsName, cellName, QString::number (angle)))) {
        return true;
    } else {
        emit error(q.lastError().text());
        return false;
    }
}

QSqlQuery QSqliteWorker::connectAndCreateQuery(QSqlDatabase &database) const
{
    if (QSqlDatabase::contains (m_connectionName)) {
        database = QSqlDatabase::database (m_connectionName);
    } else {
        database = QSqlDatabase::addDatabase ("QSQLITE", m_connectionName);
        database.setDatabaseName (m_filepath);
    }

    if (!database.isOpen ()) {
        database.open ();
    }

    return QSqlQuery ("", database);
}
