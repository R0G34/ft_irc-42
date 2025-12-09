#pragma once

#ifndef SERVER_HPP
# define SERVER_HPP






# include <irc.hpp>
# include <Client.hpp>
# include <Channel.hpp>

class Client;
class Channel;

typedef struct	s_msg
{
	std::string					allMsg;
	std::string					command;
	std::vector<std::string>	params;
	std::string 				trailing;
	bool						hasTrailing;
}  t_msg;

class Server
{
	private:

		typedef void (Server::*FCmd)(t_msg&, int);

		const int							_port;
		const std::string					_password;
		bool								_running;
		std::map <int, Client *>			_clients;
		std::map <std::string, Channel *>	_channel;
		int									_socketFd;
		sockaddr_in							_servAddr;
		int									_epollFd;
		std::map <std::string, FCmd>		_fCommands;
		size_t 								_maxChannelUsers;
		std::string 						_serverName;
		std::string							_version;
		std::string							_creationDate;
		std::string							_chanModes;
		

		void	connectNewClient();

		void	disconnectClient(int fd);

		t_msg	parseMsg(std::string msg);

		void	readMsg(int fd);

		void	manageServerInput();

		void	handleCommand(t_msg& msg, int fd);

		void	answerClient(int fdClient, int code, const std::string &target, const std::string& msg);

		void	initCmds();

		void	sendMsgToClient(int fd, const std::string &cmd, const std::string &channel, const std::string &msg);

		void	msgClientToClient(int from, int to, const std::string &cmd, const std::string &msg);

		void	sendWelcomeMsg(int fdClient);

		void	joinGeneralChannel(int fdClient);

		std::string makePrefix(int fd);


		/* COMMANDS */

		void sendMsgChangeNick(std::string newNick, int fdClient);

		void CmPass(t_msg& msg, int fd);

		void CmNick(t_msg& msg, int fd);

		void CmUser(t_msg& msg, int fd);

		void CmCAP(t_msg& msg, int fd);

		void CmPrivMsg(t_msg &msg, int fd);

		void CmJoin(t_msg &msg, int fd);

		void CmList(t_msg &msg, int fd);

		void CmPart(t_msg &msg, int fd);

		void CmInvite(t_msg &msg, int fd);

		void CmKick(t_msg &msg, int fd);

		void CmTopic(t_msg &msg, int fd);

		void CmMode(t_msg &msg, int fd);

		void manageAddMode(char mode, const std::string &channel, std::vector<std::string> &params, int fd);

		void manageRemoveMode(char mode, const std::string &channel, std::vector<std::string> &params, int fd);

		void CmNames(t_msg &msg, int fd);

		void CmWho(t_msg& msg, int fd);

		void CmQuit(t_msg& msg, int fd);

		void CmNotice(t_msg &msg, int fd);

	public:


		Server(const int &port, const std::string &password);

		~Server();


		const int &getPort() const;

		const std::string &getPassword() const;


		void 	run();

		void 	init();
};


bool	setNonBlocking(int fd);

bool	isSpecial(char c);

std::string currentDateTimeString();

template<typename T>
static size_t stringtoint( const T & s ) 
{
    size_t i;

    std::istringstream(s) >> i;
    return i;
}

template<typename T>
std::string to_string(const T &value) 
{
    std::ostringstream oss;
    oss << value;

    return oss.str();
}

template<typename T>
void splitCmd(const std::string &cmd, T &result, const char del) 
{
	size_t start = 0;
	size_t comma;
	while ((comma = cmd.find(del, start)) != std::string::npos) 
	{
		if (comma == start) 
		{
			result.push_back("");
			start++;
			continue;
		}

		result.push_back(cmd.substr(start, comma - start));
		start = comma + 1;
	}

	result.push_back(cmd.substr(start));
}

#endif