// ======================================================================
// Archivo: CmInvite.cpp
// Propósito: Implementación del comando IRC INVITE: valida parámetros y aplica su lógica sobre clientes y canales.
// ======================================================================

#include <Server.hpp>

// INVITE <nickname> <channel>
void Server::CmInvite(t_msg &msg, int fd) 
{
	if (msg.params.size() != 2 || (msg.params[0][0] == '#' || msg.params[0][0] == '&') || 
								  (msg.params[1][0] != '#' && msg.params[1][0] != '&')) 
	{
		answerClient(fd, ERR_NEEDMOREPARAMS, "INVITE", "Error INVITE command format. (It must be like \"INVITE NICKNAME #channel\")");
		return ;
	}

	std::string nickToInvite = msg.params[0];
	std::string channel = msg.params[1];
	int invited = -1;
	for (std::map<int, Client *>::iterator it = _clients.begin(); it != _clients.end(); ++it) 
	{
		if (it->second->getNickname() == nickToInvite) 
		{
			invited = it->first;
			break;
		}
	}

	if (invited == -1) 
	{
		answerClient(fd, ERR_NOSUCHNICK, nickToInvite, "Error, the nick \"" + nickToInvite + "\", doesn't exist on the server.");
		return ;
	}
	if (_channel.find(channel) == _channel.end()) 
	{
		answerClient(fd, ERR_NOTONCHANNEL, channel, "Error, the channel \"" + channel + "\", doesn't exist on the server.");
		return ;
	}

	if (!_channel[channel]->hasUser(_clients[fd]->getNickname())) 
	{
		answerClient(fd, ERR_NOTONCHANNEL, channel, "Error, you're not on the \"" + channel + "\" channel. So you can't invite any one to the channel");
		return ;
	}

	if (_channel[channel]->hasMode('i') && !_channel[channel]->isAdmin(fd)) 
	{
		answerClient(fd, ERR_INVITEONLYCHAN, channel, "Error, you need to be administrator of the \"" + channel + "\" channel to invite someone.");
		return ;
	}

	if (_channel[channel]->hasUser(nickToInvite)) 
	{
		answerClient(fd, ERR_USERONCHANNEL, nickToInvite, "\"" + nickToInvite + "\" is already on \"" + channel + "\" channel.");
		return ;
	}
	// if (_channel[channel]->hasMode('i') && !_channel[channel]->isAdmin(fd)) {
	// 	answerClient(fd, ERR_INVITEONLYCHAN, channel, "Cannot invite to invite-only channel");
	// 	return ;
	// }
	if (_channel[channel]->hasMode('l') && _channel[channel]->getUserCount() >= _channel[channel]->getMaxUsers()) {
		answerClient(fd, ERR_CHANNELISFULL, channel, "The channel \"" + channel + "\" is full. You must to increse de limit of users of the channel if you want to invite someone");
		return ;
	}

	if (_channel[channel]->hasMode('b') && _channel[channel]->isBanned(invited)) 
	{
		answerClient(fd, ERR_BANNEDFROMCHAN, channel, "Cannot invite banned user");
		return ;
	}

	//DMK 
	Client* cli = _clients[invited];
	if (!_channel[channel]->isInvited(invited))
	{	
		_channel[channel]->addInvitedList(_clients[invited]);
		msgClientToClient(fd, invited, "INVITE", channel);
		answerClient(fd, RPL_INVITING, _clients[invited]->getNickname(), channel);
	}
	else
	{
		answerClient(fd, RPL_INVITING, cli->getNickname(), "Warning, \"" + cli->getNickname() + "\" is  already invited.");
	}
}
	
