// ======================================================================
// Archivo: CmNick.cpp
// Propósito: Implementación del comando IRC NICK: valida parámetros y aplica su lógica sobre clientes y canales.
// ======================================================================

#include <Server.hpp>

// Envía datos o mensajes al cliente o al canal correspondiente.
void	Server::sendMsgChangeNick(std::string newNick, int fdClient)
{
	sendMsgToClient(fdClient, "NICK", newNick, "");

	std::vector<Channel*>& channels = _clients[fdClient]->getChannels();

	for (std::vector<Channel*>::iterator it = channels.begin(); it != channels.end(); ++it)
    {
        Channel* ch = *it;
		ch->broadcastMessageNochan(fdClient, "NICK", newNick);
	}
}

// Método de la clase void Server que realiza la operación principal asociada.
void Server::CmNick(t_msg& msg, int fdClient)
{
	if (msg.params.empty() && msg.trailing.size() == 0)
	{
		answerClient(fdClient, ERR_NONICKNAMEGIVEN, "", "No nickname given");
		return ;
	}
	else if (msg.params.empty() && msg.trailing.size() != 0)
		msg.params[0] = msg.trailing;

	if (msg.params[0].size() > NICKLEN)
	{
		answerClient(fdClient, ERR_ERRONEUSNICKNAME, "", "Erroneus nickname");
		return ;
	}

	if (!isSpecial(msg.params[0][0]) && !isalpha(msg.params[0][0]))
	{
		answerClient(fdClient, ERR_ERRONEUSNICKNAME, "", "Erroneus nickname");
		return ;
	}

	for (std::size_t i = 1; i < msg.params[0].size(); i++)
	{
		char c = msg.params[0][i];
		if (!isalpha(c) && !isSpecial(c) && !isdigit(c) && c != '-')
		{
			answerClient(fdClient, ERR_ERRONEUSNICKNAME, "", "Erroneus nickname");
			return ;
		}
	}
	
	std::map<int, Client*>::iterator	it;
	for (it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (it->first == fdClient)
			continue ;
		else if (it->second->getNickname() == msg.params[0])
		{
			answerClient(fdClient, ERR_NICKNAMEINUSE, "", "Nickname is already in use");
			return ;
		}
	}

	if (_clients[fdClient]->getRegistrationState() == RS_PassValidated)
		_clients[fdClient]->setRegistrationState(RS_NickValidated);
	
	if (_clients[fdClient]->getRegistrationState() == RS_Registered)
		sendMsgChangeNick(msg.params[0], fdClient);
		
	_clients[fdClient]->setNickname(msg.params[0]);
}