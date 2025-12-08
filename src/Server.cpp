// ======================================================================
// Archivo: Server.cpp
// Propósito: Implementación de la clase Server: gestión del socket, epoll y bucle principal del servidor IRC.
// ======================================================================

#include <Server.hpp>

// Devuelve el atributo correspondiente.
const int &Server::getPort() const { return _port; }

// Devuelve el atributo correspondiente.
const std::string &Server::getPassword() const { return _password; }

// Método de la clase Server( const int &port, const std que realiza la operación principal asociada.
Server::Server( const int &port, const std::string &password )
	: _port(port), _password(password), _running(true), _socketFd(-1), _epollFd(-1), _maxChannelUsers(15),
	   _serverName ("ircserver.com"), _version("ChatServ-1.0"), _chanModes("itoblk") {}

// Destructor de la clase Server.
Server::~Server() {
	for (std::map<int, Client *>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
		if (it->second) {
			/*NOTE: */
			std::cout << "Closing client with fd: " << it->second->getFd() << std::endl;
			if (_epollFd >= 0)
				epoll_ctl(_epollFd, EPOLL_CTL_DEL, it->second->getFd(), NULL);
			close(it->second->getFd());
			delete it->second;
		}
	}

	for (std::map<std::string, Channel *>::iterator it = _channel.begin(); it != _channel.end(); ++it) {
		if (it->second)
			delete it->second;
	}

	_clients.clear();
	if (_socketFd >= 0)
		close(_socketFd);
	if (_epollFd >= 0)
		close(_epollFd);

	/*NOTE: */
	std::cout << "Server closed" << std::endl;
}

// Inicializa el estado interno o los recursos necesarios.
void Server::init() 
{
	_socketFd = socket(AF_INET, SOCK_STREAM, 0);
	if (_socketFd < 0) 
		throw std::runtime_error("Creating the socket server");
	/*NOTE: */
	std::cout << "Socket created with fd -> " << _socketFd << std::endl;

	int	opt = 1;
	if (setsockopt(_socketFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
		throw std::runtime_error("When setting SO_REUSEADDR");

	if (!setNonBlocking(_socketFd))
		throw std::runtime_error("Setting flags to non blocking");

	std::memset(&_servAddr, 0, sizeof(_servAddr));
	_servAddr.sin_family = AF_INET; 
	_servAddr.sin_addr.s_addr = INADDR_ANY; 
	_servAddr.sin_port = htons(_port); 

	if (bind(_socketFd, (sockaddr*)&_servAddr, sizeof(_servAddr)) < 0)
		throw std::runtime_error("When using bind()");

	if (listen(_socketFd, 5) < 0)
		throw std::runtime_error("When using listen()");

	_epollFd = epoll_create1(0);
	if (_epollFd < 0)
		throw std::runtime_error("When creating epoll instance");

	epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.fd = _socketFd; 
	if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, _socketFd, &ev) < 0)
		throw std::runtime_error("When adding the socket to the epoll instance");

	fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
	epoll_event stdin_ev;
	stdin_ev.events = EPOLLIN;
	stdin_ev.data.fd = STDIN_FILENO;
	if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, STDIN_FILENO, &stdin_ev) < 0)
		throw std::runtime_error("When adding stdin to epoll instance");

	initCmds();
	_creationDate = currentDateTimeString();

	/* NOTE:  11. Print server info */
	std::cout << "Server initialized with the following parameters:" << std::endl;
	std::cout << "Port: " << _port << std::endl;
	std::cout << "Password: " << _password << std::endl;
	std::cout << "Socket file descriptor: " << _socketFd << std::endl;
	std::cout << "Epoll file descriptor: " << _epollFd << std::endl;
	std::cout << "Server port (network byte order): " << ntohs(_servAddr.sin_port) << std::endl;
	std::cout << "Server port (host byte order): " << _servAddr.sin_port << std::endl;
	std::cout << "Server address family: " << _servAddr.sin_family << std::endl << std::endl;
	std::cout << "Welcome to the IRC server!" << std::endl;
	std::cout << "Type 'help' for a list of commands." << std::endl;
	std::cout << "Type 'exit' or 'quit' to stop the server." << std::endl;
}

