#include <Server.hpp>


void Server::CmQuit(t_msg& msg, int fdClient)
{
    std::string reason;
    if (!msg.trailing.empty())
        reason = "Quit: " + msg.trailing;
    else
        reason = "Quit: ";

    std::string response = makePrefix(fdClient) + " " + reason + "\r\n";
    if (send(fdClient, response.c_str(), response.size(), MSG_EOR) < 0)
		throw std::runtime_error("Sending msg to client");

	std::vector<Channel*>& channels = _clients[fdClient]->getChannels();

	for (std::vector<Channel*>::iterator it = channels.begin(); it != channels.end(); ++it)
    {
        Channel* ch = *it;
		ch->broadcastMessageNochan(fdClient, "QUIT", reason);
	}
    disconnectClient(fdClient);
}