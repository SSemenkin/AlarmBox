#include "alarminterrogator.h"
#include <QSharedPointer>
#include <network/telnet.h>
#include "settings.h"

#define makeAndReturnStaticQString(x) static QString d((x)); \
    return d;

uint64_t AlarmInterrogator::timeDelta {Settings::instance()->getRefreshPeriod() * 60 * 1000}; // by default 180 seconds

AlarmInterrogator::AlarmInterrogator(const QList<QSharedPointer<Telnet>> &controllerList, QObject *parent)
    : QObject{parent}
    , m_timer(new QTimer(this))
    , m_defaultTimer(new QTimer(this))
    , m_controllerList(controllerList)
    , m_autoRefresh(Settings::instance()->getIsAutoRefreshEnabled())
{
    for (int i = 0; i < m_controllerList.size(); ++i) {
        QSharedPointer<Telnet> controller = m_controllerList.at(i);
        connectController(controller);
        m_receivedAnswers[controller.data()] = 0;
    }

    connect(m_timer, &QTimer::timeout, this, [this] () {
        if (m_autoRefresh) {
            interrogateControllers();
        }
    });
    connect(m_defaultTimer, &QTimer::timeout, this, &AlarmInterrogator::supportConnection);

    interrogateControllers();
    m_timer->start(timeDelta);
    m_defaultTimer->start(60000);
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
    interrogateControllers();
}

void AlarmInterrogator::onControllerRemoved(Telnet *t)
{
    m_receivedAnswers.remove(t);
}

void AlarmInterrogator::onPeriodChanged(uint32_t delta)
{
    timeDelta = delta * 60 * 1000;
    m_timer->stop();
    m_timer->start(timeDelta);
}

void AlarmInterrogator::onAutoRefreshChanged(bool state)
{
    m_autoRefresh = state;
}

void AlarmInterrogator::onActivateRBSRequested(const QString &object, const QString &controllerHostname)
{
    for (auto it = m_fromTGtoRBS.begin(); it != m_fromTGtoRBS.end(); ++it) {
        if (it.key()->hostname() == controllerHostname) {
            QString tg = it.value().key(object);
            if (!tg.isEmpty()) {
                it.key()->executeCommand(rxble().arg(tg));
            }

            break;
        }
    }
}

void AlarmInterrogator::onDeactivateRBSRequested(const QString &object, const QString &controllerHostname)
{
    for (auto it = m_fromTGtoRBS.begin(); it != m_fromTGtoRBS.end(); ++it) {
        if (it.key()->hostname() == controllerHostname) {
            QString tg = it.value().key(object);
            if (!tg.isEmpty()) {
                it.key()->executeCommand(rxbli().arg(tg));
            }
            break;
        }
    }
}

const QStringList& AlarmInterrogator::interrogatorCommands()
{
    static QStringList commandList {rxasp(), rxmsp(), rxstp(), rlcrp()};
    return commandList;
}

const QString &AlarmInterrogator::rxble()
{
    makeAndReturnStaticQString("rxble:mo=rxotg-%1,subord;");
}

const QString &AlarmInterrogator::rxbli()
{
    makeAndReturnStaticQString("rxbli:mo=rxotg-%1,subord,force;");
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
    if (!isCanUpdate()) {
        return;
    }
    for (int i = 0; i < m_controllerList.size(); ++i) {
        for (int j = 0; j < interrogatorCommands().size(); ++j) {
            m_controllerList.at(i)->executeCommand(interrogatorCommands().at(j));
        }
    }
}

bool AlarmInterrogator::isCanUpdate() const
{
    for (auto it = m_receivedAnswers.begin(); it != m_receivedAnswers.end(); ++it) {
        if (it.value() != 0) {
            return false;
        }
    }
    return true;
}

QHash<Telnet *, QMap<QString, QString>> AlarmInterrogator::objectsHierarchy() const
{
    return m_fromTGtoRBS;
}

void AlarmInterrogator::processOutput(const QString &output)
{
    if (output.contains(rxble().left(5)) || output.contains(rxbli().left(5))) {
        return;
    }
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
        qDebug() << "Undefined Telnet printout " + output;
        Q_ASSERT(false && "Undefinded output");
    }

     ++m_receivedAnswers[fromController()];
    if (isAllCommandsReceived()) {
        std::sort(m_alarms.begin(), m_alarms.end());
        emit alarmsReceived(m_alarms);
        m_alarms.clear();

        for (auto it = m_receivedAnswers.begin(); it != m_receivedAnswers.end(); ++it) {
            *it = 0;
        }
    }
}

