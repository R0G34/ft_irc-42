#pragma once

#ifndef CHANNEL_HPP
# define CHANNEL_HPP






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


		Channel(const std::string &name);

		Channel(const std::string &name, const std::string &pass, const int &maxUsers);

		~Channel();


		const std::string &getName() const;

		const std::string &getPass() const;

		const size_t &getMaxUsers() const;

		const std::string &getTopic() const;

		size_t getUserCount() const;

		int getUserFd(const std::string &nick) const;

		std::string getMode() const;


		void setName(const std::string &name);

		void setMaxUsers(const size_t &maxUsers);

		void setPass(const std::string &pass);

		void setTopic(const std::string &topic);

		void setMode(const char &mode);

		void unsetMode(const char &mode);


		bool hasMode(const char &mode) const;

		bool hasUser(int fd) const;

		bool hasUser(const std::string &nick) const;

		bool isBanned(int fd) const;

		bool isInvited(int fd) const;

		bool isAdmin(int fd) const;


		std::string listUsers();

		std::string listBanned();

		void addInvitedList(Client *client);

		void removeInvitedList(Client *client);

		void addBannedList(Client *client);

		void removeBannedList(Client *client);

		void addAdminList(Client *client);

		void removeAdminList(Client *client);

		void newChannelUser(Client *client);

		void disconnectUser(Client *client);

		void broadcastMessage(int fd, const std::string &cmd, const std::string &user, const std::string &msg) const;

		void broadcastMessageNochan(int fd, const std::string &cmd, const std::string &msg) const;
};

#endif 