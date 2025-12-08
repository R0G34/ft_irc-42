// ======================================================================
// Archivo: CmMode.cpp
// Propósito: Implementación del comando IRC MODE: valida parámetros y aplica su lógica sobre clientes y canales.
// ======================================================================

#include <Server.hpp>

// MODE <channel/user> [<mode> [<mode params>]]
void Server::CmMode(t_msg &msg, int fd) 
{
	if (msg.params.size() < 1 || (msg.params.size() == 1 && msg.params[0].size() <= 1)) 
	{
		answerClient(fd, ERR_NEEDMOREPARAMS, "MODE", "Not enough parameters");
	}
	else
	{
		std::string channel = msg.params[0];
		if (channel[0] == '#' || channel[0] == '&') 
		{
			if (_channel.find(channel) == _channel.end()) 
			{
				answerClient(fd, ERR_NOSUCHCHANNEL, msg.params[0], "Error: the channel " + channel + ", doesn't exist on the server");
			}
			else if (msg.params.size() == 1)
			{
				std::string modes = _channel[msg.params[0]]->getMode();
				answerClient(fd, RPL_CHANNELMODEIS, msg.params[0], modes);
			}
			else
			{
				if (channel == "#general" || channel == "&general") 
				{
					answerClient(fd, ERR_CHANOPRIVSNEEDED, msg.params[0], "Cannot change modes of the general channel");
				}
				else
				{
					if (!_channel[msg.params[0]]->isAdmin(fd)) 
					{
						answerClient(fd, ERR_CHANOPRIVSNEEDED, msg.params[0], "You're not channel operator");
					}
					else
					{
						//analize MODE command
						std::vector<std::string> modes;
						std::vector<std::string> params;
						//size_t modeCount = 0;
						std::string sign;
						std::string mode;
						std::vector<std::string> validModes; //("i", "t", "k", "l", "o", "b");
						validModes.push_back("i");validModes.push_back("t");validModes.push_back("k");validModes.push_back("l");validModes.push_back("o");validModes.push_back("b");;
						for (size_t i = 1; i < msg.params.size(); ++i) 
						{
							if (i%2 == 1) //EL MODO
							{
								sign = msg.params[i][0];									
								if((sign == "+" || sign == "-") && msg.params[i].size() == 2)
								{	
									mode = msg.params[i][1];
									if(std::find(validModes.begin(), validModes.end(), mode) != validModes.end())
										modes.push_back(sign + mode);
									else
									{
										answerClient(fd, ERR_UNKNOWNMODE, mode, "Unknown mode (Valid mode: k,l,i,t,o,b)");
										return ;
									}
										
									
									
									// bool valid = false;
									// for (size_t j = 0; j < msg.params[i].size(); ++j) 
									// {
									// 	valid = false;
									// 	if (msg.params[i][j] == '+' || msg.params[i][j] == '-') 
									// 	{
									// 		sign = msg.params[i][j];
									// 		continue ;
									// 	}
									// 	if (isalpha(msg.params[i][j])) 
									// 	{
									// 		if (sign.empty()) 
									// 		{
									// 			answerClient(fd, ERR_NEEDMOREPARAMS, "MODE", "Not enough parameters for mode");
									// 			return ;
									// 		}
									// 		if (msg.params[i][j] == 'k' || 
									// 			msg.params[i][j] == 'l' || 
									// 			msg.params[i][j] == 'i'	|| 
									// 			msg.params[i][j] == 't' || 
									// 			msg.params[i][j] == 'o' || 
									// 			msg.params[i][j] == 'b') 
									// 			{
									// 				if (((msg.params[i][j] == 'k' || 
									// 					  msg.params[i][j] == 'l') && sign == "+") ||  
									// 					  msg.params[i][j] == 'o' || msg.params[i][j] == 'b')
									// 				modeCount++;
									// 			valid = true;
									// 			modes.push_back(sign + msg.params[i][j]);
									// 		}
									// 		else 
									// 		{
									// 			std::string character;
									// 			character += msg.params[i][j];
									// 			answerClient(fd, ERR_UNKNOWNMODE, character, "Unknown mode (Valid mode: k,l,i,t,o,b)");
									// 			return ;
									// 		}
									// 	}
									// }
									// if (!valid) 
									// {
									// 	answerClient(fd, ERR_NEEDMOREPARAMS, "MODE", "Sign must be followed by a mode");
									// 	return ;
									// }									
								}							
								else  //DMK OJO CON LA CONSTANTE DE ERROR QUE ESTOY UTILIZANDO.
								{
									// std::string errMsg = "Error MODE command format. (MODE #channel ";
									// errMsg += msg.params[i][0];
									// errMsg += "x ...), where x represent the channel mode you want to change.";
									answerClient(fd, ERR_NEEDMOREPARAMS, "MODE", "Error MODE command format. (MODE #channel " + sign + "x ...), where x represent the channel mode you want to change.");
									return;
								}
							}
							else //PARAM
							{	
								std::cout << "Signo = " << sign << ", modo = " << mode[0] << ", param = " << msg.params[i] << std::endl;
								params.push_back(msg.params[i]);
								if("+" == sign)
									manageAddMode(mode[0], channel, params, fd);
								else
									manageRemoveMode(mode[0], channel, params, fd);
								params.clear();
								sign = "";
								mode = "";
								std::cout << "After manage mode --> Param = " << msg.params[i] << std::endl;
							}

						}
						if(mode.size() != 0)
						{
							std::cout << "Signo = " << sign << ", modo = " << mode[0] << std::endl;
							if("+" == sign)
								manageAddMode(mode[0], channel, params, fd);
							else
								manageRemoveMode(mode[0], channel, params, fd);
						}

						//DMK EN TEORIA SE CHEQUEA ARRIBA SI SOLO ES DE TIPO MODE  #channel
						// if (modes.empty()) {
						// 	answerClient(fd, RPL_CHANNELMODEIS, msg.params[0], _channel[msg.params[0]]->getMode());
						// 	return ;
						// }

						//DMK PREGUNTAR POR QUÉ HACE ESTO.... CUAND
						// if (params.size() != modeCount) 
						// {
						// 	size_t modeB = 0;
						// 	if ((modeCount - params.size()) > 0) 
						// 	{
						// 		for (size_t i = 0; i < modes.size(); ++i) 
						// 		{
						// 			if (modes[i] == "+b")
						// 				modeB++;
						// 		}
						// 	}
						// 	if ((modeCount - params.size()) > modeB) 
						// 	{
						// 		answerClient(fd, ERR_NEEDMOREPARAMS, "MODE", "Not enough parameters for mode");
						// 		return ;
						// 	}
						// }

						// for (size_t i = 0; i < modes.size(); ++i) 
						// {
						// 	if (modes[i][0] == '+')
						// 		manageAddMode(modes[i][1], msg.params[0], params, fd);
						// 	else if (modes[i][0] == '-')
						// 		manageRemoveMode(modes[i][1], msg.params[0], params, fd);
						// 	else {
						// 		answerClient(fd, ERR_NEEDMOREPARAMS, "MODE", "Mode must start with + or -");
						// 		return ;
						// 	}
						// }
					}
				}
			}
		}
		else 
		{
			answerClient(fd, ERR_NOSUCHCHANNEL, msg.params[0], "MODE command error: channel name must start with \"#\" o \"&\" (MODE #channel...)");
		}
	}

	//DMK para no tener duplicado el chequeo de si el canal está correctamente
	/*
	if (msg.params.size() < 1 || (msg.params.size() == 1 && msg.params[0].size() <= 1)) 
	{
		answerClient(fd, ERR_NEEDMOREPARAMS, "MODE", "Not enough parameters");
		return ;
	}

	
	if (msg.params.size() == 1) 
	{
		if (msg.params[0][0] == '#' || msg.params[0][0] == '&') 
		{
			if (_channel.find(msg.params[0]) == _channel.end()) 
			{
				answerClient(fd, ERR_NOSUCHCHANNEL, msg.params[0], "No such channel 1");
				return ;
			}
			std::string modes = _channel[msg.params[0]]->getMode();
			answerClient(fd, RPL_CHANNELMODEIS, msg.params[0], modes);
			return ;
		}
		else 
		{
			if (_clients[fd]->getNickname() != msg.params[0]) 
			{
				answerClient(fd, ERR_USERSDONTMATCH, "", "Cannot change modes of other users 1");
				return ;
			}
			// answerClient(fd, RPL_UMODEIS, _clients[fd]->getNickname(), "modes");
			return ;
		}
	}	

	if (msg.params[0][0] == '#' || msg.params[0][0] == '&') 
	{
		if (_channel.find(msg.params[0]) == _channel.end()) 
		{
			answerClient(fd, ERR_NOSUCHCHANNEL, msg.params[0], "No such channel 2");
			return ;
		}
	}
	else 
	{
		if (_clients[fd]->getNickname() != msg.params[0]) 
		{
			answerClient(fd, ERR_USERSDONTMATCH, "", "Cannot change modes of other users 2");
			return ;
		}
		answerClient(fd, RPL_CHANNELMODEIS, _clients[fd]->getNickname(), "modes");
		return ;
	}

	if (msg.params[0] == "#general" || msg.params[0] == "&general") {
		answerClient(fd, ERR_CHANOPRIVSNEEDED, msg.params[0], "Cannot change modes of the general channel");
		return ;
	}

	if (!_channel[msg.params[0]]->isAdmin(fd)) {
		answerClient(fd, ERR_CHANOPRIVSNEEDED, msg.params[0], "You're not channel operator");
		return ;
	}
	

	std::vector<std::string> modes;
	std::vector<std::string> params;
	size_t modeCount = 0;
	for (size_t i = 1; i < msg.params.size(); ++i) 
	{
		if ((msg.params[i][0] == '+' || msg.params[i][0] == '-') && msg.params[i].size() > 1) 
		{
			std::string sign;
			bool valid = false;
			for (size_t j = 0; j < msg.params[i].size(); ++j) 
			{
				valid = false;
				if (msg.params[i][j] == '+' || msg.params[i][j] == '-') 
				{
					sign = msg.params[i][j];
					continue ;
				}
				if (isalpha(msg.params[i][j])) 
				{
					if (sign.empty()) 
					{
						answerClient(fd, ERR_NEEDMOREPARAMS, "MODE", "Not enough parameters for mode");
						return ;
					}
					if (msg.params[i][j] == 'k' || msg.params[i][j] == 'l' || msg.params[i][j] == 'i'
						|| msg.params[i][j] == 't' || msg.params[i][j] == 'o' || msg.params[i][j] == 'b') {
						if (((msg.params[i][j] == 'k' || msg.params[i][j] == 'l') && sign == "+")
							||  msg.params[i][j] == 'o' || msg.params[i][j] == 'b')
							modeCount++;
						valid = true;
						modes.push_back(sign + msg.params[i][j]);
					}
					else {
						std::string character;
						character += msg.params[i][j];
						answerClient(fd, ERR_UNKNOWNMODE, character, "Unknown mode");
						return ;
					}
				}
			}
			if (!valid) {
				answerClient(fd, ERR_NEEDMOREPARAMS, "MODE", "Sign must be followed by a mode");
				return ;
			}
		}
		else if ((msg.params[i][0] == '+' || msg.params[i][0] == '-') && msg.params[i].size() == 1) {
			answerClient(fd, ERR_NEEDMOREPARAMS, "MODE", "Not enough parameters for mode");
			return ;
		}
		else
			params.push_back(msg.params[i]);
	}

	if (modes.empty()) {
		answerClient(fd, RPL_CHANNELMODEIS, msg.params[0], _channel[msg.params[0]]->getMode());
		return ;
	}

	if (params.size() != modeCount) {
		size_t modeB = 0;
		if ((modeCount - params.size()) > 0) {
			for (size_t i = 0; i < modes.size(); ++i) {
				if (modes[i] == "+b")
					modeB++;
			}
		}
		if ((modeCount - params.size()) > modeB) {
			answerClient(fd, ERR_NEEDMOREPARAMS, "MODE", "Not enough parameters for mode");
			return ;
		}
	}

	for (size_t i = 0; i < modes.size(); ++i) {
		if (modes[i][0] == '+')
			manageAddMode(modes[i][1], msg.params[0], params, fd);
		else if (modes[i][0] == '-')
			manageRemoveMode(modes[i][1], msg.params[0], params, fd);
		else {
			answerClient(fd, ERR_NEEDMOREPARAMS, "MODE", "Mode must start with + or -");
			return ;
		}
	}
	*/
}

