#include "alarminterrogator.h"
#include <QSharedPointer>
#include <network/telnet.h>

#define makeAndReturnStaticQString(x) static QString d((x)); \
    return d;

uint AlarmInterrogator::timeDelta {3 * 60 * 1000}; // by default 3 minutes

AlarmInterrogator::AlarmInterrogator(const QList<QSharedPointer<Telnet> > &controllerList, QObject *parent)
    : QObject{parent}
    , m_timer(new QTimer(this))
    , m_controllerList(controllerList)
    , m_answerExpected(m_controllerList.size() * interrogatorCommands().size())
{

    for (int i = 0; i < m_controllerList.size(); ++i) {
        connect(m_controllerList.at(i).data(), &Telnet::commandExecuted,
                this, &AlarmInterrogator::processOutput);
        connect(m_controllerList.at(i).data(), &Telnet::errorOccured,
                this, &AlarmInterrogator::processErrors);
        m_controllerList.at(i)->executeCommand(rxtcp());
    }



    connect(m_timer, &QTimer::timeout, this, &AlarmInterrogator::interrogateControllers);
    interrogateControllers();
    m_timer->start(timeDelta);
}

QString AlarmInterrogator::joinLastN(const QStringList &input, int count, const char *separator)
{
    QString result;
    for (int i = input.length() - count; i < input.length(); ++i) {
        result += input.at(i);
        if (i < input.length() - 1) {
            result += separator;
        }
    }
    return result;
}

const QStringList& AlarmInterrogator::interrogatorCommands()
{
    static QStringList commandList {rxasp(), rxmsp(), rxstp(), rlcrp()};
    return commandList;
}

const QString &AlarmInterrogator::rxasp()
{
    makeAndReturnStaticQString("rxasp:moty=rxocf;");
}

const QString &AlarmInterrogator::rxmsp()
{
    makeAndReturnStaticQString("rxmsp:moty=rxocf;");
}

const QString &AlarmInterrogator::rxstp()
{
    makeAndReturnStaticQString("rlstp:cell=all, state=halted;");
}

const QString &AlarmInterrogator::rlcrp()
{
    makeAndReturnStaticQString("rlcrp:cell=all;");
}

const QString &AlarmInterrogator::rxtcp()
{
    makeAndReturnStaticQString("rxtcp:moty=rxotg;");
}

void AlarmInterrogator::interrogateControllers()
{
    for (int i = 0; i < m_controllerList.size(); ++i) {
        for (int j = 0; j < interrogatorCommands().size(); ++j) {
            m_controllerList.at(i)->executeCommand(interrogatorCommands().at(j));
        }
    }
}

void AlarmInterrogator::processOutput(const QString &output)
{
    if (output.contains(rxasp())) {
        processRXASP(output);
        ++m_answerReceived;

    } else if (output.contains(rxmsp())) {
        processRXMSP(output);
        ++m_answerReceived;

    } else if (output.contains(rxstp())) {
        processRXSTP(output);
        ++m_answerReceived;

    } else if (output.contains(rlcrp())) {
        processRLCRP(output);
        ++m_answerReceived;

    } else if (output.contains(rxtcp())) {
        updateObjectHierarchy(output);
    } else {
        qDebug() << "Undefined output";
    }


    if (m_answerReceived == m_answerExpected) {
        emit alarmsReceived(m_alarms);
        m_answerReceived = 0;
    }
}

void AlarmInterrogator::processRXASP(const QString &print)
{
    const QStringList answerRows = print.split("\n", Qt::SkipEmptyParts);

    Alarm alarm;
    for (int i = 0; i < answerRows.size(); ++i) {
        const QString& row = answerRows.at(i);

        if (row.contains("RXOCF-") && !row.contains("UNAFFECTED")) {
            QStringList rElements = row.split(' ', Qt::SkipEmptyParts);

            alarm.m_object = rElements.at(1);
            alarm.m_controllerTitle = fromController()->parsedTitle();
            alarm.m_controller = fromController()->hostname();
            alarm.m_raisedTime = QDateTime::currentDateTime();
            alarm.m_description = joinLastN(rElements, rElements.size() - 2, " ");
            m_alarms.push_back(alarm);
        }
    }
}

void AlarmInterrogator::processRXMSP(const QString &print)
{

}

void AlarmInterrogator::processRXSTP(const QString &print)
{

}

void AlarmInterrogator::processRLCRP(const QString &print)
{
}

void AlarmInterrogator::processErrors(const QString &errorText)
{

}

Telnet *AlarmInterrogator::fromController() const
{
    return static_cast<Telnet*>(sender());
}

void AlarmInterrogator::updateObjectHierarchy(const QString &print)
{
    QString tg;
    RBS rbs;
    const QStringList rows = print.split('\n', Qt::SkipEmptyParts);
    for (int i = 0; i < rows.size(); ++i) {
        const QString &row = rows.at(i);
        if(row.rightRef(1) == "0" || row.rightRef(1) == "1") {
            QStringList rE = row.split(" ", Qt::SkipEmptyParts);
            if (row.contains("RXOTG-")) {
                if (!tg.isEmpty()) {
                    m_objectHierarchy[fromController()->hostname()].push_back(rbs);
                    rbs.m_cells.clear();
                }
                tg = rE.first().split('-').at(1);
                rbs.m_tg = tg;
                rbs.m_name = rE.at(1).left(rE.at(1).length() - 1);
                rbs.m_cells.insert(rE.at(1));
            } else {
                rbs.m_cells.insert(rE.at(0));
            }
        }
    }
}
