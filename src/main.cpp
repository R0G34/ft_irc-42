// ======================================================================
// Archivo: main.cpp
// Propósito: Punto de entrada del programa. Valida los argumentos y arranca el servidor IRC.
// ======================================================================

#include <irc.hpp>
#include <Server.hpp>

// Gestiona la lógica asociada a un evento o comando.
void	handleSignal(int signal) {
	(void)signal;
		std::cout << "\nManage Signal..." << std::endl;
}

// Función auxiliar que realiza la operación principal asociada.
void	runSignals() {
	signal(SIGINT, handleSignal);
	signal(SIGQUIT, handleSignal);
	signal(SIGTSTP, handleSignal);
	signal(SIGTERM, handleSignal);
	signal(SIGPIPE, SIG_IGN);
}

// Interpreta y descompone los datos de entrada.
int	parseArgs(const std::string &port, const std::string &password) 
{
	for (size_t i = 0; i < port.size(); ++i) 
	{//DMK
		if (!std::isdigit(port[i]))
			throw std::invalid_argument("Port must be a number.");
		if (port[i] == '0' && i == 0)
			throw std::invalid_argument("Port cannot start with zero.");
		if (i > 5)
			throw std::length_error("Port is too long.");
	}

	int check = std::atoi(port.c_str());
	if (check <= 1024 || check > 65535)
		throw std::out_of_range("Port must be between 1024 and 65535.");
	if (password.empty())
		throw std::invalid_argument("Password cannot be empty.");
	return check;
}

// Función auxiliar que realiza la operación principal asociada.
int main(int ac, char** av) {
	
	if (ac != 3) {
		std::cerr << "Usage: " << av[0] << " <port> <password>" << std::endl;
		return 1;
	}
	
	try {
		runSignals();
		int port = parseArgs(av[1], av[2]); 
		Server	Server(port, av[2]);
		Server.init();
		Server.run();
	}
	catch (const std::exception &e) {
		std::cerr << RED << "Error: " << CLEAR << e.what() << std::endl;
		return 1;
	}

	return 0;
}