// Método de la clase void	Server que realiza la operación principal asociada.
void	Server::connectNewClient()
{
	sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);

	int client_fd = accept(_socketFd, (sockaddr*)&client_addr, &client_len);
	if (client_fd < 0)
		throw std::runtime_error("Client dont works");

	if (!setNonBlocking(client_fd))
		throw std::runtime_error("Setting flags to non blocking for client");

	Client	*auxClient = new Client(client_fd, &client_addr);
	epoll_event ev;
	ev.events = EPOLLIN | EPOLLRDHUP;
	ev.data.fd = client_fd;
	if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, client_fd, &ev) < 0)
		throw std::runtime_error("When add new client to epoll");
	_clients.insert(std::pair<int, Client*>(client_fd, auxClient));

	/* NOTE: Client Data */
	std::cout << "Client fd connected with fd: " << client_fd << std::endl;
	std::cout << " Client connected from: "
	<< inet_ntoa(client_addr.sin_addr) << ":" 
	<< ntohs(client_addr.sin_port) << std::endl;
}

// Interpreta y descompone los datos de entrada.
t_msg	Server::parseMsg(std::string fullMsg)
{
	/* NOTE: */
	std::cout << "FULL MSG RECIVED:\n" << BLUE << fullMsg << CLEAR << std::endl;

	while (!fullMsg.empty() && (fullMsg[fullMsg.size() - 1] == '\r' || fullMsg[fullMsg.size() - 1] == '\n'))
    	fullMsg.resize(fullMsg.size() - 1);

	t_msg	msg;
	msg.allMsg = fullMsg;

	size_t firstChar = fullMsg.find_first_not_of(' ');
	if (firstChar != std::string::npos)
    	fullMsg = fullMsg.substr(firstChar);

	size_t firstSpace = fullMsg.find(' ');
	std::string args;

	if (firstSpace == std::string::npos)
	{
		msg.command = fullMsg;
		args = "";
	}
	else
	{
		msg.command = fullMsg.substr(0, firstSpace);
		args = fullMsg.substr(firstSpace + 1);
	}

	std::size_t trailingPos = args.find(':');
	if (trailingPos != std::string::npos)
	{
		msg.trailing = args.substr(trailingPos + 1);
		msg.hasTrailing = true;
		args = args.substr(0, trailingPos);
	}

	while(!args.empty() && msg.params.size() < MAX_PARAMS)
	{
		size_t pos = args.find(' ');
		std::string token;

		if (pos == std::string::npos)
		{
			token = args;
			args.clear();
		}
		else
		{
			token = args.substr(0, pos);
			args = args.substr(pos + 1);
		}

		msg.params.push_back(token);
		size_t nonSpace = args.find_first_not_of(' ');

		if (nonSpace != std::string::npos)
			args = args.substr(nonSpace);
		else
			args.clear();
	}

	if (trailingPos == std::string::npos && !args.empty() && msg.params.size() < MAX_PARAMS + 1)
	{
		size_t pos = args.find(' ');
		if (pos == std::string::npos)
			msg.params.push_back(args);
		else
		{
			args = args.substr(0, pos);
			msg.params.push_back(args);
		}
	}
	return msg;
}


// Método de la clase void	Server que realiza la operación principal asociada.
void	Server::readMsg(int fd)
{
	/* NOTE: */
	// if (_clients.find(fd) == _clients.end())
    //     return;

	std::cout << "Event received from fd: " << fd << std::endl;
	
	char	msg[MAX_BYTES_MSG];
	std::memset(msg, 0, sizeof(msg));

	int bytes_recived =  recv(fd, &msg, MAX_BYTES_MSG, 0);

	if (bytes_recived < 0) {
		disconnectClient(fd);
		return;
	}
		/*throw std::runtime_error("On recv()");*/

	// if (_clients.find(fd) == _clients.end())
	// 	return;

    std::string aux = _clients[fd]->getBufferMsgClient();
    aux.append(msg, bytes_recived);
    _clients[fd]->setBufferMsgClient(aux);

    std::string& buffer = _clients[fd]->getBufferMsgClient();
    std::size_t pos;

    while ((pos = buffer.find("\n")) != std::string::npos)
    {
        std::string fullMsg = buffer.substr(0, pos);
        buffer.erase(0, pos + 1);

		/* NOTE: */
        std::cout << YELLOW << "MSG: " << fullMsg << CLEAR << std::endl;

        t_msg parsedMsg = parseMsg(fullMsg);
		handleCommand(parsedMsg, fd);
		if (_clients.find(fd) == _clients.end())
			return ;
    }
}

