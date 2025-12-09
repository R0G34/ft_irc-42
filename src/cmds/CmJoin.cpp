#include <Server.hpp>


void Server::CmJoin(t_msg &msg, int fd) 
{
	if (msg.params.size() < 1) 
	{
		answerClient(fd, ERR_NEEDMOREPARAMS, "JOIN", "Not enough parameters. You must past 1 or 2 parameters");
		return ;
	}
	if (msg.params.size() > 2) {
		answerClient(fd, ERR_NEEDMOREPARAMS, "JOIN", "Too much parameters. You must past 1 or 2 parameters");
		return ;
	}

	std::vector<std::string> channels;
	std::vector<std::string> passwords;




	splitCmd(msg.params[0], channels, ',');








	if (msg.params.size() == 2)
		splitCmd(msg.params[1], passwords, ',');

	for (size_t i = 0; i < channels.size(); ++i) 
	{

		std::string errMsg = "Error joinning channel named (" + channels[i] + "): ";
		if ((channels[i][0] != '#' && channels[i][0] != '&') && channels[i] != "0") 
		{
			errMsg += "Channel name must start with #, & or 0";
			answerClient(fd, ERR_BADCHANMASK, "", errMsg);
			continue ;
		}
		if (channels[i] == "0") 
		{
			if (channels.size() > 1 || msg.params.size() > 1) 
			{
				answerClient(fd, ERR_NEEDMOREPARAMS, "JOIN", "Channel '0' must be used alone");
				continue ;
			}

			std::string partChannels;
			for (std::map<std::string, Channel *>::iterator it = _channel.begin(); it != _channel.end(); ++it) 
			{
				if (it->second->hasUser(fd)) 
				{
					if (!partChannels.empty())
						partChannels += ",";
					partChannels += it->first;
				}
			}
			if (!partChannels.empty()) 
			{
				t_msg partMsg;
				partMsg.command = "PART";
				partMsg.params.push_back(partChannels);
				CmPart(partMsg, fd);
			}
		}
		else 
		{
			if (channels[i].size() < 2) 
			{
				errMsg += "Channel name is too short";
				answerClient(fd, ERR_NOSUCHCHANNEL, channels[i], errMsg);
				continue ;
			}

			if (_clients[fd]->getChannels().size() >= _maxChannelUsers) 
			{
				errMsg += "Channel has reached the maximum number of channels";
				answerClient(fd, ERR_TOOMANYCHANNELS, channels[i], errMsg);
				continue ;
			}

			if (_channel.find(channels[i]) == _channel.end()) 
			{
				Channel *newChannel = new Channel(channels[i]);
				if (passwords.size() > i && !passwords[i].empty()) 
				{
					newChannel->setPass(passwords[i]);
					newChannel->setMode('k');
				}
				_channel.insert(std::pair<std::string, Channel*>(channels[i], newChannel));
			}

			if (_channel[channels[i]]->hasUser(fd)) 
			{
				errMsg += "You are already in the channel";
				answerClient(fd, ERR_ALREADYONCHANNEL, channels[i], errMsg);
				continue ;
			}

			if (_channel[channels[i]]->hasMode('b')) 
			{
				if (_channel[channels[i]]->isBanned(fd)) 
				{
					errMsg += "You are banned by admin. Cannot join to channel (+b)";
					answerClient(fd, ERR_BANNEDFROMCHAN, channels[i], errMsg);
					continue ;
				}
			}

			if (_channel[channels[i]]->hasMode('i')) 
			{
				if (!_channel[channels[i]]->isInvited(fd)) 
				{
					errMsg += "Cannot join channel (+i)";
					answerClient(fd, ERR_INVITEONLYCHAN, channels[i], errMsg);
					continue ;
				}
				else
					_channel[channels[i]]->removeInvitedList(_clients[fd]);
			}

			if (_channel[channels[i]]->hasMode('l') && _channel[channels[i]]->getUserCount() >= _channel[channels[i]]->getMaxUsers()) 
			{
				errMsg += "Channel is full (+l)";
				answerClient(fd, ERR_CHANNELISFULL, channels[i], errMsg);
				continue ;
			}

			if (_channel[channels[i]]->hasMode('k')) 
			{
				if (passwords.size() <= i ) 
				{
					errMsg += "Cannot join channel (+k). You need password to join.";
					answerClient(fd, ERR_BADCHANNELKEY, channels[i], errMsg);
					continue ;
				}
				if (passwords[i] != _channel[channels[i]]->getPass()) 
				{
					errMsg += "Wrong password (" + passwords[i] + "). Cannot join channel (+k)";
					answerClient(fd, ERR_BADCHANNELKEY, channels[i], errMsg);
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