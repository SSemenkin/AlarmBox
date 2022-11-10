#include "databaseupdater.h"
#include <QPointF>
#include <QFile>
#include "maps/qsqliteworker.h"

struct NodeCell {
    QString nodename;
    QString cellid;
    float angle;

    bool operator == (const NodeCell& other) const {
        return nodename == other.nodename &&
                cellid == other.cellid &&
               angle == other.angle;
    }
};
struct MapNode {
    QString name;
    QPointF position;
    bool operator == (const MapNode& other) const {
        return name == other.name && position == other.position;
    }
};

struct NodeInfo {
    QVector<NodeCell> cells;
    QVector<MapNode> nodes;
    QString errorString;

    void addCell(const NodeCell &cell) {
        if (!cells.contains(cell)) {
            cells.push_back(cell);
        }
    }

    void addNode(const MapNode &node) {
        if (!nodes.contains(node)) {
            nodes.push_back(node);
        }
    }
};

QString transform(QString source) {
    return source.replace(',', '.').remove('N').remove('E').left(8);
}

NodeInfo loadFromFile(const QString &filepath)
{
    NodeInfo result;

    QFile f (filepath);
    if (f.open(QIODevice::ReadOnly)) {
        QStringList d = QString(f.readAll()).split("\r\n");
        for (int i = 1; i < d.size(); ++i) {
            QStringList row = d.at(i).split(';', Qt::SkipEmptyParts);

            if (row.isEmpty()) continue;

            result.addCell(NodeCell{row.at(0), row.at(1), row.at(2).toFloat()});
            result.addNode(MapNode{row.at(0), QPointF {transform(row[3]).toFloat(), transform(row[4]).toFloat()}});
        }
    } else  {
        result.errorString = f.errorString();
    }
    return result;
}



DatabaseUpdater::DatabaseUpdater(const QString &sourceFilename, QObject *parent)
    : QObject{parent}
    , m_filename(sourceFilename)
{
    moveToThread(new QThread(this));
    connect(thread(), &QThread::started, this, &DatabaseUpdater::updateDatabase);
    m_busy = true;
    thread()->start();
}

DatabaseUpdater::~DatabaseUpdater()
{
    if (thread()->isRunning()) {
        thread()->quit();
        thread()->wait();
    }
}

void DatabaseUpdater::updateDatabase()
{
     QSqliteWorker worker;
     connect(&worker, &QSqliteWorker::error, this, &DatabaseUpdater::error);
     NodeInfo info = loadFromFile(m_filename);
     if (!info.errorString.isEmpty()) {
         emit error(info.errorString);
         return;
     }

     int counter {0};
     for (const MapNode &node : qAsConst(info.nodes)) {
         worker.addRbs(node.name, node.position, "");
         emitProgress(counter++, info.cells.size() + info.nodes.size());
     }

     for (const NodeCell &cell : qAsConst(info.cells)) {
         worker.addCell(cell.nodename, cell.cellid, cell.angle);
         emitProgress(counter++, info.cells.size() + info.nodes.size());
     }
     emit finished();
     thread()->quit();
     m_busy = false;
}

void DatabaseUpdater::emitProgress(int prog, int totalSize)
{
    emit progress(static_cast<float>(prog) / static_cast<float>(totalSize) * 100.f);
}

