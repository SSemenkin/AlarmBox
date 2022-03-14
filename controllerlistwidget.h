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

    void processFailedControllerAuthentication(Telnet *telnet);
    void processSuccessfullControllerAuthentication(Telnet *telnet);

    void removeController();

signals:
    void addRequested();
    void detailRequested(const QString &hostname);
    void reconnectRequested(const QString &hostname);
    void reconnectRequested(Telnet *telnet);
    void removeRequested(const QString &hostname);
    void editRequested();
protected:
    void contextMenuEvent(QContextMenuEvent *event);
private:
    void addController(Telnet *telnet);
    bool callMethod(void (ControllerListWidget::*method)(const QString&));
private:
    QVector<QString> m_controllersHosts;
    QHash<QString, int> m_hostToRow;
    QPixmap m_okPixmap;
    QPixmap m_noOkPixmap;

    QScopedPointer<QMenu> m_contextMenu;

    QScopedPointer<QAction> m_reconnectAction;
    QScopedPointer<QAction> m_editAction;
    QScopedPointer<QAction> m_removeAction;
    QScopedPointer<QAction> m_detailAction;
    QScopedPointer<QAction> m_addAction;

    QListWidgetItem *m_lastContextItem {nullptr};
};

#endif // CONTROLLERLISTWIDGET_H
