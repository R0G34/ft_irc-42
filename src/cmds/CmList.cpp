#include <Server.hpp>



void Server::CmList(t_msg &msg, int fd) {
	answerClient(fd, RPL_LISTSTART, "LIST", "Users  Name");
	if (_channel.empty()) {
		answerClient(fd, RPL_LISTEND, "", "End of /LIST");
		return ;
	}
	std::vector<std::string> channels;
	if (!msg.params.empty())
		splitCmd(msg.params[0], channels, ',');

	for (std::map<std::string, Channel *>::iterator it = _channel.begin(); it != _channel.end(); ++it) {
		if (!channels.empty() && std::find(channels.begin(), channels.end(), it->first) == channels.end())
			continue;
		std::string topic = it->second->getTopic();
		if (topic.empty())
			topic = "No topic";
		std::string userCount = to_string(it->second->getUserCount());
		answerClient(fd, RPL_LIST, it->first + " " + userCount, topic);
	}
	answerClient(fd, RPL_LISTEND, "", "End of /LIST");
}