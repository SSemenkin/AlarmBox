#ifndef ALARMINTERROGATOR_H
#define ALARMINTERROGATOR_H

#include <QObject>
#include <QTimer>

#include "types/alarm.h"

class Telnet;


class AlarmInterrogator : public QObject
{
    Q_OBJECT
public:
    explicit AlarmInterrogator(const QList<QSharedPointer<Telnet>> &controllerList,
                               QObject *parent = nullptr);
    static QString joinLastN(const QStringList &input, int count, const char* separator = "");

    void onControllerAdded();
    void onControllerRemoved();
    void onPeriodChanged(uint32_t delta);
    void onAutoRefreshChanged(bool state);

    void onActivateRBSRequested(const QString &object, const QString &controllerHostname);
    void onDeactivateRBSRequested(const QString &object, const QString &controllerHostname);

    void interrogateControllers() const;

    QHash<Telnet*, QMap<QString, QString>> objectsHierarchy() const;
signals:
    void alarmsReceived(const QVector<Alarm> &currentAlarms);
    void noMMLError(const QString &text, Telnet *controller);
    void MMLError(const QString &text, Telnet *controller);
    void hierarchyUpdated();
protected:
    static const QString& rxasp();
    static const QString& rxmsp();
    static const QString& rxstp();
    static const QString& rlcrp();
    static const QString& rxtcp();
    static const QStringList& interrogatorCommands();
    static const QString &rxble(); // activate
    static const QString &rxbli(); // deactivate
private:
    ///@brief обработка всех поступивших принтов команд
    void processOutput(const QString &output);
    ///@brief тут расположены аварии по типу OML/PERMANENT и другие
    void processRXASP(const QString &print);
    ///@brief здесь мы увидим заблокированные RBS
    void processRXMSP(const QString &print);
    ///@brief здесь будут захалченные сектора
    void processRXSTP(const QString &print);
    ///@brief здесь будут сектора с показателем 0, значит они не работают
    void processRLCRP(const QString &print);
    void processErrors(const QString &errorText);
    ///@brief контроллер, с которого поступила последяя команда
    Telnet* fromController() const;
    void updateObjectHierarchy(const QString &print);

    Alarm createDefaultAlarm(Alarm::Category category) const;
    bool isRBSinAlarm(const QString &cellname) const;

    void connectController(QSharedPointer<Telnet> controller);
    void processControllerAuthentication(bool state);

    void supportConnection() const;
    void calcExpectedAnswers();
private:
    static uint64_t timeDelta;

    QTimer *m_timer; // таймер для опроса
    QTimer *m_defaultTimer; // таймер для поддержания соединения, отслыает "\r\n" см. Telnet::sendConnect();

    const QList<QSharedPointer<Telnet>> &m_controllerList;
    uint32_t m_answerReceived {0};
    uint32_t m_answerExpected;

    QVector<Alarm> m_alarms;
    QHash<Telnet*, QMap<QString, QString>> m_fromTGtoRBS;
    bool m_autoRefresh;
};

#endif // ALARMINTERROGATOR_H
