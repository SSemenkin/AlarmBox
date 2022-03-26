#ifndef TELNET_H
#define TELNET_H

#include <QObject>
#include <QString>
#include <QQueue>
#include <QFile>

#include "QTelnet.h"

class Telnet : public QObject
{
    Q_OBJECT
public:
    explicit Telnet(const QString &nodeTitle, const QString &hostname, const QString& username, const QString &password,
                    const uint8_t port = 23, QObject *parent = nullptr);
    explicit Telnet(QObject *parent = nullptr);

    virtual ~Telnet();
    ///@brief call this method when you need to try connect with network element
    /// before you need subscribe on SIGNAL loginStateChanged(bool state), so you can check
    /// your data is valid.
    void connectToNode();
    void reconnect();

    bool isLoggedInNode() const;

    ///@brief interface to communicate with telnet.
    void executeCommand(const QString &command);

    void sendRelease();
    void sendConnect();

    QString nodeTitle() const;
    void setNodeTitle(const QString &title);

    QString hostname() const;
    void setHostname(const QString &host);

    QString username() const;
    void setUsername(const QString &username);

    QString password() const;
    void setPassword(const QString &password);

    uint8_t port() const;
    void setPort(uint8_t port);

    QString parsedTitle() const;
    QString lastCommand() const;

    QAbstractSocket::SocketState state() const;

    static const QStringList& finishTokens();


signals:
    ///
    /// @brief emitted when Telnet received terminating symbol.
    ///
    void commandExecuted(const QString &answer);
    void loginStateChanged(bool state);
    void errorOccured(const QString &detail);
private:
    /// @brief struct contains info about network element and info to authentication to him.
    struct AuthenticationData{
        AuthenticationData(const QString &node, const QString &host,
                            const QString &user, const QString &password,
                            const uint8_t port) : nodeTitle (node),
                            hostname (host), username(user), password(password),
                            port(port) {}
        QString nodeTitle;
        QString hostname;
        QString username;
        QString password;
        uint8_t port {23};
    };

    enum class EricssonTerminalProgressState : int8_t {
        InAuthentication,
        InMML
    };

    enum class QueueState : int8_t {
        Write,
        Read
    };

    QueueState queueState {QueueState::Write};
    EricssonTerminalProgressState terminalProgressState {EricssonTerminalProgressState::InAuthentication};
    AuthenticationData authData;

    QString buffer;
    QStringList commands;
    QString m_lastCommand;

    QTelnet *telnet;

    QChar disconnectSymbol {'\x04'};
    QString connectSymbol {"\r\n"};

    QString parsedName {"Unknown"};

    bool isLogged {false};

    QAbstractSocket::SocketState m_state {QAbstractSocket::SocketState::UnconnectedState};
    QFile m_file;


private Q_SLOTS:
    void receiveData(const char *data, int length);
    void authenticationHandler(const QString &responce);
    void mmlHandler(const QString &responce);
    void writeIfStateEnabled();
    void resetState();
    void processSocketError(QAbstractSocket::SocketError error);
};


#endif // TELNET_H
