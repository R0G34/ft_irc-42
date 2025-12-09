#include <Client.hpp>


Client::~Client() {}


Client::Client(const int fd, sockaddr_in *clientAddr) : _clientFd(fd), _clientAddr(clientAddr), _registrationState(RS_NoPass) {
    _ip = inet_ntoa(_clientAddr->sin_addr);
}



const std::string &Client::getUsername() const { return _userName; }


const std::string &Client::getNickname() const { return _nickName; }


const std::string &Client::getRealname() const { return _realName; }


const RegistrationStatus &Client::getRegistrationState() const { return _registrationState;}


std::string &Client::getBufferMsgClient() { return _bufferMsgClient; }


std::vector<Channel*>& Client::getChannels() { return _channels; }


int Client::getFd() const { return _clientFd; }


const std::string Client::GetIp() const { return _ip; }



void Client::setUsername(const std::string &username) { _userName = username; }


void Client::setNickname(const std::string &nickname) { _nickName = nickname; }


void Client::setRealname(const std::string &realname) { _realName = realname; }


void Client::setRegistrationState(RegistrationStatus st) { _registrationState = st; }


void Client::setBufferMsgClient(const std::string &msg) { _bufferMsgClient = msg; }


void Client::joinChannel(Channel* ch) {
    _channels.push_back(ch);
}


void Client::leaveChannel(Channel* ch) {
    _channels.erase(std::remove(_channels.begin(), _channels.end(), ch),_channels.end());
}
