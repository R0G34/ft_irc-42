// ======================================================================
// Archivo: Channel.cpp
// Propósito: Implementación de la clase Channel: gestión de usuarios, modos y mensajes de un canal IRC.
// ======================================================================

#include <Channel.hpp>

// Destructor de la clase Channel.
Channel::~Channel() {}

// Método de la clase Channel(const std que realiza la operación principal asociada.
Channel::Channel(const std::string &name) : _maxUsers(0), _name(name), _ownerFd(-1) {}

// GETTERS

const std::string &Channel::getName() const { return _name; }

// Devuelve el atributo correspondiente.
const std::string &Channel::getPass() const { return _pass; }

// Devuelve el atributo correspondiente.
const size_t &Channel::getMaxUsers() const { return _maxUsers; }

// Devuelve el atributo correspondiente.
const std::string &Channel::getTopic() const { return _topic; }

// Devuelve el atributo correspondiente.
size_t Channel::getUserCount() const { return _users.size(); }

// Devuelve el atributo correspondiente.
int Channel::getUserFd(const std::string &nick) const {
	for (std::map<int, Client *>::const_iterator it = _users.begin(); it != _users.end(); ++it) {
		if (it->second->getNickname() == nick)
			return it->first;
	}
	return -1;
}

// Devuelve el atributo correspondiente.
std::string Channel::getMode() const {
	std::string modes = "+";
	for (std::set<char>::const_iterator it = _mode.begin(); it != _mode.end(); ++it) {
		modes += *it;
	}
	return modes;
}

// SETTERS

void Channel::setName(const std::string &name) { _name = name; }

// Actualiza el atributo correspondiente.
void Channel::setPass(const std::string &pass) { _pass = pass; }

// Actualiza el atributo correspondiente.
void Channel::setMaxUsers(const size_t &maxUsers) { _maxUsers = maxUsers; }

// Actualiza el atributo correspondiente.
void Channel::setMode(const char &mode) { _mode.insert(mode); }

// Método de la clase void Channel que realiza la operación principal asociada.
void Channel::unsetMode(const char &mode) { _mode.erase(mode); }

// Actualiza el atributo correspondiente.
void Channel::setTopic(const std::string &topic) { _topic = topic; }

// CHECKS

bool Channel::isBanned(int fd) const { return _banned.find(fd) != _banned.end(); }

// Comprueba o valida una condición concreta.
bool Channel::isInvited(int fd) const { return _invited.find(fd) != _invited.end(); }

// Comprueba o valida una condición concreta.
bool Channel::isAdmin(int fd) const { return _admins.find(fd) != _admins.end(); }

// Método de la clase bool Channel que realiza la operación principal asociada.
bool Channel::hasMode(const char &mode) const { return _mode.find(mode) != _mode.end(); }

// Método de la clase bool Channel que realiza la operación principal asociada.
bool Channel::hasUser(int fd) const { return _users.find(fd) != _users.end(); }

// Método de la clase bool Channel que realiza la operación principal asociada.
bool Channel::hasUser(const std::string &nick) const {
	for (std::map<int, Client *>::const_iterator it = _users.begin(); it != _users.end(); ++it) {
		if (it->second->getNickname() == nick)
			return true;
	}
	return false;
}

// METHODS

void Channel::newChannelUser(Client *client) {
	if (_users.size() >= _maxUsers && _maxUsers > 0)
		throw std::runtime_error("Channel is full.");
	if (_users.find(client->getFd()) != _users.end())
		throw std::runtime_error("User already connected to this channel.");
	if (_banned.find(client->getFd()) != _banned.end())
		throw std::runtime_error("User is banned from this channel.");

	if (_ownerFd == -1) {
		_ownerFd = client->getFd();
		_admins.insert(std::make_pair(client->getFd(), client));
	}

	_users.insert(std::make_pair(client->getFd(), client));
	client->joinChannel(this);
}

// Método de la clase void Channel que realiza la operación principal asociada.
void Channel::disconnectUser(Client *client) {
	if (_users.find(client->getFd()) == _users.end())
		return ;

	if (_users.find(client->getFd()) != _users.end()) {
		if (_ownerFd == client->getFd()) {
			if (_admins.size() > 1) {
				removeAdminList(client);
				if (_admins.begin()->second->getNickname() != "Bot")
					_ownerFd = _admins.begin()->first;
			}
			else
			_ownerFd = -1;
		}
	}
	if (_ownerFd == -1 && _users.size() > 1) {
		for (std::map<int, Client *>::iterator it = _users.begin(); it != _users.end(); ++it) {
			if (it->second->getFd() != client->getFd()) {
				if (it->second->getNickname() != "Bot") {
					_ownerFd = it->first;
					break;
				}
			}
		}
		if (_ownerFd != -1) {
			_admins.insert(std::make_pair(_ownerFd, _users[_ownerFd]));
			std::string reponse = ":" + _users[_ownerFd]->getNickname() + "!" + _users[_ownerFd]->getUsername() + "@" + _users[_ownerFd]->GetIp();
			reponse += " MODE " + _name + " +o " + _users[_ownerFd]->getNickname() + "\r\n";
			if (send(_ownerFd, reponse.c_str(), reponse.size(), MSG_EOR) < 0)
				throw std::runtime_error("Sending msg to client");
			broadcastMessage(client->getFd(), "MODE", _users[_ownerFd]->getNickname(), "+o " + _users[_ownerFd]->getNickname());
		}
	}
	if (isAdmin(client->getFd()))
		removeAdminList(client);
	if (isInvited(client->getFd()))
		removeInvitedList(client);
	client->leaveChannel(this);
	_users.erase(client->getFd());
}

