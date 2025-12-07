// ======================================================================
// Archivo: CmPrivmsg.cpp
// Propósito: Implementación del comando IRC PRIVMSG: valida parámetros y aplica su lógica sobre clientes y canales.
// ======================================================================

#include <Server.hpp>

// Método de la clase void Server que realiza la operación principal asociada.
void Server::CmPrivMsg(t_msg &msg, int fdClient)
{
    if (msg.params.empty()) {
        answerClient(fdClient, ERR_NORECIPIENT, "", "No recipient given");
        return;
    }
    if (!msg.hasTrailing || msg.trailing.empty()) {
        answerClient(fdClient, ERR_NOTEXTTOSEND, msg.params[0], "No text to send");
        return;
    }

    std::vector<std::string> targets;
    splitCmd(msg.params[0], targets, ',');
    if (targets.size() > MAX_TARGETS) {
        answerClient(fdClient, ERR_TOOMANYTARGETS, "", "Too many targets");
        return;
    }

    for (std::vector<std::string>::iterator it = targets.begin(); it != targets.end(); ++it)
    {
        std::string target = *it;

        if (target[0] == '#' || target[0] == '&')
        {
            std::map<std::string, Channel*>::iterator chit = _channel.find(target);

            if (chit == _channel.end())
            {
                answerClient(fdClient, ERR_NOSUCHCHANNEL, target, "No such channel");
                continue ;
            }
            else
            {
                Channel* ch = chit->second;
                if (ch->getUserFd(_clients[fdClient]->getNickname()) == -1)
                {
                    answerClient(fdClient, ERR_NOTONCHANNEL, target, "Cannot send to channel");
                    continue ;
                }
                else if ( ch->isBanned(fdClient))
                {
                    answerClient(fdClient, ERR_CANNOTSENDTOCHAN, target, "Cannot send to channel");
                    continue ;
                }
                else 
                    ch->broadcastMessage(fdClient, "PRIVMSG", "", msg.trailing);
            }
        }
        else
        {
            int foundFd = -1;
            for (std::map<int, Client*>::iterator cit = _clients.begin(); cit != _clients.end(); ++cit)
            {
                if (cit->second->getNickname() == target) 
                {
                    foundFd = cit->first;
                    break;
                }
            }
            if (foundFd == -1)
            {
                answerClient(fdClient, ERR_NOSUCHNICK, target, "No such nick");
                continue ;
            }
            else
            {
                msgClientToClient(fdClient, foundFd, "PRIVMSG", msg.trailing);
                continue;
            }
        }
    }
}

