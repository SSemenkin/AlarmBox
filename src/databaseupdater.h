#ifndef DATABASEUPDATER_H
#define DATABASEUPDATER_H

#include <QObject>

class DatabaseUpdater : public QObject
{
    Q_OBJECT
public:
    explicit DatabaseUpdater(const QString &sourceFilename, QObject *parent = nullptr);
    virtual ~DatabaseUpdater();
    inline bool isBusy() const {
        return m_busy;
    }
signals:
    void progress(int);
    void error(const QString &errorMessage);
    void finished();
private:
    void updateDatabase();
    void emitProgress(int progress, int totalSize);
private:
    bool m_busy {false};
    QString m_filename;
};

#endif // DATABASEUPDATER_H