// Añade elementos o información a las estructuras internas.
void Channel::addInvitedList(Client *client) {
	if (_invited.find(client->getFd()) != _invited.end())
		throw std::runtime_error("User already invited to this channel.");
	_invited.insert(std::make_pair(client->getFd(), client));
}

// Elimina elementos o información de las estructuras internas.
void Channel::removeInvitedList(Client *client) {
	if (_invited.find(client->getFd()) == _invited.end())
		throw std::runtime_error("User not invited to this channel.");
	_invited.erase(client->getFd());
}

// Añade elementos o información a las estructuras internas.
void Channel::addBannedList(Client *client) {
	if (_banned.find(client->getFd()) != _banned.end())
		throw std::runtime_error("User already banned from this channel.");
	_banned.insert(std::make_pair(client->getFd(), client));
}

// Elimina elementos o información de las estructuras internas.
void Channel::removeBannedList(Client *client) {
	if (_banned.find(client->getFd()) == _banned.end())
		throw std::runtime_error("User unbanned from this channel.");
	_banned.erase(client->getFd());
}

// Añade elementos o información a las estructuras internas.
void Channel::addAdminList(Client *client) {
	if (_admins.find(client->getFd()) != _admins.end())
		throw std::runtime_error("User already admin of this channel.");
	_admins.insert(std::make_pair(client->getFd(), client));
}

// Elimina elementos o información de las estructuras internas.
void Channel::removeAdminList(Client *client) {
	if (_admins.find(client->getFd()) == _admins.end())
		throw std::runtime_error("User is not admin of this channel.");
	_admins.erase(client->getFd());
	_invited.erase(client->getFd());
}

// Envía un mensaje a todos los clientes afectados.
void Channel::broadcastMessageNochan(int fd, const std::string &cmd, const std::string &msg) const {
	std::string prefix = ":" + _users.at(fd)->getNickname() + "!" + _users.at(fd)->getUsername() + "@" + _users.at(fd)->GetIp();
	std::string message = prefix + " " + cmd;
	
	if (!msg.empty())
		message += " :" + msg;
	message += "\r\n";
	for (std::map<int, Client *>::const_iterator it = _users.begin(); it != _users.end(); ++it) {
		if (it->first != fd)
			if (send(it->first, message.c_str(), message.size(), MSG_EOR) < 0)
				throw std::runtime_error("Sending msg to client");
	}
}

// Envía un mensaje a todos los clientes afectados.
void Channel::broadcastMessage(int fd, const std::string &cmd, const std::string &user, const std::string &msg) const {
	std::string prefix = ":" + _users.at(fd)->getNickname() + "!" + _users.at(fd)->getUsername() + "@" + _users.at(fd)->GetIp();
	std::string message = prefix + " " + cmd + " " + _name;
	
	if (!user.empty())
		message += " " + user;
	if (!msg.empty())
		message += " :" + msg;
	message += "\r\n";
	for (std::map<int, Client *>::const_iterator it = _users.begin(); it != _users.end(); ++it) {
		if (it->first != fd)
			if (send(it->first, message.c_str(), message.size(), MSG_EOR) < 0)
				throw std::runtime_error("Sending msg to client");
	}
}

// Devuelve una representación en texto de los elementos correspondientes.
std::string Channel::listUsers() {
	std::string list;
	if (_users.empty())
		return list;
	for (std::map<int, Client *>::iterator it = _users.begin(); it != _users.end(); ++it) {
		if (!list.empty())
			list += " ";
		if (isAdmin(it->first))
			list += "@";
		list += it->second->getNickname();
	}
	return list;
}

// Devuelve una representación en texto de los elementos correspondientes.
std::string Channel::listBanned() {
	std::string list;
	if (_banned.empty())
		return list;
	for (std::map<int, Client *>::iterator it = _banned.begin(); it != _banned.end(); ++it) {
		if (!list.empty())
			list += " ";
		list += it->second->getNickname();
	}
	return list;
}
