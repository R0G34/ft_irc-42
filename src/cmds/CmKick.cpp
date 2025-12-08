// ======================================================================
// Archivo: CmKick.cpp
// Propósito: Implementación del comando IRC KICK: valida parámetros y aplica su lógica sobre clientes y canales.
// ======================================================================

#include <Server.hpp>

// KICK <channel> <user> *( "," <user> ) [<comment>]
void Server::CmKick(t_msg &msg, int fd) 
{
	if (msg.params.size() != 2 || (msg.params[0][0] != '#' && msg.params[0][0] != '&') || 
								  (msg.params[1][0] == '#' || msg.params[1][0] == '&'))
	{
		answerClient(fd, ERR_NEEDMOREPARAMS, "KICK", "Error KICK command format. It must be like \"KICK #channel NICK(,NICK2...)");
		return ;
	}

	std::string channel = msg.params[0];
	std::string nickToKick = msg.params[1];
	if (_channel.find(channel) == _channel.end()) 
	{
		answerClient(fd, ERR_NOSUCHCHANNEL, channel, "Error, the \"" + channel + "\" channel , doesn't exist on the server.");
		return ;
	}

	if (!_channel[channel]->hasUser(fd)) 
	{
		answerClient(fd, ERR_NOTONCHANNEL, channel, "Error, you're not on \"" + channel + "\" channel");
		return ;
	}
	if (!_channel[channel]->isAdmin(fd)) 
	{
		answerClient(fd, ERR_CHANOPRIVSNEEDED, channel, "Error, you need to be administrator of the \"" + channel + "\" channel to kick someone.");
		return ;
	}
	if (_channel[channel]->getName() == "#general") 
	{
		answerClient(fd, ERR_CHANOPRIVSNEEDED, channel, "You can't kick users from #general channel");
		return ;
	}
	
	std::vector<std::string> users;
	splitCmd(nickToKick, users, ',');
	for (std::vector<std::string>::iterator it = users.begin(); it != users.end(); ++it) 
	{
		if (!_channel[channel]->hasUser(*it)) 
		{
			answerClient(fd, ERR_USERNOTINCHANNEL, channel, "User " + *it + " is not on that channel");
			continue ;
		}
		int userFd = _channel[channel]->getUserFd(*it);
		if (!userFd)
			continue ;

		std::string reason = _clients[fd]->getNickname() + " kicked " + nickToKick + " from " + channel + " channel";
		if(!msg.trailing.empty())
		{
			reason += " because" + msg.trailing;
		}
		sendMsgToClient(fd, "KICK", channel + " " + *it , reason);
		_channel[channel]->broadcastMessage(fd, "KICK", *it, reason);
		_channel[channel]->disconnectUser(_clients[userFd]);
		if (_channel[channel]->getUserCount() == 0) 
		{
			delete _channel[channel];
			_channel.erase(channel);
		}
	}
}