#include "alarminterrogator.h"
#include <QSharedPointer>
#include <network/telnet.h>

#define makeAndReturnStaticQString(x) static QString d((x)); \
    return d;

uint AlarmInterrogator::timeDelta { 15 * 1000}; // by default 30 seconds

AlarmInterrogator::AlarmInterrogator(const QList<QSharedPointer<Telnet> > &controllerList, QObject *parent)
    : QObject{parent}
    , m_timer(new QTimer(this))
    , m_controllerList(controllerList)
    , m_answerExpected(m_controllerList.size() * interrogatorCommands().size())
{

    for (int i = 0; i < m_controllerList.size(); ++i) {
        connectController(m_controllerList.at(i));
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

void AlarmInterrogator::onControllerAdded()
{
    connectController(m_controllerList.last());
    m_answerExpected = m_controllerList.size() * interrogatorCommands().size();
    interrogateControllers();
}

void AlarmInterrogator::onControllerRemoved()
{
    m_answerExpected = m_controllerList.size()
            * interrogatorCommands().size();
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

void AlarmInterrogator::interrogateControllers() const
{
    for (int i = 0; i < m_controllerList.size(); ++i) {
        for (int j = 0; j < interrogatorCommands().size(); ++j) {
            m_controllerList.at(i)->executeCommand(interrogatorCommands().at(j));
        }
    }
}

void AlarmInterrogator::processOutput(const QString &output)
{
    if (output.contains(rxtcp())) {
        updateObjectHierarchy(output);
        return;
    }
    if (output.contains(rxasp())) {
        processRXASP(output);
    } else if (output.contains(rxmsp())) {
        processRXMSP(output);
    } else if (output.contains(rxstp())) {
        processRXSTP(output);
    } else if (output.contains(rlcrp())) {
        processRLCRP(output);
    } else {
        qDebug() << "Undefined output";
    }
    ++m_answerReceived;

    if (m_answerReceived == m_answerExpected) {
        emit alarmsReceived(m_alarms);
        m_answerReceived = 0;
    }
}

void AlarmInterrogator::processRXASP(const QString &print)
{
    const QStringList answerRows = print.split("\n", Qt::SkipEmptyParts);

    Alarm alarm = createDefaultAlarm(Alarm::AlarmCategory::A1);
    for (int i = 0; i < answerRows.size(); ++i) {
        const QString& row = answerRows.at(i);

        if (row.contains("RXOCF-") && !row.contains("UNAFFECTED")) {
            QStringList rElements = row.split(' ', Qt::SkipEmptyParts);

            alarm.m_object = rElements.at(1);
            alarm.m_description = joinLastN(rElements, rElements.size() - 2, " ");
            m_alarms.push_back(alarm);
        }
    }
}

void AlarmInterrogator::processRXMSP(const QString &print)
{
    Alarm alarm = createDefaultAlarm(Alarm::AlarmCategory::A2);
    alarm.m_description = tr("Manually Blocked");
    const QStringList rows = print.split("\n", Qt::SkipEmptyParts);
    for (int i = 0; i < rows.size(); ++i) {
        const QString &row = rows.at(i);

        if (row.contains("MBL")) {
            alarm.m_object = m_fromTGtoRBS[alarm.m_controller]
                    [row.split(' ', Qt::SkipEmptyParts).first().split('-').at(1)];
            m_alarms.push_back(alarm);
        }
    }
}

void AlarmInterrogator::processRXSTP(const QString &print)
{
    Alarm alarm = createDefaultAlarm(Alarm::AlarmCategory::A3);
    alarm.m_description = tr("Halted");
    const QStringList rows = print.split("\n", Qt::SkipEmptyParts);
    for (int i = 0; i < rows.size(); ++i) {
        const QString &row  = rows.at(i);

        if (row.contains("HALTED") && row[0] != 32) {
            alarm.m_object = row.split(' ').first();
            m_alarms.push_back(alarm);
        }
    }
}

void AlarmInterrogator::processRLCRP(const QString &print)
{
    Alarm alarm = createDefaultAlarm(Alarm::AlarmCategory::A4);
    alarm.m_description = tr("Not works");
    const QStringList rows = print.split("\n", Qt::SkipEmptyParts);
    for (int i = 0; i < rows.size(); ++i) {
        const QString &row = rows.at(i);

        if (row[row.length()-1] == "0" && row.split(' ', Qt::SkipEmptyParts)[3] == "0") {
            alarm.m_object = row.split(' ').first();
            if (!isRBSinAlarm(alarm.m_object)) {
                m_alarms.push_back(alarm);
            }
        }
    }

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
                    m_fromTGtoRBS[fromController()->hostname()][tg] = rbs.m_name;
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

    m_objectHierarchy[fromController()->hostname()].push_back(rbs);
    m_fromTGtoRBS[fromController()->hostname()][tg] = rbs.m_name;
    rbs.m_cells.clear();
}

Alarm AlarmInterrogator::createDefaultAlarm(Alarm::AlarmCategory category) const
{
    Alarm a;
    a.m_raisedTime = QDateTime::currentDateTime();
    a.m_controller = fromController()->hostname();
    a.m_controllerTitle = fromController()->parsedTitle();
    a.m_category = category;
    return a;
}

bool AlarmInterrogator::isRBSinAlarm(const QString &cellname) const
{
    for (int i = 0; i < m_alarms.size(); ++i) {
        if (m_alarms.at(i).m_object == cellname.left(cellname.length() - 1) &&
                m_alarms.at(i).m_controller == fromController()->hostname()) {
            return true;
        }
    }
    return false;
}

void AlarmInterrogator::connectController(QSharedPointer<Telnet> controller)
{
    connect(controller.data(), &Telnet::commandExecuted,
            this, &AlarmInterrogator::processOutput);
    connect(controller.data(), &Telnet::errorOccured,
            this, &AlarmInterrogator::processErrors);
//    if (!controller.data()->isLoggedInNode())
//    connect(controller.data(), &Telnet::loginState,
//            this, &AlarmInterrogator::processControllerAuthentication);
    controller->executeCommand(rxtcp());
}

void AlarmInterrogator::processControllerAuthentication(bool state)
{
    if (state) {
        fromController()->executeCommand(rxtcp());
        interrogateControllers();
    }
}
