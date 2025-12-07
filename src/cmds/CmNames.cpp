// ======================================================================
// Archivo: CmNames.cpp
// Propósito: Implementación del comando IRC NAMES: valida parámetros y aplica su lógica sobre clientes y canales.
// ======================================================================

#include <Server.hpp>

// NAMES <channel>{,<channel>}
void Server::CmNames(t_msg &msg, int fd) {
	if (msg.params.size() > 1) {
		answerClient(fd, ERR_NEEDMOREPARAMS, "NAMES", "Not enough parameters");
		return ;
	}
	if (msg.params.empty()) {
		for (std::map<std::string, Channel *>::iterator it = _channel.begin(); it != _channel.end(); ++it) {
			std::string target;
			if (it->second->hasMode('s') || it->second->hasMode('p')) {
				if (!it->second->hasUser(fd))
					continue ;
				if (it->second->hasMode('s'))
					target = "@ " + it->first;
				else
					target = "* " + it->first;
			}
			else 
				target = "= " + it->first;
			std::string response = it->second->listUsers();
			answerClient(fd, RPL_NAMREPLY, target, response);
			answerClient(fd, RPL_ENDOFNAMES, target, "End of /NAMES list");
		}
	}
	else {
		std::vector<std::string> channels;
		splitCmd(msg.params[0], channels, ',');

		for (std::vector<std::string>::iterator it = channels.begin(); it != channels.end(); ++it) {
			std::string target;
			if (_channel.find(*it) == _channel.end())
				continue ;
			if (_channel[*it]->hasMode('s') || _channel[*it]->hasMode('p')) {
				if (!_channel[*it]->hasUser(fd))
					continue ;
				if (_channel[*it]->hasMode('s'))
					target = "@ " + *it;
				else
					target = "* " + *it;
			}
			else
				target = "= " + *it;
			std::string response = _channel[*it]->listUsers();
			answerClient(fd, RPL_NAMREPLY, target, response);
			answerClient(fd, RPL_ENDOFNAMES, *it, "End of /NAMES list");
		}
	}
}