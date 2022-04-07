#include "telnet.h"
#include <QTimer>


Telnet::Telnet(const QString &nodeTitle, const QString &hostname, const QString &username,
               const QString &password, const uint8_t port, QObject *parent) :
    QObject(parent),
    authData(nodeTitle, hostname, username, password, port),
    telnet(new QTelnet(this))
{

    QObject::connect(telnet, &QTelnet::newData,      this, &Telnet::receiveData);
    QObject::connect(telnet, &QTelnet::stateChanged, this, [this] (QAbstractSocket::SocketState state){
        m_state = state;
    });
    QObject::connect(telnet, &QTelnet::socketError, this, &Telnet::processSocketError);
}

Telnet::Telnet(QObject *parent) : Telnet("", "", "", "", 23, parent)
{

}

Telnet::~Telnet()
{
    if (telnet->isConnected()) {
        telnet->disconnectFromHost();
    }
}

void Telnet::connectToNode()
{
    if (!hostname().isEmpty()) {
        resetState();
        telnet->connectToHost(hostname(), port());
    } else {
        emit errorOccured(tr("Hostname of node is empty."));
    }
}

void Telnet::reconnect()
{
    connectToNode();
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

QAbstractSocket::SocketState Telnet::state() const
{
    return m_state;
}

const QStringList &Telnet::finishTokens()
{
    static QStringList tokens =  {"END\n", "NOT ACCEPTED", "UNRESONABLE VALUE",
                                  "FUNCTION BUSY", "INHIBITED", "FORMAT ERROR", "TIME OUT"};
    return tokens;
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

    if (responce.contains(tr("login incorrect"), cs)) {
        emit loginStateChanged(false);
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
        telnet->sendData("mml\n");
    } else if (responce.contains("WO") || responce.contains("EX-")) {
        const QStringList p = responce.split(' ', Qt::SkipEmptyParts);
        if (p.size() > 1)
            parsedName = p.at(1).split('/').at(0);
        terminalProgressState = EricssonTerminalProgressState::InMML;
        writeIfStateEnabled();
        isLogged = true;
        emit loginStateChanged(isLogged);
    }
}

void Telnet::mmlHandler(const QString &responce)
{
    buffer.append(responce);

    for (const QString &token : finishTokens()) {
        if (buffer.contains(token)) {
            token == finishTokens().first() ? emit commandExecuted(buffer.left(buffer.indexOf(token) + token.length())) :
                                              emit errorOccured(buffer.left(buffer.indexOf(token) + token.length()));
            buffer = buffer.right(buffer.length() - buffer.indexOf(token) - token.length());

            writeIfStateEnabled();
            break;
        }
    }
}

void Telnet::writeIfStateEnabled()
{
    if (!commands.isEmpty() && terminalProgressState == EricssonTerminalProgressState::InMML) {
        if (commands.first() == connectSymbol || commands.first() == disconnectSymbol) {
            telnet->sendData(commands.first().toLatin1());
        } else {
            if (commands.first().length() > 5 && commands.first().at(4) != 'p') {
                commands.first() += "\n;";
            }
            telnet->sendData(commands.first().toLatin1() + '\n');
        }
        m_lastCommand = commands.first();
        commands.removeFirst();
    }
}

void Telnet::resetState()
{
    isLogged = false;
    buffer.clear();
    terminalProgressState = EricssonTerminalProgressState::InAuthentication;
    commands.clear();
    if (telnet->isConnected()) {
        telnet->disconnectFromHost();
    }
}
void Telnet::processSocketError(QAbstractSocket::SocketError error)
{
    QString errorMessage;
    switch (error) {

    case QAbstractSocket::SocketError::AddressInUseError :
    {
        errorMessage = tr("The address specified to QAbstractSocket::bind() is already in use and was set to be exclusive.");
        break;
    }
    case QAbstractSocket::SocketError::ConnectionRefusedError :
    {
        errorMessage = tr("The connection was refused by the peer (or timed out).");
        break;
    }
    case QAbstractSocket::SocketError::DatagramTooLargeError :
    {
        errorMessage = tr("The datagram was larger than the operating system's limit (which can be as low as 8192 bytes).");
        break;
    }
    case QAbstractSocket::SocketError::HostNotFoundError :
    {
        errorMessage = tr("The host address was not found.");
        break;
    }
    case QAbstractSocket::SocketError::NetworkError :
    {
        errorMessage = tr("An error occurred with the network (e.g., the network cable was accidentally plugged out).");
        break;
    }
    case QAbstractSocket::SocketError::OperationError :
    {
        errorMessage = tr("An operation was attempted while the socket was in a state that did not permit it.");
        break;
    }
    case QAbstractSocket::SocketError::ProxyAuthenticationRequiredError :
    {
        errorMessage = tr("The socket is using a proxy, and the proxy requires authentication.");
        break;
    }
    case QAbstractSocket::SocketError::ProxyConnectionClosedError :
    {
        errorMessage = tr("The connection to the proxy server was closed unexpectedly (before the connection to the final peer was established).");
        break;
    }
    case QAbstractSocket::SocketError::ProxyConnectionRefusedError:
    {
        errorMessage = tr("Could not contact the proxy server because the connection to that server was denied.");
        break;
    }
    case QAbstractSocket::SocketError::ProxyConnectionTimeoutError :
    {
        errorMessage = tr("The connection to the proxy server timed out or the proxy server stopped responding in the authentication phase.");
        break;
    }
    case QAbstractSocket::SocketError::ProxyNotFoundError :
    {
        errorMessage = tr("The proxy address set with setProxy() (or the application proxy) was not found.");
        break;
    }
    case QAbstractSocket::SocketError::ProxyProtocolError :
    {
        errorMessage = tr("The connection negotiation with the proxy server failed, because the response from the proxy server could not be understood.");
        break;
    }
    case QAbstractSocket::SocketError::RemoteHostClosedError :
    {
        errorMessage = tr("The remote host closed the connection.");
        break;
    }
    case QAbstractSocket::SocketError::SocketAccessError :
    {
        errorMessage = tr("The socket operation failed because the application lacked the required privileges.");
        break;
    }
    case QAbstractSocket::SocketError::SocketAddressNotAvailableError :
    {
        errorMessage = tr("The address specified to QAbstractSocket::bind() does not belong to the host.");
        break;
    }
    case QAbstractSocket::SocketError::SocketResourceError :
    {
        errorMessage = tr("The local system ran out of resources (e.g., too many sockets).");
        break;
    }
    case QAbstractSocket::SocketError::SocketTimeoutError :
    {
        errorMessage = tr("The socket operation timed out.");
        break;
    }
    case QAbstractSocket::SocketError::TemporaryError :
    {
        errorMessage = tr("A temporary error occurred (e.g., operation would block and socket is non-blocking).");
        break;
    }
    case QAbstractSocket::SocketError::UnknownSocketError :
    {
        errorMessage = tr("An unidentified error occurred.");
        break;
    }
    case QAbstractSocket::SocketError::UnsupportedSocketOperationError :
    {
        errorMessage = tr("The requested socket operation is not supported by the local operating system (e.g., lack of IPv6 support).");
        break;
    }

    }// end switch
    emit errorOccured(errorMessage);
}
