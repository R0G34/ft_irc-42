#pragma once

#ifndef SERVER_HPP
# define SERVER_HPP

// ======================================================================
// Archivo: Server.hpp
// Propósito: Declara la clase Server y la estructura de datos principal del servidor IRC.
// ======================================================================

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
		// Declaración auxiliar utilizada en el servidor IRC.
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
		
		// Función auxiliar que realiza la operación principal asociada.
		void	connectNewClient();
		// Función auxiliar que realiza la operación principal asociada.
		void	disconnectClient(int fd);
		// Interpreta y descompone los datos de entrada.
		t_msg	parseMsg(std::string msg);
		// Función auxiliar que realiza la operación principal asociada.
		void	readMsg(int fd);
		// Función auxiliar que realiza la operación principal asociada.
		void	manageServerInput();
		// Gestiona la lógica asociada a un evento o comando.
		void	handleCommand(t_msg& msg, int fd);
		// Función auxiliar que realiza la operación principal asociada.
		void	answerClient(int fdClient, int code, const std::string &target, const std::string& msg);
		// Inicializa el estado interno o los recursos necesarios.
		void	initCmds();
		// Envía datos o mensajes al cliente o al canal correspondiente.
		void	sendMsgToClient(int fd, const std::string &cmd, const std::string &channel, const std::string &msg);
		// Función auxiliar que realiza la operación principal asociada.
		void	msgClientToClient(int from, int to, const std::string &cmd, const std::string &msg);
		// Envía datos o mensajes al cliente o al canal correspondiente.
		void	sendWelcomeMsg(int fdClient);
		// Función auxiliar que realiza la operación principal asociada.
		void	joinGeneralChannel(int fdClient);
		// Crea o inicializa los recursos necesarios.
		std::string makePrefix(int fd);


		/* COMMANDS */
		// Envía datos o mensajes al cliente o al canal correspondiente.
		void sendMsgChangeNick(std::string newNick, int fdClient);
		// Función auxiliar que realiza la operación principal asociada.
		void CmPass(t_msg& msg, int fd);
		// Función auxiliar que realiza la operación principal asociada.
		void CmNick(t_msg& msg, int fd);
		// Función auxiliar que realiza la operación principal asociada.
		void CmUser(t_msg& msg, int fd);
		// Función auxiliar que realiza la operación principal asociada.
		void CmCAP(t_msg& msg, int fd);
		// Función auxiliar que realiza la operación principal asociada.
		void CmPrivMsg(t_msg &msg, int fd);
		// Función auxiliar que realiza la operación principal asociada.
		void CmJoin(t_msg &msg, int fd);
		// Función auxiliar que realiza la operación principal asociada.
		void CmList(t_msg &msg, int fd);
		// Función auxiliar que realiza la operación principal asociada.
		void CmPart(t_msg &msg, int fd);
		// Función auxiliar que realiza la operación principal asociada.
		void CmInvite(t_msg &msg, int fd);
		// Función auxiliar que realiza la operación principal asociada.
		void CmKick(t_msg &msg, int fd);
		// Función auxiliar que realiza la operación principal asociada.
		void CmTopic(t_msg &msg, int fd);
		// Función auxiliar que realiza la operación principal asociada.
		void CmMode(t_msg &msg, int fd);
		// Función auxiliar que realiza la operación principal asociada.
		void manageAddMode(char mode, const std::string &channel, std::vector<std::string> &params, int fd);
		// Función auxiliar que realiza la operación principal asociada.
		void manageRemoveMode(char mode, const std::string &channel, std::vector<std::string> &params, int fd);
		// Función auxiliar que realiza la operación principal asociada.
		void CmNames(t_msg &msg, int fd);
		// Función auxiliar que realiza la operación principal asociada.
		void CmWho(t_msg& msg, int fd);
		// Función auxiliar que realiza la operación principal asociada.
		void CmQuit(t_msg& msg, int fd);
		// Función auxiliar que realiza la operación principal asociada.
		void CmNotice(t_msg &msg, int fd);

	public:

		// Función auxiliar que realiza la operación principal asociada.
		Server(const int &port, const std::string &password);
		// Destructor de la clase Server.
		~Server();

		// Getters
		const int &getPort() const;
		// Devuelve el atributo correspondiente.
		const std::string &getPassword() const;

		// Ejecuta el bucle principal o pone en marcha el componente.
		void 	run();
		// Inicializa el estado interno o los recursos necesarios.
		void 	init();
};

// Actualiza el atributo correspondiente.
bool	setNonBlocking(int fd);
// Comprueba o valida una condición concreta.
bool	isSpecial(char c);
// Función auxiliar que realiza la operación principal asociada.
std::string currentDateTimeString();

template<typename T>
static size_t stringtoint( const T & s ) {
    size_t i;
    // Función auxiliar que realiza la operación principal asociada.
    std::istringstream(s) >> i;
    return i;
}

template<typename T>
std::string to_string(const T &value) {
    std::ostringstream oss;
    oss << value;
    // Función auxiliar que realiza la operación principal asociada.
    return oss.str();
}

template<typename T>
void splitCmd(const std::string &cmd, T &result, const char del) {
	size_t start = 0;
	size_t comma;
	while ((comma = cmd.find(del, start)) != std::string::npos) {
		if (comma == start) {
			start++;
			continue;
		}
		// Función auxiliar que realiza la operación principal asociada.
		result.push_back(cmd.substr(start, comma - start));
		start = comma + 1;
	}
	// Función auxiliar que realiza la operación principal asociada.
	result.push_back(cmd.substr(start));
}

#endif