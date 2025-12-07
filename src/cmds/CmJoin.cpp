// ======================================================================
// Archivo: CmJoin.cpp
// Propósito: Implementación del comando IRC JOIN: valida parámetros y aplica su lógica sobre clientes y canales.
// ======================================================================

#include <Server.hpp>

// JOIN <channel>{,<channel>} [<key>{,<key>}]
void Server::CmJoin(t_msg &msg, int fd) {
	if (msg.params.size() < 1 || msg.params.size() > 2) {
		answerClient(fd, ERR_NEEDMOREPARAMS, "JOIN", "Not enough parameters");
		return ;
	}

	std::vector<std::string> channels;
	std::vector<std::string> passwords;

	splitCmd(msg.params[0], channels, ',');
	if (msg.params.size() == 2)
		splitCmd(msg.params[1], passwords, ',');

	for (size_t i = 0; i < channels.size(); ++i) {
		if ((channels[i][0] != '#' && channels[i][0] != '&') && channels[i] != "0") {
			answerClient(fd, ERR_BADCHANMASK, "", "Channel name must start with #, & or 0");
			return ;
		}
		if (channels[i] == "0") {
			if (channels.size() > 1 || msg.params.size() > 1) {
				answerClient(fd, ERR_NEEDMOREPARAMS, "JOIN", "Channel '0' must be used alone");
				return ;
			}
			std::string partChannels;
			for (std::map<std::string, Channel *>::iterator it = _channel.begin(); it != _channel.end(); ++it) {
				if (it->second->hasUser(fd)) {
					if (!partChannels.empty())
						partChannels += ",";
					partChannels += it->first;
				}
			}
			if (!partChannels.empty()) {
				t_msg partMsg;
				partMsg.command = "PART";
				partMsg.params.push_back(partChannels);
				CmPart(partMsg, fd);
			}
		}
		else {
			if (channels[i].size() < 2) {
				answerClient(fd, ERR_NOSUCHCHANNEL, channels[i], "Channel name is too short");
				continue ;
			}
			if (_clients[fd]->getChannels().size() >= _maxChannelUsers) {
				answerClient(fd, ERR_TOOMANYCHANNELS, channels[i], "You have reached the maximum number of channels");
				continue ;
			}
			if (_channel.find(channels[i]) == _channel.end()) {
				Channel *newChannel = new Channel(channels[i]);
				if (passwords.size() > i && !passwords[i].empty()) {
					newChannel->setPass(passwords[i]);
					newChannel->setMode('k');
				}
				_channel.insert(std::pair<std::string, Channel*>(channels[i], newChannel));
			}
			if (_channel[channels[i]]->hasUser(fd)) {
				answerClient(fd, ERR_ALREADYONCHANNEL, channels[i], "You are already in the channel");
				continue ;
			}
			if (_channel[channels[i]]->hasMode('l') && _channel[channels[i]]->getUserCount() >= _channel[channels[i]]->getMaxUsers()) {
				answerClient(fd, ERR_CHANNELISFULL, channels[i], "Channel is full (+l)");
				continue ;
			}
			if (_channel[channels[i]]->hasMode('k')) {
				if (passwords.size() <= i || passwords[i] != _channel[channels[i]]->getPass()) {
					answerClient(fd, ERR_BADCHANNELKEY, channels[i], "Cannot join channel (+k)");
					continue ;
				}
			}
			if (_channel[channels[i]]->hasMode('i')) {
				if (!_channel[channels[i]]->isInvited(fd)) {
					answerClient(fd, ERR_INVITEONLYCHAN, channels[i], "Cannot join channel (+i)");
					continue ;
				}
				else
					_channel[channels[i]]->removeInvitedList(_clients[fd]);
			}
			if (_channel[channels[i]]->hasMode('b')) {
				if (_channel[channels[i]]->isBanned(fd)) {
					answerClient(fd, ERR_BANNEDFROMCHAN, channels[i], "Cannot join channel (+b)");
					continue ;
				}
			}

			_channel[channels[i]]->newChannelUser(_clients[fd]);
			_channel[channels[i]]->broadcastMessage(fd, "JOIN", "", "");
			sendMsgToClient(fd, "JOIN", channels[i], "");

			t_msg topic;
			topic.command = "TOPIC";
			topic.params.push_back(channels[i]);
			topic.trailing = msg.trailing;
			CmTopic(topic, fd);

			t_msg names;
			names.command = "NAMES";
			names.params.push_back(channels[i]);
			CmNames(names, fd);
		}
	}
}