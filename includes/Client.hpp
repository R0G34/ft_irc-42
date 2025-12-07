#pragma once

#ifndef CLIENT_HPP
# define CLIENT_HPP

// ======================================================================
// Archivo: Client.hpp
// Propósito: Declara la clase Client, que representa a un usuario conectado al servidor IRC.
// ======================================================================

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
		// Función auxiliar que realiza la operación principal asociada.
		Client(const int fd, sockaddr_in *clientAddr);
		// Destructor de la clase Client.
		~Client();

		// Getters
		const std::string &getUsername() const;
		// Devuelve el atributo correspondiente.
		const std::string &getNickname() const;
		// Devuelve el atributo correspondiente.
		const std::string &getRealname() const;
		// Devuelve el atributo correspondiente.
		const RegistrationStatus &getRegistrationState() const;
		// Devuelve el atributo correspondiente.
		const std::string GetIp() const;
		// Devuelve el atributo correspondiente.
		int getFd() const;
		// Devuelve el atributo correspondiente.
		std::string &getBufferMsgClient();
		// Devuelve el atributo correspondiente.
		std::vector<Channel*>& getChannels();

		// Setters
		void setUsername(const std::string &username);
		// Actualiza el atributo correspondiente.
		void setNickname(const std::string &nickname);
		// Actualiza el atributo correspondiente.
		void setRealname(const std::string &realname);
		// Actualiza el atributo correspondiente.
		void setRegistrationState(RegistrationStatus st);
		// Actualiza el atributo correspondiente.
		void setBufferMsgClient(const std::string &msg);

    	// Función auxiliar que realiza la operación principal asociada.
    	void joinChannel(Channel* ch);
    	// Función auxiliar que realiza la operación principal asociada.
    	void leaveChannel(Channel* ch);

};

#endif