#include <Server.hpp>


void Server::CmPart(t_msg &msg, int fd) {
	if (msg.params.size() < 1 || msg.params.size() > 2) {
		answerClient(fd, ERR_NEEDMOREPARAMS, "PART", "Not enough parameters");
		return ;
	}

	std::vector<std::string> channels;
	splitCmd(msg.params[0], channels, ',');

	if (!channels.empty()) {
		for (std::vector<std::string>::iterator it = channels.begin(); it != channels.end(); it++) {
			if (_channel.find(*it) == _channel.end()) {
				answerClient(fd, ERR_NOSUCHCHANNEL, "", "No such channel " + *it);
				continue ;
			}
			if (!_channel[*it]->hasUser(fd)) {
				answerClient(fd, ERR_NOTONCHANNEL, "", "You're not on that channel " + *it);
				continue ;
			}
			std::string reason = (!msg.trailing.empty()) ? msg.trailing : _clients[fd]->getNickname() + " has left " + *it;
			sendMsgToClient(fd, "PART", *it, "Leaving... ");
			_channel[*it]->broadcastMessage(fd, "PART", "", reason);
			_channel[*it]->disconnectUser(_clients[fd]);
			if (_channel[*it]->getUserCount() <= 1) {
				if (_channel[*it]->hasUser("Bot"))
					_channel[*it]->disconnectUser(_clients[_channel[*it]->getUserFd("Bot")]);
				delete _channel[*it];
				_channel.erase(*it);
			}
		}
	}
}