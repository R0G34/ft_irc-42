#pragma once

#ifndef CHANNEL_HPP
# define CHANNEL_HPP

// ======================================================================
// Archivo: Channel.hpp
// Propósito: Declara la clase Channel, que representa un canal de IRC y gestiona usuarios, modos y mensajes.
// ======================================================================

# include <irc.hpp>
# include <set>
# include <Client.hpp>

class Client;

class Channel
{
	private:
		size_t					_maxUsers;
		std::string				_name;
		std::string				_pass;
		std::string				_topic;
		int						_ownerFd;
		
		std::set<char>			_mode;
		std::map<int, Client *>	_banned;
		std::map<int, Client *>	_invited;
		std::map<int, Client *>	_users;
		std::map<int, Client *>	_admins;

	public:

		// Función auxiliar que realiza la operación principal asociada.
		Channel(const std::string &name);
		// Función auxiliar que realiza la operación principal asociada.
		Channel(const std::string &name, const std::string &pass, const int &maxUsers);
		// Destructor de la clase Channel.
		~Channel();

		// Devuelve el atributo correspondiente.
		const std::string &getName() const;
		// Devuelve el atributo correspondiente.
		const std::string &getPass() const;
		// Devuelve el atributo correspondiente.
		const size_t &getMaxUsers() const;
		// Devuelve el atributo correspondiente.
		const std::string &getTopic() const;
		// Devuelve el atributo correspondiente.
		size_t getUserCount() const;
		// Devuelve el atributo correspondiente.
		int getUserFd(const std::string &nick) const;
		// Devuelve el atributo correspondiente.
		std::string getMode() const;

		// Actualiza el atributo correspondiente.
		void setName(const std::string &name);
		// Actualiza el atributo correspondiente.
		void setMaxUsers(const size_t &maxUsers);
		// Actualiza el atributo correspondiente.
		void setPass(const std::string &pass);
		// Actualiza el atributo correspondiente.
		void setTopic(const std::string &topic);
		// Actualiza el atributo correspondiente.
		void setMode(const char &mode);
		// Función auxiliar que realiza la operación principal asociada.
		void unsetMode(const char &mode);

		// Función auxiliar que realiza la operación principal asociada.
		bool hasMode(const char &mode) const;
		// Función auxiliar que realiza la operación principal asociada.
		bool hasUser(int fd) const;
		// Función auxiliar que realiza la operación principal asociada.
		bool hasUser(const std::string &nick) const;
		// Comprueba o valida una condición concreta.
		bool isBanned(int fd) const;
		// Comprueba o valida una condición concreta.
		bool isInvited(int fd) const;
		// Comprueba o valida una condición concreta.
		bool isAdmin(int fd) const;

		// Devuelve una representación en texto de los elementos correspondientes.
		std::string listUsers();
		// Devuelve una representación en texto de los elementos correspondientes.
		std::string listBanned();
		// Añade elementos o información a las estructuras internas.
		void addInvitedList(Client *client);
		// Elimina elementos o información de las estructuras internas.
		void removeInvitedList(Client *client);
		// Añade elementos o información a las estructuras internas.
		void addBannedList(Client *client);
		// Elimina elementos o información de las estructuras internas.
		void removeBannedList(Client *client);
		// Añade elementos o información a las estructuras internas.
		void addAdminList(Client *client);
		// Elimina elementos o información de las estructuras internas.
		void removeAdminList(Client *client);
		// Función auxiliar que realiza la operación principal asociada.
		void newChannelUser(Client *client);
		// Función auxiliar que realiza la operación principal asociada.
		void disconnectUser(Client *client);
		// Envía un mensaje a todos los clientes afectados.
		void broadcastMessage(int fd, const std::string &cmd, const std::string &user, const std::string &msg) const;
		// Envía un mensaje a todos los clientes afectados.
		void broadcastMessageNochan(int fd, const std::string &cmd, const std::string &msg) const;
};

#endif 