#include <Server.hpp>
#include <iostream>
#include <sstream>
#include <string>


void	Server::initCmds()
{
	_fCommands.insert(std::pair<std::string, FCmd>("PASS", &Server::CmPass));
	_fCommands.insert(std::pair<std::string, FCmd>("NICK", &Server::CmNick));
	_fCommands.insert(std::pair<std::string, FCmd>("USER", &Server::CmUser));
	_fCommands.insert(std::pair<std::string, FCmd>("CAP", &Server::CmCAP));
	_fCommands.insert(std::pair<std::string, FCmd>("JOIN", &Server::CmJoin));
	_fCommands.insert(std::pair<std::string, FCmd>("LIST", &Server::CmList));
	_fCommands.insert(std::pair<std::string, FCmd>("PART", &Server::CmPart));
	_fCommands.insert(std::pair<std::string, FCmd>("NAMES", &Server::CmNames));
	_fCommands.insert(std::pair<std::string, FCmd>("TOPIC", &Server::CmTopic));
	_fCommands.insert(std::pair<std::string, FCmd>("KICK", &Server::CmKick));
	_fCommands.insert(std::pair<std::string, FCmd>("INVITE", &Server::CmInvite));
	_fCommands.insert(std::pair<std::string, FCmd>("MODE", &Server::CmMode));
	_fCommands.insert(std::pair<std::string, FCmd>("PRIVMSG", &Server::CmPrivMsg));
	_fCommands.insert(std::pair<std::string, FCmd>("WHO", &Server::CmWho));
	_fCommands.insert(std::pair<std::string, FCmd>("QUIT", &Server::CmQuit));
	_fCommands.insert(std::pair<std::string, FCmd>("NOTICE", &Server::CmNotice));
}


std::string Server::makePrefix(int fd) 
{
  return ":" + _clients[fd]->getNickname()
       + "!" + _clients[fd]->getUsername()
       + _clients[fd]->GetIp();
}


void Server::answerClient(int fdClient, int code, const std::string &target, const std::string &msg)
{
	std::ostringstream ss;
	ss << std::setfill('0') << std::setw(3) << code;
	std::string codeStr = ss.str();

	std::string nickname = _clients[fdClient]->getNickname();
	if (nickname.empty())
		nickname = "unknown";
	std::string response = ":" + _serverName + " " + codeStr + " " + nickname;
	
	if (!target.empty())
		response += " " + target;
	response += " :" + msg + "\r\n";

	std::cout << GREEN << response << CLEAR << std::endl;
	if (send(fdClient, response.c_str(), response.size(), MSG_EOR) < 0)
		throw std::runtime_error("Sending msg to client");
}




void Server::sendMsgToClient(int fd, const std::string &cmd, const std::string &channel, const std::string &msg) 
{
	std::string prefix = makePrefix(fd);
	std::string response = prefix + " " + cmd + " " + channel;
	
	if (!msg.empty())
		response += " :" + msg;
	response += "\r\n";
	if (send(fd, response.c_str(), response.size(), MSG_EOR) < 0)
		throw std::runtime_error("Sending msg to client");
}


void Server::msgClientToClient(int from, int to, const std::string &cmd, const std::string &msg) 
{
	std::string prefix = makePrefix(from);
	std::string response = prefix + " " + cmd + " " + _clients[to]->getNickname();
	
	if (!msg.empty())
		response += " :" + msg;
	response += "\r\n";
	if (send(to, response.c_str(), response.size(), MSG_EOR) < 0)
		throw std::runtime_error("Sending msg to client");
}


void	Server::sendWelcomeMsg(int fdClient)
{
	Client& c = *_clients[fdClient];

	answerClient(fdClient, RPL_WELCOME, "", "Welcome to the IRC network, " + c.getNickname());
	
    answerClient(fdClient, RPL_YOURHOST, "", "Your host is " + _serverName + ", running version " + _version);

    answerClient(fdClient, RPL_CREATED, "", "This server was created " + _creationDate);

    answerClient(fdClient, RPL_MYINFO, "", _serverName + " " + _version + " " + _chanModes);
}


void	Server::joinGeneralChannel(int fdClient)
{
	if (_channel.find("#general") == _channel.end()) 
	{
        Channel *newChannel = new Channel("#general");
        _channel.insert(std::pair<std::string, Channel*>("#general", newChannel));
    }
    t_msg join;
    join.command = "JOIN";
    join.params.push_back("#general");
    CmJoin(join, fdClient);
}


void Server::handleCommand(t_msg& msg, int fdClient)
{
	if (_clients[fdClient]->getRegistrationState() != RS_Registered)
	{
		if (msg.command == "CAP") 
		{
			CmCAP(msg, fdClient);
		}		
		else if(msg.command == "PASS")
		{
			CmPass(msg, fdClient);
		}
		else if(msg.command == "NICK")
		{
			CmNick(msg, fdClient); 	
		}
		else if(msg.command == "USER")
		{
			CmUser(msg, fdClient);
			if (_clients[fdClient]->getRegistrationState() == RS_Registered)
			{
				sendWelcomeMsg(fdClient);
				joinGeneralChannel(fdClient);
			}
		}	
		else
			answerClient(fdClient, ERR_UNKNOWNCOMMAND, "", "Unknown command (" + msg.command + "). Remember that command must be UPPERCASE.");
			
		



















	}
	else
	{
		std::map<std::string, FCmd>::iterator it = _fCommands.find(msg.command);
		if (it != _fCommands.end()) 
		{
			std::cout << it->first << std::endl;
			FCmd func = it->second;
			(this->*func)(msg, fdClient);
		} 
		else
			answerClient(fdClient, ERR_UNKNOWNCOMMAND, "", _clients[fdClient]->getNickname() + ": Unknown command (" + msg.command + "). Remember that command must be UPPERCASE.");
	}
}


