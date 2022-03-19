#ifndef EXCEPTIONSPANEL_H
#define EXCEPTIONSPANEL_H

#include <QTableWidget>
#include <QWidget>

struct DisplayException;

class ExceptionsPanel : public QTableWidget
{
    Q_OBJECT
public:
    ExceptionsPanel(QWidget *parent = nullptr);
    ~ExceptionsPanel();

    void execAddExceptionDialog();
    bool isInException(const QString &controller,
                       const QString &object,
                       const QString &alarmType) const;

private:
    void addException(const QString &controller, const QString &object, const QString &alarmType);
    void removeException();

    bool isDuplicate(const DisplayException &exception) const;

    void setupContextMenu();

    static QString titleToAlarmTypeIndex(int index);
private:
    QMap<QString, QVector<DisplayException>> m_exceptions;
};

#endif // EXCEPTIONSPANEL_H
