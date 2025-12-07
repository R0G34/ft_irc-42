// ======================================================================
// Archivo: CmUser.cpp
// Propósito: Implementación del comando IRC USER: valida parámetros y aplica su lógica sobre clientes y canales.
// ======================================================================

#include <Server.hpp>

// Método de la clase void	Server que realiza la operación principal asociada.
void	Server::CmUser(t_msg& msg, int fdClient)
{
	if (_clients[fdClient]->getRegistrationState() == RS_Registered)
	{
		answerClient(fdClient, ERR_ALREADYREGISTERED, "", "You may not reregister");
		return ;
	}

	if (msg.params.size() < 3 || msg.hasTrailing == false) 
	{
    	answerClient(fdClient, ERR_NEEDMOREPARAMS, "USER", "Not enough parameters"); //ok 
    	return;
	}

	if (msg.params[0].length() <= 0)
	{
		answerClient(fdClient, ERR_NEEDMOREPARAMS, "USER", "Not enough parameters"); //ok 
    	return;
	}

	// TODO COMENTARIO NECESARIO NO BORRAR poner en ingles
	// RFC 2812 establece que params[1] y params[2] “SHOULD” ser “0” y “*”
	// pero no es obligatorio, así que los ignoramos intencionadamente.

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
			answerClient(fdClient, ERR_NICKNAMEINUSE, "", "User is already in use"); // TODO:  que error mandar aqui?
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