// Método de la clase void Server que realiza la operación principal asociada.
void Server::disconnectClient(int fd) 
{
	/*
	if (_clients.find(fd) == _clients.end()) 
		throw std::runtime_error("Trying to disconnect a client that does not exist");

	std::cout << "disconnectClient 1 DMK" << std::endl;
	
	Client *client = _clients[fd];
	for (std::map<std::string, Channel *>::iterator it = _channel.begin(); it != _channel.end(); ++it) 
	{
		_channel[it->first]->disconnectUser(client);
		if (_channel[it->first]->getUserCount() == 1 && _channel[it->first]->hasUser("Bot"))
			_channel[it->first]->disconnectUser(_clients[_channel[it->first]->getUserFd("Bot")]);
	}

	std::cout << "disconnectClient 2 DMK" << std::endl;
	if (epoll_ctl(_epollFd, EPOLL_CTL_DEL, fd, NULL) < 0)
		throw std::runtime_error("When removing client from epoll instance");

	std::cout << "disconnectClient 3 DMK" << std::endl;	
	if (close(fd) < 0) 
		throw std::runtime_error("When closing client socket");

	std::cout << "disconnectClient 4 DMK" << std::endl;
	delete client;
	
	std::cout << "disconnectClient 5 DMK" << std::endl;
	_clients.erase(fd);
	std::cout << "disconnectClient 6 DMK" << std::endl;

	std::map<std::string, Channel *>::iterator it_begin = _channel.begin();
	std::map<std::string, Channel *>::iterator it_end = _channel.end();
	for (std::map<std::string, Channel *>::iterator it = it_begin; it != it_end; it++) 
	{
		std::cout << "disconnectClient 7 DMK: channel: " << _channel[it->first]->getName() << ", Número usuarios: " << _channel[it->first]->getUserCount() << std::endl;
		if (NULL != _channel[it->first] && _channel[it->first]->getUserCount() == 0) 
		{
			std::cout << "disconnectClient 8 DMK" << std::endl;
			delete _channel[it->first];
			std::cout << "disconnectClient 9 DMK" << std::endl;
//DMK 
			_channel.erase(it->first);
			std::cout << "disconnectClient 10 DMK" << std::endl;
		}
	}*/
	/* NOTE: */
	//std::cout << GREEN << "Client disconnected successfully." << CLEAR << std::endl;


    // Buscar el cliente primero
    std::map<int, Client*>::iterator itClient = _clients.find(fd);
    if (itClient == _clients.end())
        throw std::runtime_error("Trying to disconnect a client that does not exist");
    Client *client = itClient->second;
    
	// 1) Sacar al cliente de todos los canales (sin borrar canales aún)
    for (std::map<std::string, Channel *>::iterator it = _channel.begin(); it != _channel.end(); ++it)
    {
        Channel *ch = it->second;
        ch->disconnectUser(client);
        // Si quieres gestionar el Bot:
        if (ch->getUserCount() == 1 && ch->hasUser("Bot")) 
		{
            int botFd = ch->getUserFd("Bot");
            std::map<int, Client*>::iterator itBot = _clients.find(botFd);
            if (itBot != _clients.end())
                ch->disconnectUser(itBot->second);
        }
    }

    // 2) Quitar fd de epoll y cerrar socket
    if (epoll_ctl(_epollFd, EPOLL_CTL_DEL, fd, NULL) < 0)
        throw std::runtime_error("When removing client from epoll instance");
		
    if (close(fd) < 0)
        throw std::runtime_error("When closing client socket");
    
	// 3) Borrar el Client y sacarlo del map
    delete client;
    _clients.erase(itClient);
    
	// 4) Borrar canales vacíos con patrón seguro
    for (std::map<std::string, Channel *>::iterator it = _channel.begin(); it != _channel.end(); )
    {
        Channel *ch = it->second;
        if (ch->getUserCount() == 0) 
		{
            std::map<std::string, Channel*>::iterator toErase = it;
			++it;
            _channel.erase(toErase);
            delete ch;
        } else 
		{
            ++it;
        }
    }
    std::cout << "Client disconnected successfully.\n";
	
}