// Método de la clase void Server que realiza la operación principal asociada.
void Server::manageAddMode(char mode, const std::string &channel, std::vector<std::string> &params, int fd) 
{
	// if (mode == 'l') 
	// {
	// 	size_t numberUsers = strtol(params[0].c_str(), NULL, 10);
	// 	if (numberUsers <= 0) {
	// 		answerClient(fd, ERR_NEEDMOREPARAMS, "MODE", "Max users must be a positive number");
	// 		return ;
	// 	}
	// 	if (numberUsers < _channel[channel]->getUserCount()) {
	// 		answerClient(fd, ERR_NEEDMOREPARAMS, "MODE", "Max users cannot be less than current users");
	// 		return ;
	// 	}
	// 	_channel[channel]->setMaxUsers(numberUsers);
	// 	if (!_channel[channel]->hasMode(mode)) 
	// 		_channel[channel]->setMode('l');
	// 	_channel[channel]->broadcastMessage(fd, "MODE", "", "+l " + to_string(numberUsers));
	// 	sendMsgToClient(fd, "MODE", channel, "+l " + to_string(numberUsers));
	// 	params.erase(params.begin());
	// }

	//Los modos que no tiene un parámetro extra además de la letra +i/+t
	if (_channel[channel]->hasMode(mode) && (mode == 'i' || mode == 't' )) 
	{
		answerClient(fd, ERR_NEEDMOREPARAMS, "MODE", "Channel already has mode +" + std::string(1, mode));
		return ;
	}
	
	if (mode == 'i') 
	{
		_channel[channel]->setMode('i');
		_channel[channel]->broadcastMessage(fd, "MODE", "", "+i");
		sendMsgToClient(fd, "MODE", channel, "+i");
		return ;
	}
	if (mode == 't') 
	{
		_channel[channel]->setMode('t');
		_channel[channel]->broadcastMessage(fd, "MODE", "", "+t");
		sendMsgToClient(fd, "MODE", channel, "+t");
		return ;
	}


	//Para el resto de modos se comprueba si se ha informado el parametro exttra
	if (params.size() < 1) 
	{
		answerClient(fd, ERR_NEEDMOREPARAMS, "MODE", "Not enough parameters for mode +" + std::string(1, mode));
		return ;
	}
	if (params[0].empty()) 
	{
		answerClient(fd, ERR_NEEDMOREPARAMS, "MODE", "Param cannot be empty for mode +" + std::string(1, mode));
		return ;
	}

	if (mode == 'b') 
	{
		if (params.empty())
		{
			answerClient(fd, RPL_BANLIST, channel, _channel[channel]->listBanned());
			answerClient(fd, RPL_ENDOFBANLIST, channel, "End of /BAN list");
			return ;
		}

		int userFd = _channel[channel]->getUserFd(params[0]);
		if (userFd == -1) 
		{
			answerClient(fd, ERR_NOSUCHNICK, params[0],  params[0] + " doesn't belong to " + channel +" channel.");
			return ;
		}

		if (_channel[channel]->isBanned(userFd)) 
		{
			answerClient(fd, ERR_CHANOPRIVSNEEDED, channel, "User is already banned");
			return ;
		}

		Client *client = _clients[userFd];
		_channel[channel]->addBannedList(client);
		_channel[channel]->broadcastMessage(fd, "MODE", client->getNickname(), "+b " + client->getNickname());
		sendMsgToClient(fd, "MODE", channel, "+b " + client->getNickname());
		params.erase(params.begin());
		return ;
	}
	if (mode == 'k') 
	{
		_channel[channel]->setPass(params[0]);
		if (!_channel[channel]->hasMode(mode)) 
			_channel[channel]->setMode('k');
		_channel[channel]->broadcastMessage(fd, "MODE", "", "+k " + params[0]);
		sendMsgToClient(fd, "MODE", channel, "+k " + params[0]);
		params.erase(params.begin());
	}
	
	//DMK COMENTADO Y PASADO AL INICIO PARA QUE NO OBLIGUE A QUITAR Y VOLVER A PONER +l cada vez que quieres cambiar de límite
	if (mode == 'l') 
	{
		size_t numberUsers = strtol(params[0].c_str(), NULL, 10);
		if (numberUsers <= 0) 
		{
			answerClient(fd, ERR_NEEDMOREPARAMS, "MODE", "Max users must be a positive number");
			return ;
		}

		if (numberUsers < _channel[channel]->getUserCount()) 
		{
			answerClient(fd, ERR_NEEDMOREPARAMS, "MODE", "Max users cannot be less than current users");
			return ;
		}

		_channel[channel]->setMaxUsers(numberUsers);
		if (!_channel[channel]->hasMode(mode)) 
			_channel[channel]->setMode('l');
		_channel[channel]->broadcastMessage(fd, "MODE", "", "+l " + to_string(numberUsers));
		sendMsgToClient(fd, "MODE", channel, "+l " + to_string(numberUsers));
		params.erase(params.begin());
	}

	if (mode == 'o') 
	{
		int userFd = _channel[channel]->getUserFd(params[0]);
		if (userFd == -1) {
			answerClient(fd, ERR_NOSUCHNICK, params[0], "No such nick");
			return ;
		}
		if (_channel[channel]->isAdmin(userFd)) {
			answerClient(fd, ERR_CHANOPRIVSNEEDED, channel, "User is already admin");
			return ;
		}
		Client *client = _clients[userFd];
		_channel[channel]->addAdminList(client);
		_channel[channel]->broadcastMessage(fd, "MODE", client->getNickname(), "+o " + client->getNickname());
		sendMsgToClient(fd, "MODE", channel, "+o " + client->getNickname());
		msgClientToClient(fd, userFd, "MODE " + channel + " +o " + client->getNickname(), "");
		params.erase(params.begin());
	}
}

