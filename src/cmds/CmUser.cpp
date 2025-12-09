#include <Server.hpp>


void	Server::CmUser(t_msg& msg, int fdClient)
{
	if(_clients[fdClient]->getRegistrationState() != RS_NickValidated && 
	   _clients[fdClient]->getRegistrationState() != RS_Registered)
	{
		if(_clients[fdClient]->getRegistrationState() == RS_PassValidated)
			answerClient(fdClient, ERR_ALREADYREGISTERED, "", "After validate your password, you must register your NICK (NICK xxxx).");
		else
			answerClient(fdClient, ERR_ALREADYREGISTERED, "", "First, you must validate password (PASS xxxx). Then register your nick (NICK zzzz).");
		return ;
	}

	if (_clients[fdClient]->getRegistrationState() == RS_Registered)
	{
		answerClient(fdClient, ERR_ALREADYREGISTERED, "", "You may not reregister");
		return ;
	}

	if (msg.params.size() < 3 || msg.hasTrailing == false) 
	{
    	answerClient(fdClient, ERR_NEEDMOREPARAMS, "USER", "Not enough parameters");
    	return;
	}

	if (msg.params[0].length() <= 0)
	{
		answerClient(fdClient, ERR_NEEDMOREPARAMS, "USER", "Not enough parameters");
    	return;
	}





	if ( msg.params[0].length() > USERLEN)
		_clients[fdClient]->setUsername( msg.params[0].substr(0, USERLEN));
	else
		_clients[fdClient]->setUsername( msg.params[0]);
	
	std::map<int, Client*>::iterator	it;
	for (it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (it->first == fdClient)
			continue ;
		else if (it->second->getUsername() == _clients[fdClient]->getUsername())
		{
			answerClient(fdClient, ERR_NICKNAMEINUSE, "", "User is already in use");
			return ;
		}
	}
	
	if (msg.trailing.length() > REALNAMELEN)
		_clients[fdClient]->setRealname(msg.trailing.substr(0, REALNAMELEN));
	else if (msg.trailing.empty())
		_clients[fdClient]->setRealname(_clients[fdClient]->getNickname());
	else
		_clients[fdClient]->setRealname(msg.trailing);

	if (_clients[fdClient]->getRegistrationState() == RS_NickValidated)
		_clients[fdClient]->setRegistrationState(RS_Registered);
}