// Método de la clase void  Server que realiza la operación principal asociada.
void  Server::manageServerInput() 
{
	std::string input;
	std::getline(std::cin, input);
	if (input.empty())
		return ;
	std::vector<std::string> commands;
	splitCmd(input, commands, ' ');
	if (commands[0] == "exit" || commands[0] == "quit") {
		_running = false;
		/* NOTE: */
		std::cout << PINK << "Server is shutting down..." << CLEAR << std::endl;
	}
	else if (commands[0] == "clients") {
		// TODO: Darle una vuelta a la información facilitada
		std::cout << "Connected clients: " << std::endl;
		for (std::map<int, Client *>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
			std::cout << "[ Client fd: " << it->first
					  << ", Username: " << it->second->getUsername() 
					  << ", Nickname: " << it->second->getNickname()
					  << " ]" << std::endl;
		}
	}
	else if (commands[0] == "channels") {
		// TODO: Darle una vuelta a la información facilitada
		std::cout << "Available channels: " << std::endl;
		for (std::map<std::string, Channel *>::iterator it = _channel.begin(); it != _channel.end(); ++it) {
			std::cout << "[ Channel name: " << it->first 
					  << ", Max users: " << it->second->getMaxUsers() 
					  << ", Password: " << it->second->getPass() << " ]"
					  << std::endl;
		}
	}
	else if (commands[0] == "clear" || commands[0] == "cls")
		std::cout << "\033[2J\033[1;1H";
	else if (commands[0] == "limit") {
		if (commands.size() > 1) {
			int number = stringtoint(commands[1]);
			if (number < 1 || number > 100)
				std::cout << RED << "Please choose a number between 1 and 100" << CLEAR << std::endl;
			else
				_maxChannelUsers = number;
		}
		std::cout << "Max channels per user: " << _maxChannelUsers << std::endl;
	}
	else if (commands[0] == "version")
		std::cout << "Server version: 1.0.0" << std::endl;
	else if (commands[0] == "help") {
		std::cout << "Available commands:" << std::endl;
		std::cout << "- exit/quit: Stop the server." << std::endl;
		std::cout << "- clients: List connected clients." << std::endl;
		std::cout << "- channels: List channels." << std::endl;
		std::cout << "- clear/cls: Clear the console." << std::endl;
		std::cout << "- limit: Show max channels per user." << std::endl;
		std::cout << "- version: Show server version." << std::endl;
		std::cout << "- help: Show this help message." << std::endl;
	}
	else
		std::cout << RED << "Command not recognized. Type 'help' for a list of commands." << CLEAR << std::endl;
}

// Ejecuta el bucle principal o pone en marcha el componente.
void Server::run() 
{
	epoll_event events[MAX_EVENTS];

	while (_running) 
	{
		std::cout << PINK << "Waiting for events..." << CLEAR << std::endl << std::endl;
		int numEvents = epoll_wait(_epollFd, events, MAX_EVENTS, -1);
		std::cout << PINK << "RUN DMK 1..." << CLEAR << std::endl << std::endl;
		if (numEvents < 0) 
		{
			std::cout << PINK << "RUN DMK 2..." << CLEAR << std::endl << std::endl;
			if (errno == EINTR) 
			{
				std::cout << "Closing server by signal..." << std::endl;
				_running = false;
				return; //continue;
			}
			std::cout << PINK << "RUN DMK 3..." << CLEAR << std::endl << std::endl;
			throw std::runtime_error("When waiting for events");
		}
		try 
		{
			std::cout << PINK << "RUN DMK 4..." << CLEAR << std::endl << std::endl;
			for (int i = 0; i < numEvents; i++) 
			{
				std::cout << PINK << "RUN DMK 5..." << CLEAR << std::endl << std::endl;
				if (events[i].data.fd == STDIN_FILENO) 
				{
					std::cout << PINK << "RUN DMK 6..." << CLEAR << std::endl << std::endl;
					manageServerInput();
				}
				else if (events[i].data.fd == _socketFd)
				{
					std::cout << PINK << "RUN DMK 7..." << CLEAR << std::endl << std::endl;
					connectNewClient();
				}
				else 
				{
					std::cout << PINK << "RUN DMK 8..." << CLEAR << std::endl << std::endl;
					int fd = events[i].data.fd;

					if (events[i].events & EPOLLIN)
					{
						std::cout << PINK << "RUN DMK 9..." << CLEAR << std::endl << std::endl;
						readMsg(fd);
					}
					if (events[i].events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR))
					{
						std::cout << PINK << "RUN DMK 10..." << CLEAR << std::endl << std::endl;
						disconnectClient(fd);
					}
				}
			}
		}
		catch (const std::exception &e) 
		{
			std::cerr << RED << "Error: " << CLEAR << e.what() << std::endl;
		}
	}
	std::cout << PINK << "RUN DMK 11..." << CLEAR << std::endl << std::endl;
}