// Método de la clase void Server que realiza la operación principal asociada.
void Server::manageRemoveMode(char mode, const std::string &channel, std::vector<std::string> &params, int fd) {
	if (mode == 'i' || mode == 't' || mode == 'k' || mode == 'l') {
		if (!_channel[channel]->hasMode(mode)) {
			answerClient(fd, ERR_NEEDMOREPARAMS, "MODE", "Channel doesn't have mode -" + std::string(1, mode));
			return ;
		}
		if (mode == 'i')
			_channel[channel]->unsetMode('i');
		if (mode == 't')
			_channel[channel]->unsetMode('t');
		if (mode == 'k') {
			_channel[channel]->setPass("");
			_channel[channel]->unsetMode('k');
		}
		if (mode == 'l') {
			_channel[channel]->setMaxUsers(0);
			_channel[channel]->unsetMode('l');
		}
		_channel[channel]->broadcastMessage(fd, "MODE", "", "-" + std::string(1, mode));
		sendMsgToClient(fd, "MODE", channel, "-" + std::string(1, mode));
		return ;
	}

	if (params.size() < 1) {
		answerClient(fd, ERR_NEEDMOREPARAMS, "MODE", "Not enough parameters for mode -" + std::string(1, mode));
		return ;
	}
	if (params[0].empty()) {
		answerClient(fd, ERR_NEEDMOREPARAMS, "MODE", "Param cannot be empty for mode -" + std::string(1, mode));
		return ;
	}

	if (mode == 'o') {
		int userFd = _channel[channel]->getUserFd(params[0]);
		if (userFd == -1) {
			answerClient(fd, ERR_NOSUCHNICK, params[0], "No such nick");
			return ;
		}
		if (!_channel[channel]->isAdmin(userFd)) {
			answerClient(fd, ERR_CHANOPRIVSNEEDED, channel, "User is not admin");
			return ;
		}
		_channel[channel]->removeAdminList(_clients[userFd]);
		_channel[channel]->broadcastMessage(fd, "MODE", _clients[userFd]->getNickname(), "-o " + _clients[userFd]->getNickname());
		sendMsgToClient(fd, "MODE", channel, "-o " + _clients[userFd]->getNickname());
		msgClientToClient(fd, userFd, "MODE " + channel + " -o " + _clients[userFd]->getNickname(), "");
		params.erase(params.begin());
	}
	if (mode == 'b') {
		int userFd = _channel[channel]->getUserFd(params[0]);
		if (userFd == -1) {
			answerClient(fd, ERR_NOSUCHNICK, params[0], "No such nick");
			return ;
		}
		if (!_channel[channel]->isBanned(userFd)) {
			answerClient(fd, ERR_CHANOPRIVSNEEDED, channel, "User is not banned");
			return ;
		}
		_channel[channel]->removeBannedList(_clients[userFd]);
		_channel[channel]->broadcastMessage(fd, "MODE", _clients[userFd]->getNickname(), "-b " + _clients[userFd]->getNickname());
		sendMsgToClient(fd, "MODE", channel, "-b " + _clients[userFd]->getNickname());
		params.erase(params.begin());
	}
}
