#ifndef ALARMINTERROGATOR_H
#define ALARMINTERROGATOR_H

#include <QObject>
#include <QTimer>
#include <QDateTime>
#include <QSet>
#include <QDebug>

class Telnet;

struct RBS{
    QString m_tg;
    QString m_name;
    QSet<QString> m_cells;

    friend QDebug operator << (QDebug d, const RBS& rbs) {
        d << "RBS {TG:" << rbs.m_tg << "; Name: " << rbs.m_name << "; Cells {\n";
        for (auto it = rbs.m_cells.begin(); it != rbs.m_cells.end(); ++it) {
            d << *it << ",";
        }
        d << "\n}\n}";
        return d;
    }
};

struct Alarm {

    enum class State : int {
        Invalid = -1,
        Normal,
        Raised,
        Cleared
    };

    enum class AlarmCategory : int {
        A1 = 0,
        A2,
        A3,
        A4
    };

    Alarm() = default;


    QString m_object;
    QString m_description;
    QString m_controller;
    QString m_controllerTitle;
    QDateTime m_raisedTime;
    QDateTime m_clearedTime;
    State m_state {State::Raised};
    AlarmCategory m_category;

    bool operator== (const Alarm& other) const {
        return m_object == other.m_object &&
                m_description == other.m_description &&
                m_controller == other.m_controller;
    }
    bool operator < (const Alarm& other) const {
        return m_object < other.m_object;
    }

    QVariantMap toVariantMap() const {
        return QVariantMap({std::make_pair("object", m_object),
                            std::make_pair("description", m_description),
                            std::make_pair("controller", m_controller),
                            std::make_pair("controllerTitle", m_controllerTitle),
                            std::make_pair("raisedTime", m_raisedTime),
                            std::make_pair("state", static_cast<int>(m_state)),
                            std::make_pair("category", static_cast<int>(m_category))});
    }

    static Alarm fromVariantMap(const QVariantMap& d) {
      Alarm r;
      r.m_object = d["object"].toString();
      r.m_description = d["description"].toString();
      r.m_controller = d["controller"].toString();
      r.m_controllerTitle = d["controllerTitle"].toString();
      r.m_raisedTime = d["raisedTime"].toDateTime();
      r.m_state = static_cast<Alarm::State>(d["state"].toInt());
      r.m_category = static_cast<Alarm::AlarmCategory>(d["category"].toInt());
      return r;
    };
};

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
    void interrogateControllers() const;
signals:
    void alarmsReceived(const QVector<Alarm> &currentAlarms);
    void noMMLError(const QString &text, Telnet *controller);
    void MMLError(const QString &text, Telnet *controller);
protected:
    static const QString& rxasp();
    static const QString& rxmsp();
    static const QString& rxstp();
    static const QString& rlcrp();
    static const QString& rxtcp();
    static const QStringList& interrogatorCommands();
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

    Alarm createDefaultAlarm(Alarm::AlarmCategory category) const;
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
    QMap<QString, QMap<QString, QString>> m_fromTGtoRBS;
};

#endif // ALARMINTERROGATOR_H
