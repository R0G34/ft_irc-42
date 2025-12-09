#include <Server.hpp>



void Server::CmTopic(t_msg &msg, int fd) {
	if (msg.params.size() < 1 || msg.params.size() > 2) {
		answerClient(fd, ERR_NEEDMOREPARAMS, "TOPIC", "Not enough parameters");
		return ;
	}
	if (_channel.find(msg.params[0]) == _channel.end()) {
		answerClient(fd, ERR_NOSUCHCHANNEL, msg.params[0], "No such channel");
		return ;
	}
	if (!_channel[msg.params[0]]->hasUser(fd)) {
		answerClient(fd, ERR_NOTONCHANNEL, msg.params[0], "You're not on that channel");
		return ;
	}

	if (!msg.trailing.empty()) {
		if (_channel[msg.params[0]]->hasMode('t') && !_channel[msg.params[0]]->isAdmin(fd)) {
			answerClient(fd, ERR_CHANOPRIVSNEEDED, msg.params[0], "You're not channel operator");
			return ;
		}
		_channel[msg.params[0]]->setTopic(msg.trailing);
		_channel[msg.params[0]]->broadcastMessage(fd, "TOPIC", "", msg.trailing);
		sendMsgToClient(fd, "TOPIC", msg.params[0], msg.trailing);
	}
	else {
		if (_channel[msg.params[0]]->getTopic().empty())
			answerClient(fd, RPL_NOTOPIC, msg.params[0], "No topic set");
		else
			answerClient(fd, RPL_TOPIC,  msg.params[0], _channel[msg.params[0]]->getTopic());
	}
}