void AlarmInterrogator::processRXASP(const QString &print)
{
    const QStringList answerRows = print.split("\n", Qt::SkipEmptyParts);

    Alarm alarm = createDefaultAlarm(Alarm::Category::A1);
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
    Alarm alarm = createDefaultAlarm(Alarm::Category::A2);
    const QStringList rows = print.split("\n", Qt::SkipEmptyParts);
    for (int i = 0; i < rows.size(); ++i) {
        const QString &row = rows.at(i);

        if (row.contains("MBL")) {
            alarm.m_object = m_fromTGtoRBS[fromController()]
                    [row.split(' ', Qt::SkipEmptyParts).first().split('-').at(1)];
            m_alarms.push_back(alarm);
        }
    }
}

void AlarmInterrogator::processRXSTP(const QString &print)
{
    Alarm alarm = createDefaultAlarm(Alarm::Category::A3);
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
    Alarm alarm = createDefaultAlarm(Alarm::Category::A4);
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

void AlarmInterrogator::processError(const QString &errorText)
{
    bool isContainsFinishToken {false};
    for (const QString &token : Telnet::finishTokens()) {
        if(errorText.contains(token)) {
            isContainsFinishToken = true;
            break;
        }
    }
    if (isContainsFinishToken) {
        if (errorText.contains(Telnet::finishTokens().at(1)) || // NOT ACCEPTED
            errorText.contains(Telnet::finishTokens().at(3)))   // FUNCTION BUSY
        {
            processOutput(fromController()->lastCommand() + errorText);
            return;
        }
        emit MMLError(errorText, fromController());
    } else {
        emit noMMLError(errorText, fromController());
    }
}

Telnet *AlarmInterrogator::fromController() const
{
    return static_cast<Telnet*>(sender());
}

void AlarmInterrogator::updateObjectHierarchy(const QString &print)
{
    QString tg;
    QString rbs;
    const QStringList rows = print.split('\n', Qt::SkipEmptyParts);
    for (int i = 0; i < rows.size(); ++i) {
        const QString &row = rows.at(i);
        if(row.rightRef(1) == "0" || row.rightRef(1) == "1") {
            QStringList rE = row.split(" ", Qt::SkipEmptyParts);
            if (row.contains("RXOTG-")) {
                if (!tg.isEmpty()) {
                    m_fromTGtoRBS[fromController()][tg] = rbs;
                }
                tg = rE.first().split('-').at(1);
                rbs = rE.at(1).left(rE.at(1).length() - 1);
            }
        }
    }

    m_fromTGtoRBS[fromController()][tg] = rbs;

    if (m_fromTGtoRBS.size() == m_controllerList.size()) {
        emit hierarchyUpdated();
    }
}

Alarm AlarmInterrogator::createDefaultAlarm(Alarm::Category category) const
{
    Alarm a;
    a.m_raisedTime = QDateTime::currentDateTime();
    a.m_controller = fromController()->hostname();
    a.m_controllerTitle = fromController()->parsedTitle();
    a.m_category = category;
    a.m_description = Alarm::descriptionFromCategory(category);
    a.m_source = fromController();
    return a;
}

bool AlarmInterrogator::isRBSinAlarm(const QString &cellname) const
{
    for (int i = 0; i < m_alarms.size(); ++i) {
        if (m_alarms.at(i).m_object == cellname.left(cellname.length() - 1)) {
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
            this, &AlarmInterrogator::processError);
    controller->executeCommand(rxtcp());
}

void AlarmInterrogator::processControllerAuthentication(bool state)
{
    if (state) {
        fromController()->executeCommand(rxtcp());
        interrogateControllers();
    }
}

void AlarmInterrogator::supportConnection() const
{
    for (int i = 0; i < m_controllerList.size(); ++i) {
        for (int j = 0; j < interrogatorCommands().size(); ++j) {
            if (m_controllerList.at(i)->isLoggedInNode()) {
                m_controllerList.at(i)->sendConnect();
            }
        }
    }
}

bool AlarmInterrogator::isAllCommandsReceived() const
{
    for (auto it = m_receivedAnswers.begin(); it != m_receivedAnswers.end(); ++it) {
        if (it.value() != (uint)interrogatorCommands().size()) {
            return false;
        }
    }
    return true;
}
