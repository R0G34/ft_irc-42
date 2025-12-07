// ======================================================================
// Archivo: CmPass.cpp
// Propósito: Implementación del comando IRC PASS: valida parámetros y aplica su lógica sobre clientes y canales.
// ======================================================================

#include <Server.hpp>

// Método de la clase void	Server que realiza la operación principal asociada.
void	Server::CmPass(t_msg& msg, int fdClient)
{
	if (_clients[fdClient]->getRegistrationState() == RS_Registered)
		answerClient(fdClient, ERR_ALREADYREGISTERED, "",  "You are already registered. You don't need register again.");
	else if(_clients[fdClient]->getRegistrationState() == RS_PassValidated )
		answerClient(fdClient, ERR_INSERTNICKAFERPASS, "",  "You have already password validated. You must input your NICK");
	else if(_clients[fdClient]->getRegistrationState() == RS_NickValidated)
		answerClient(fdClient, ERR_INSERTUSERAFTERPASSANDNICK, "",  "You have already password validated and register your nick. You must input your USER");
	else if (msg.params.size() < 1) 
		answerClient(fdClient, ERR_NEEDMOREPARAMS, "", "You must write the password.");
	else if (_password != msg.params[0])
		answerClient(fdClient, ERR_PASSWDMISMATCH, "", "Password incorrect");
	else
		_clients[fdClient]->setRegistrationState(RS_PassValidated);
}