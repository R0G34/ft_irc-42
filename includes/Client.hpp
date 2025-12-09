#pragma once

#ifndef CLIENT_HPP
# define CLIENT_HPP






# include <irc.hpp>
# include <sys/socket.h>
# include <netinet/in.h>
# include "Channel.hpp"

class Channel;

enum RegistrationStatus {
    RS_NoPass = 0,
    RS_PassValidated,
    RS_NickValidated,
    RS_Registered
};

class Client
{
	private:
		std::string				_userName;
		std::string 			_nickName;
		std::string 			_realName;
		const int				_clientFd;
		sockaddr_in	*			_clientAddr;
		RegistrationStatus		_registrationState;
		std::string				_bufferMsgClient;
		std::vector<Channel*>	_channels;
		std::string				_ip;

	public:

		Client(const int fd, sockaddr_in *clientAddr);

		~Client();


		const std::string &getUsername() const;

		const std::string &getNickname() const;

		const std::string &getRealname() const;

		const RegistrationStatus &getRegistrationState() const;

		const std::string GetIp() const;

		int getFd() const;

		std::string &getBufferMsgClient();

		std::vector<Channel*>& getChannels();


		void setUsername(const std::string &username);

		void setNickname(const std::string &nickname);

		void setRealname(const std::string &realname);

		void setRegistrationState(RegistrationStatus st);

		void setBufferMsgClient(const std::string &msg);


    	void joinChannel(Channel* ch);

    	void leaveChannel(Channel* ch);

};

#endif