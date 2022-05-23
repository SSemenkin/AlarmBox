#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QSettings>
#include <QCoreApplication>

#include "network/telnet.h"
#include "types/types.h"

class Settings : protected QSettings // наследование protected для того чтобы закрыть доступ к методам базового класса
                                     // и оставить такую возможность только для наследников и самого себя.
{
    Q_OBJECT
public:
    static Settings* instance();

    void setControllersInfos(const QList<QSharedPointer<Telnet>> &controllersList);
    QList<ControllerInfo> getControllersInfos();

    static QString decodeEncodeData(const QString &input, const QString &key = "%31_)*&z;");

    void setLocale(const QLocale &locale);
    QLocale getLocale() const;

    void setAutoRefreshEnabled(bool state);
    bool getIsAutoRefreshEnabled() const;

    void setRefreshPeriod(uint32_t period);
    uint32_t getRefreshPeriod() const;

    void setAlarmComments(const QMap<QString, QMap<QString, AlarmComment>> &controllerComments);
    QMap<QString, QMap<QString, AlarmComment>> getAlarmComments() const;

    void setDisplayExceptions(const QMap<QString, QVector<DisplayException>>& exceptions);
    QMap<QString, QVector<DisplayException>> getExceptions() const;

    void setWindowGeometry(const QRect &windowRect);
    QRect getWindowGeometry() const;

    void setSplitterSizes(const QList<int> &sizes);
    QList<int> getSplitterSizes() const;

    void setFont(const QFont &font);
    QFont getFont() const;

    void setLocationFilepath(const QString &filepath);
    QString getLocationFilepath() const;

    void setExistingAlarms(const QVector<Alarm> &alarms);
    QVector<Alarm> getExistingAlarms() const;

    void setNodeList(const QVector<Node> &nodes);
    QVector<Node> getNodes() const;

    void setLastUpdates(const QString &content);
    QString getLastUpdates() const;

    void setThemeIndex(int index);
    int getThemeIndex() const;


protected:
    explicit Settings(QObject *parent = nullptr);

    Q_DISABLE_COPY_MOVE(Settings);

private:
    ///@brief Метод сериализации данных в JSON формат.
    /// @param data контейнер данных
    /// накладывает на объект, такие требование что получаемый при итерации объект должен быть сам итерируем
    /// и внутри такого объекта необходимо реализовать метод QVariantMap toVariantMap()
    /// @param filename
    /// имя файла (будет распологаться в user/appdata/local/programm/filename.prefix
    template<typename T>
    void serialize(const T &data, const QString &filename) const;

    template<typename T>
    void serializeV(const T &data, const QString &filename) const;

    template<typename T>
    QVector<T> deserialize(const QString &filename) const;
};

#endif // SETTINGS_H
