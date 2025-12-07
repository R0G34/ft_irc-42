// ======================================================================
// Archivo: Client.cpp
// Propósito: Implementación de la clase Client: representa a un usuario conectado y su estado dentro del servidor.
// ======================================================================

#include <Client.hpp>

// Destructor de la clase Client.
Client::~Client() {}

// Método de la clase Client que realiza la operación principal asociada.
Client::Client(const int fd, sockaddr_in *clientAddr) : _clientFd(fd), _clientAddr(clientAddr), _registrationState(RS_NoPass) {
    _ip = inet_ntoa(_clientAddr->sin_addr);
}

// GETTERS

const std::string &Client::getUsername() const { return _userName; }

// Devuelve el atributo correspondiente.
const std::string &Client::getNickname() const { return _nickName; }

// Devuelve el atributo correspondiente.
const std::string &Client::getRealname() const { return _realName; }

// Devuelve el atributo correspondiente.
const RegistrationStatus &Client::getRegistrationState() const { return _registrationState;}

// Devuelve el atributo correspondiente.
std::string &Client::getBufferMsgClient() { return _bufferMsgClient; }

// Devuelve el atributo correspondiente.
std::vector<Channel*>& Client::getChannels() { return _channels; }

// Devuelve el atributo correspondiente.
int Client::getFd() const { return _clientFd; }

// Devuelve el atributo correspondiente.
const std::string Client::GetIp() const { return _ip; }

// SETTERS

void Client::setUsername(const std::string &username) { _userName = username; }

// Actualiza el atributo correspondiente.
void Client::setNickname(const std::string &nickname) { _nickName = nickname; }

// Actualiza el atributo correspondiente.
void Client::setRealname(const std::string &realname) { _realName = realname; }

// Actualiza el atributo correspondiente.
void Client::setRegistrationState(RegistrationStatus st) { _registrationState = st; }

// Actualiza el atributo correspondiente.
void Client::setBufferMsgClient(const std::string &msg) { _bufferMsgClient = msg; }

// Método de la clase void Client que realiza la operación principal asociada.
void Client::joinChannel(Channel* ch) {
    _channels.push_back(ch);
}

// Método de la clase void Client que realiza la operación principal asociada.
void Client::leaveChannel(Channel* ch) {
    _channels.erase(std::remove(_channels.begin(), _channels.end(), ch),_channels.end());
}
