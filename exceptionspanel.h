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

private:
    QMap<QString, QMap<QString, DisplayException>> m_exceptions;
};

#endif // EXCEPTIONSPANEL_H
