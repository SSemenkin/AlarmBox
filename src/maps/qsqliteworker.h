#ifndef QSQLITEWORKER_H
#define QSQLITEWORKER_H

#include <QDir>
#include <QtSql>

struct CellInfo {
    QString cellname;
    int angle;
};

class QSqliteWorker : public QObject
{
    Q_OBJECT

public:
    QSqliteWorker(const QString &filepath = "rbsObjects.db",
                  const QString &connectionName = "local_sqlite_connection",
                  QObject *parent = nullptr);
    ~QSqliteWorker();

    QMap<QString, QPointF> getRbsList() const;
    QMap<QString, QVector<CellInfo>> getCellList() const;
    QMap<QString, QString> getAdresses() const;

    bool addRbs(const QString &name, const QPointF &point, const QString &adress) const;
    bool addCell(const QString &rbsName, const QString &cellName, int angle) const;
signals:
    void error(const QString &errorMessage) const;

private:

    QSqlQuery connectAndCreateQuery (QSqlDatabase &database) const;

    QString m_filepath;
    QString m_connectionName;
};
#endif // QSQLITEWORKER_H
