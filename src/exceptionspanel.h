#ifndef EXCEPTIONSPANEL_H
#define EXCEPTIONSPANEL_H

#include <QTableWidget>
#include <QWidget>

struct DisplayException;
class Alarm;

class ExceptionsPanel : public QTableWidget
{
    Q_OBJECT
public:
    ExceptionsPanel(QWidget *parent = nullptr);
    ~ExceptionsPanel();

    void execAddExceptionDialog();
    bool isInException(const QString &controller, const QString &object, const QString &alarmType) const;
    bool isInException(const Alarm &alarm) const;

private:
    void addException(const QString &controller, const QString &object, const QString &alarmType);
    void removeException();

    bool isDuplicate(const DisplayException &exception) const;

    void setupContextMenu();

    static QString titleToAlarmTypeIndex(int index);
    static int indexToAlarmTitle(const QString &title);
    static QStringList alarmTitles();



private:
    QMap<QString, QVector<DisplayException>> m_exceptions;
};

#endif // EXCEPTIONSPANEL_H
