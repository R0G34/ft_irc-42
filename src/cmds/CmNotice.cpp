// ======================================================================
// Archivo: CmNotice.cpp
// Propósito: Implementación del comando IRC NOTICE: valida parámetros y aplica su lógica sobre clientes y canales.
// ======================================================================

# include <Server.hpp>

// Método de la clase void Server que realiza la operación principal asociada.
void Server::CmNotice(t_msg &msg, int fd) {
	if (msg.params.empty())
        return;
    if (!msg.hasTrailing || msg.trailing.empty())
        return;

    std::vector<std::string> targets;
    splitCmd(msg.params[0], targets, ',');
    if (targets.size() > MAX_TARGETS)
        return;

    for (std::vector<std::string>::iterator it = targets.begin(); it != targets.end(); ++it) {
        std::string target = *it;

        if (target[0] == '#' || target[0] == '&') {
            std::map<std::string, Channel*>::iterator chit = _channel.find(target);

            if (chit == _channel.end())
                continue ;
            else {
                Channel* ch = chit->second;
                if (ch->getUserFd(_clients[fd]->getNickname()) == -1)
                    continue ;
                else if ( ch->isBanned(fd))
                    continue ;
                else 
                    ch->broadcastMessage(fd, "NOTICE", "", msg.trailing);
            }
        }
        else {
            int foundFd = -1;
            for (std::map<int, Client*>::iterator cit = _clients.begin(); cit != _clients.end(); ++cit) {
                if (cit->second->getNickname() == target) {
                    foundFd = cit->first;
                    break;
                }
            }
            if (foundFd == -1)
                continue ;
            else {
                msgClientToClient(fd, foundFd, "NOTICE", msg.trailing);
                continue;
            }
        }
    }	
}