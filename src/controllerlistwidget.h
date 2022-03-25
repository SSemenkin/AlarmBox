#ifndef CONTROLLERLISTWIDGET_H
#define CONTROLLERLISTWIDGET_H

#include <QListWidget>
#include <QWidget>
#include <QMenu>

class Telnet;

class ControllerListWidget : public QListWidget
{
    Q_OBJECT
public:
    ControllerListWidget(QWidget *parent = nullptr);

    void processFailedControllerAuthentication(Telnet *controller);
    void processSuccessfullControllerAuthentication(Telnet *controller);

    void processControllerNoMMLError(const QString &errorText, Telnet *controller);
    void processControllerMMLError(const QString &errorText, Telnet *controller);

    void removeController();
signals:
    void addRequested();
    void detailRequested(const QString &hostname);
    void reconnectRequested(const QString &hostname);
    void reconnectRequested(Telnet *telnet);
    void removeRequested(const QString &hostname);
    void editRequested();
    void controllerSelectionChanged(const QString &hostname);
protected:
    void contextMenuEvent(QContextMenuEvent *event);
private:
    void addController(Telnet *telnet);
    bool callMethod(void (ControllerListWidget::*method)(const QString&));
    void askForReconnect(Telnet *controller, const QString &error = QString());
private:
    QVector<QString> m_controllersHosts;
    QHash<QString, int> m_hostToRow;
    QIcon m_okIcon;
    QIcon m_noOkIcon;
    QIcon m_undefIcon;

    QScopedPointer<QMenu> m_contextMenu;

    QScopedPointer<QAction> m_reconnectAction;
    QScopedPointer<QAction> m_editAction;
    QScopedPointer<QAction> m_removeAction;
    QScopedPointer<QAction> m_detailAction;
    QScopedPointer<QAction> m_addAction;
};

#endif // CONTROLLERLISTWIDGET_H
