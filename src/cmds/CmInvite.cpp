// ======================================================================
// Archivo: CmInvite.cpp
// Propósito: Implementación del comando IRC INVITE: valida parámetros y aplica su lógica sobre clientes y canales.
// ======================================================================

#include <Server.hpp>

// INVITE <nickname> <channel>
void Server::CmInvite(t_msg &msg, int fd) {
	if (msg.params.size() != 2) {
		answerClient(fd, ERR_NEEDMOREPARAMS, "INVITE", "Not enough parameters");
		return ;
	}

	int invited = -1;
	for (std::map<int, Client *>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
		if (it->second->getNickname() == msg.params[0]) {
			invited = it->first;
			break;
		}
	}
	if (invited == -1) {
		answerClient(fd, ERR_NOSUCHNICK, msg.params[0], "No such nick/channel");
		return ;
	}
	if (_channel.find(msg.params[1]) == _channel.end()) {
		answerClient(fd, ERR_NOTONCHANNEL, msg.params[1], "You're not on that channel");
		return ;
	}
	if (!_channel[msg.params[1]]->hasUser(_clients[fd]->getNickname())) {
		answerClient(fd, ERR_NOTONCHANNEL, msg.params[1], "You're not on that channel");
		return ;
	}
	if (_channel[msg.params[1]]->hasUser(msg.params[0])) {
		answerClient(fd, ERR_USERONCHANNEL, msg.params[0], "is already on channel");
		return ;
	}
	if (_channel[msg.params[1]]->hasMode('i') && !_channel[msg.params[1]]->isAdmin(fd)) {
		answerClient(fd, ERR_INVITEONLYCHAN, msg.params[1], "Cannot invite to invite-only channel");
		return ;
	}
	if (_channel[msg.params[1]]->hasMode('l') && _channel[msg.params[1]]->getUserCount() >= _channel[msg.params[1]]->getMaxUsers()) {
		answerClient(fd, ERR_CHANNELISFULL, msg.params[1], "Cannot invite to full channel");
		return ;
	}
	if (_channel[msg.params[1]]->hasMode('b') && _channel[msg.params[1]]->isBanned(invited)) {
		answerClient(fd, ERR_BANNEDFROMCHAN, msg.params[1], "Cannot invite banned user");
		return ;
	}
	if (!_channel[msg.params[1]]->isInvited(invited))
		_channel[msg.params[1]]->addInvitedList(_clients[invited]);
	msgClientToClient(fd, invited, "INVITE", msg.params[1]);
	answerClient(fd, RPL_INVITING, _clients[invited]->getNickname(), msg.params[1]);
}
