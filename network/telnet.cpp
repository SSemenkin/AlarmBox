#include "telnet.h"

#include <QTimer>


Telnet::Telnet(const QString &nodeTitle, const QString &hostname, const QString &username,
               const QString &password, const uint8_t port, QObject *parent) :
    QObject(parent),
    authData(nodeTitle, hostname, username, password, port),
    telnet(new QTelnet(this))
{
    QObject::connect(telnet, &QTelnet::newData,      this, &Telnet::receiveData);
    QObject::connect(telnet, &QTelnet::disconnected, this, &Telnet::sendDisconnect);
}

Telnet::Telnet(QObject *parent) : Telnet("", "", "", "", 23, parent)
{

}

Telnet::~Telnet()
{
    disconnect(telnet, &QTelnet::disconnected, this, &Telnet::sendDisconnect);
    if (telnet->isConnected()) {
        telnet->disconnectFromHost();
    }
    qDebug() << "Destroyed telnet " << hostname();
}

void Telnet::connectToNode()
{
    if (!hostname().isEmpty()) {
        resetState();
        telnet->connectToHost(hostname(), port());
        QTimer::singleShot(5000, this, [this] () {
           if (!isLogged) {
               emit loginState(isLogged);
           }
        });
    } else {
        emit errorOccured("Hostname of node is empty.");
    }
}

bool Telnet::isLoggedInNode() const
{
    return isLogged;
}

void Telnet::executeCommand(const QString &command)
{
    commands.append(command);
    writeIfStateEnabled();
}

void Telnet::sendRelease()
{
    commands.append(disconnectSymbol);
    writeIfStateEnabled();
}

void Telnet::sendConnect()
{
    commands.append(connectSymbol);
    writeIfStateEnabled();
}

QString Telnet::nodeTitle() const
{
    return authData.nodeTitle;
}

void Telnet::setNodeTitle(const QString &title)
{
    authData.nodeTitle = title;
}

QString Telnet::hostname() const
{
    return authData.hostname;
}

void Telnet::setHostname(const QString &host)
{
    authData.hostname = host;
}

QString Telnet::username() const
{
    return authData.username;
}

void Telnet::setUsername(const QString &username)
{
    authData.username = username;
}

QString Telnet::password() const
{
    return authData.password;
}

void Telnet::setPassword(const QString &password)
{
    authData.password = password;
}

uint8_t Telnet::port() const
{
    return authData.port;
}

void Telnet::setPort(uint8_t port)
{
    authData.port = port;
}

QString Telnet::parsedTitle() const
{
    return parsedName;
}

QString Telnet::lastCommand() const
{
    return m_lastCommand;
}


void Telnet::receiveData(const char *data, int length)
{
    QString responce;
    for (int i = 0; i < length; ++i) {
        responce.append(data[i]);
    }

    responce.remove("\u0003");
    responce.remove("\u0004");

    switch (terminalProgressState) {
        case Telnet::EricssonTerminalProgressState::InAuthentication:
            authenticationHandler(responce);
        break;
        case Telnet::EricssonTerminalProgressState::InMML:
            mmlHandler(responce);
        break;
    }
}

void Telnet::authenticationHandler(const QString &responce)
{
    Qt::CaseSensitivity cs = Qt::CaseSensitivity::CaseInsensitive;

    if (responce.contains("login incorrect", cs)) {
        emit loginState(false);
        return;
    } else if (responce.contains("login:", cs) || responce.contains("login name:", cs)) {
        telnet->sendData(authData.username.toLatin1() + '\n');
    } else if (responce.contains("password:", cs)) {
        telnet->sendData(authData.password.toLatin1() + '\n');
    } else if (responce.contains("domain", cs)) {
        telnet->sendData("\n");
    } else if (responce.contains("terminal", cs)) {
        telnet->sendData("xterm\n");
    } else if (responce.contains(">")) {
        telnet->sendData("mml -a\n");
    } else if (responce.contains("WO") || responce.contains("EX-")) {
        const QStringList p = responce.split(' ', Qt::SkipEmptyParts);
        if (p.size() > 1)
            parsedName = p.at(1).split('/').at(0);
        terminalProgressState = EricssonTerminalProgressState::InMML;
        writeIfStateEnabled();
        isLogged = true;
        emit loginState(true);
    }
}

void Telnet::mmlHandler(const QString &responce)
{
    buffer.append(responce);

    static QStringList finishTokens =  {"END\n", "NOT ACCEPTED", "UNRESONABLE VALUE",
                                       "ORDERED", "FUNCTION BUSY", "INHIBITED", "FORMAT ERROR", "TIME OUT"};

    for (const QString &token : finishTokens) {
        if (buffer.contains(token)) {
            token == finishTokens.first() ? emit commandExecuted(buffer.left(buffer.indexOf(token) + token.length())) :
                                            emit errorOccured(buffer.left(buffer.indexOf(token) + token.length()));

            if (!buffer.contains("*** ALARM")) {
                queueState = QueueState::Write;
            }

            buffer = buffer.right(buffer.length() - buffer.indexOf(token) - token.length());

            writeIfStateEnabled();
            break;
        }
    }
}

void Telnet::writeIfStateEnabled()
{
    if (queueState == QueueState::Write &&
            commands.size() && terminalProgressState == EricssonTerminalProgressState::InMML) {
        commands.first() != disconnectSymbol && commands.first() != connectSymbol ?
        telnet->sendData(commands.first().toLatin1() + '\n') : telnet->sendData(commands.first().toLatin1());
        queueState = QueueState::Read;
        m_lastCommand = commands.first();
        commands.removeFirst();
    }
}

void Telnet::resetState()
{
    isLogged = false;
    buffer.clear();
    queueState = QueueState::Write;
    terminalProgressState = EricssonTerminalProgressState::InAuthentication;
    commands.clear();
    if (telnet->isConnected()) {
        telnet->disconnectFromHost();
    }
}

void Telnet::sendDisconnect()
{
    emit errorOccured(QString("disconnected from host %1").arg(this->hostname()));
}
